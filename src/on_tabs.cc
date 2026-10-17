#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <process.h>
#include <uiautomation.h>
#include <objbase.h>
#include "config.h"
#include "utils.h"

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define ON_CLOSED_FLAGS 998
#define ON_BUTTON_FLAGS 999

static int  mouse_time = 0;
static bool fn_chrome = false;
static bool tab_event = false;
static bool right_click = false;
static bool left_click = false;
static IUIAutomation *g_uia = NULL;
static HHOOK message_hook = NULL, mouse_hook = NULL;

static void
send_key_click(int mouse)
{
    if (mouse == MOUSEEVENTF_MIDDLEDOWN)
    {
    #if defined(__GNUC__) || defined(__clang__)
        INPUT input[] =
        {
            { INPUT_MOUSE, {.mi = { 0, 0, 0, MOUSEEVENTF_MIDDLEDOWN, 0, 0 } } },
            { INPUT_MOUSE, {.mi = { 0, 0, 0, MOUSEEVENTF_MIDDLEUP, 0, 0 } } }
        };
    #else
        INPUT input[] =
        {
            { INPUT_MOUSE, {0, } },
            { INPUT_MOUSE, {0, } }
        };
        input[0].mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
        input[1].mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
    #endif
        SendInput(sizeof(input) / sizeof(INPUT), input, sizeof(INPUT));
    }
    else if (mouse == WM_NCLBUTTONDBLCLK)
    {
    #if defined(__GNUC__) || defined(__clang__)
        INPUT input[] =
        {
            { INPUT_KEYBOARD, {.ki = { VK_RCONTROL, 0, 0, 0, 0 } } },
            { INPUT_KEYBOARD, {.ki = { 'T', 0, 0, 0, 0 } } },
            { INPUT_KEYBOARD, {.ki = { 'T', 0, KEYEVENTF_KEYUP, 0, 0 } } },
            { INPUT_KEYBOARD, {.ki = { VK_RCONTROL, 0, KEYEVENTF_KEYUP, 0, 0 } } }
        };
    #else
        INPUT input[] =
        {
            { INPUT_KEYBOARD, {0, } },
            { INPUT_KEYBOARD, {0, } },
            { INPUT_KEYBOARD, {0, } },
            { INPUT_KEYBOARD, {0, } }
        };
        input[0].ki.wVk = input[3].ki.wVk = VK_RCONTROL;
        input[1].ki.wVk = input[2].ki.wVk = 'T';
        input[2].ki.dwFlags = input[3].ki.dwFlags = KEYEVENTF_KEYUP;
    #endif
        SendInput(sizeof(input) / sizeof(INPUT), input, sizeof(INPUT));
    }
    else if (mouse == WM_NCRBUTTONDOWN)
    {
    #if defined(__GNUC__) || defined(__clang__)
        INPUT input[] =
        {
            { INPUT_KEYBOARD, {.ki = { VK_RCONTROL, 0, 0, 0, 0 } } },
            { INPUT_KEYBOARD, {.ki = { VK_RSHIFT, 0, 0, 0, 0 } } },
            { INPUT_KEYBOARD, {.ki = { 'T', 0, 0, 0, 0 } } },
            { INPUT_KEYBOARD, {.ki = { VK_RCONTROL, 0, KEYEVENTF_KEYUP, 0, 0 } } },
            { INPUT_KEYBOARD, {.ki = { VK_RSHIFT, 0, KEYEVENTF_KEYUP, 0, 0 } } },
            { INPUT_KEYBOARD, {.ki = { 'T', 0, KEYEVENTF_KEYUP, 0, 0 } } }
        };
    #else
        INPUT input[] =
        {
            { INPUT_KEYBOARD, {0, } },
            { INPUT_KEYBOARD, {0, } },
            { INPUT_KEYBOARD, {0, } },
            { INPUT_KEYBOARD, {0, } },
            { INPUT_KEYBOARD, {0, } },
            { INPUT_KEYBOARD, {0, } }
        };
        input[0].ki.wVk = input[3].ki.wVk = VK_RCONTROL;
        input[1].ki.wVk = input[4].ki.wVk = VK_RSHIFT;
        input[2].ki.wVk = input[5].ki.wVk = 'T';
        input[3].ki.dwFlags = input[4].ki.dwFlags = input[5].ki.dwFlags = KEYEVENTF_KEYUP;
    #endif
        SendInput(sizeof(input) / sizeof(INPUT), input, sizeof(INPUT));
    }
}

static void
send_click(int mouse)
{
    static uint32_t pre_rec = 0;
    if (mouse == MOUSEEVENTF_LEFTDOWN)
    {
    #if defined(__GNUC__) || defined(__clang__)
        INPUT input[] =
        {
            { INPUT_MOUSE, {.mi = { 0, 0, 0, MOUSEEVENTF_LEFTDOWN, 0, 0 } } },
            { INPUT_MOUSE, {.mi = { 0, 0, 0, MOUSEEVENTF_LEFTUP, 0, 0 } } }
        };
    #else
        INPUT input[] =
        {
            { INPUT_MOUSE, {0, } },
            { INPUT_MOUSE, {0, } }
        };
        input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    #endif
        pre_rec = GetTickCount();
        SendInput(sizeof(input) / sizeof(INPUT), input, sizeof(INPUT));
    }
    else
    {
        uint32_t m_rec = GetTickCount();
        if (!left_click && m_rec - pre_rec > (uint32_t) GetDoubleClickTime())
        {
            pre_rec = m_rec;
            send_key_click(MOUSEEVENTF_MIDDLEDOWN);
        }
        else if (left_click)
        {
            static uint32_t pre_left = 0;
            if (m_rec - pre_left > (uint32_t) GetDoubleClickTime())
            {
                pre_left = m_rec;
                send_key_click(MOUSEEVENTF_MIDDLEDOWN);
            }
        }
    }
}

class ScopedBstr {
 public:
  ScopedBstr() = default;
  ~ScopedBstr() { SysFreeString(bstr_); }
  ScopedBstr(const ScopedBstr&) = delete;
  ScopedBstr& operator=(const ScopedBstr&) = delete;

  BSTR* Receive() { return &bstr_; }
  BSTR Get() const { return bstr_ == nullptr ? const_cast<BSTR>(L"") : bstr_; }
  UINT Length() const { return bstr_ == nullptr ? 0 : SysStringLen(bstr_); }
  explicit operator bool() const { return bstr_ != nullptr; }

 private:
  BSTR bstr_ = nullptr;
};

static HRESULT
find_next_child(IUIAutomationElement *pElement, long uia_id, IUIAutomationElement **pout)
{
    HRESULT hr = -1;
    IUIAutomationCondition *pCondition = NULL;
    do
    {
        VARIANT var;
        var.vt = VT_I4;
        var.lVal = uia_id;
        if (!pElement || !g_uia)
        {
            DebugLog(L"find_next_child Failed, cause exist nullptr");
            break;
        }
        hr = g_uia->CreatePropertyCondition(UIA_ControlTypePropertyId, var, &pCondition);
        if (FAILED(hr))
        {
            DebugLog(L"find_next_child, CreatePropertyCondition false");
            break;
        }
        hr = pElement->FindFirst(TreeScope_Children, pCondition, pout);
        if (FAILED(hr))
        {
            DebugLog(L"find_next_child, FindFirst false");
        }
    } while (0);
    if (pCondition)
    {
        pCondition->Release();
    }
    return hr;
}

static HRESULT
get_botton_flags(IUIAutomationElement *tab_bar, const POINT *pt, int *active, int dv)
{
    /* 获取关闭标签按钮 */
    IUIAutomationElement *botton = NULL;
    HRESULT hr = find_next_child(tab_bar, UIA_ButtonControlTypeId, &botton);
    if (SUCCEEDED(hr))
    {
        if (botton)
        {
            ScopedBstr name;
            hr = botton->get_CurrentClassName(name.Receive());
            if (SUCCEEDED(hr) && wcscmp(name.Get(), fn_chrome ? L"TabCloseButton" : L"EdgeTabCloseButton") == 0)
            {
                RECT rc;
                hr = botton->get_CurrentBoundingRectangle(&rc);
                *active = (SUCCEEDED(hr) && PtInRect(&rc, *pt)) ? dv : 0;
            #ifdef _DEBUG
                DebugLog(L"get_CurrentBoundingRectangle ok, *active = {}", *active);
            #endif
            }
            botton->Release();
        }
        else
        {
            *active = 0;
        #ifdef _DEBUG
            DebugLog(L"Maybe fixed tabs");
        #endif
        }
    }
    return hr;
}

static bool
get_hv_bars(IUIAutomationElement *ice_root, IUIAutomationElementArray **tabs)
{
    bool ret = false;
    IUIAutomationCondition* pCondition = NULL;
    VARIANT var = {};
    var.vt = VT_BSTR;
    var.bstrVal = SysAllocString(fn_chrome ? L"TabView" : L"EdgeTab");
    do
    {
        int count = 0;
        HRESULT hr = g_uia->CreatePropertyCondition(UIA_ClassNamePropertyId, var, &pCondition);
        if (FAILED(hr))
        {
            DebugLog(L"CreatePropertyCondition UIA_ClassNamePropertyId error: {}", GetLastError());
            break;
        }
        hr = ice_root->FindAll(TreeScope_Subtree, pCondition, tabs);
        if (FAILED(hr) || *tabs == NULL)
        {
            DebugLog(L"get_hv_bars, FindAll error");
            break;
        }
        (*tabs)->get_Length(&count);
        ret = count > 0;
    } while(0);
    if (pCondition)
    {
        pCondition->Release();
    }
    if (var.bstrVal)
    {
        SysFreeString(var.bstrVal);
    }
    return ret;
}

static bool
get_hv_bar2(IUIAutomationElement *ice_root, IUIAutomationElement **tab_bar, const VARIANT *pvar, const WCHAR *cname)
{
    bool ret = false;
    IUIAutomationCondition* pCondition = NULL;
    IUIAutomationElementArray* pElementFound = NULL;
    if (!g_uia || !ice_root || !pvar || !cname)
    {
        return false;
    }
    do
    {
        int count = 0;
        HRESULT hr = g_uia->CreatePropertyCondition(UIA_ControlTypePropertyId, *pvar, &pCondition);
        if (FAILED(hr) || !pCondition)
        {
            DebugLog(L"get_hv_bar2, CreatePropertyCondition error");
            break;
        }
        hr = ice_root->FindAll(TreeScope_Subtree, pCondition, &pElementFound);
        if (FAILED(hr) || !pElementFound)
        {
            DebugLog(L"get_hv_bar2, FindAll error");
            break;
        }
        pElementFound->get_Length(&count);
        for (int i = 0; i < count; ++i)
        {
            ScopedBstr name;
            IUIAutomationElement* tmp = NULL;
            hr = pElementFound->GetElement(i, &tmp);
            if (FAILED(hr) || !tmp)
            {
                DebugLog(L"get_hv_bar2, GetElement error");
                break;
            }
            hr = tmp->get_CurrentClassName(name.Receive());
            if (SUCCEEDED(hr) && wcscmp(name.Get(), cname) == 0)
            {
                *tab_bar = tmp;
                ret = true;
                break;
            }
            tmp->Release();
        }
    } while(0);
    if (pCondition)
    {
        pCondition->Release();
    }
    if (pElementFound)
    {
        pElementFound->Release();
    }
    return ret;
}

static HRESULT
get_tab_bars(IUIAutomationElement **tab_bar, HWND hwnd, bool *pv)
{
    HRESULT hr = -1;
    IUIAutomationCondition *pCondition = NULL;
    IUIAutomationElement *ice_root = NULL;
    if (!g_uia || !hwnd)
    {
        return hr;
    }
    if (!IsWindow(hwnd))
    {
    #ifdef _DEBUG
        WCHAR classname[MAX_PATH] = {0};
        GetClassName(hwnd, classname, MAX_PATH - 1);
        DebugLog(L"get_tab_bars, {} not window", classname);
    #endif
        return hr;
    }
    do
    {
        VARIANT var = {};
        var.vt = VT_I4;
        var.lVal = UIA_TabControlTypeId;
        hr = g_uia->ElementFromHandle(hwnd, &ice_root);
        if (hr != S_OK || !ice_root)
        {
            DebugLog(L"ElementFromHandle failed or ice_root is null");
            hr = -1;
            break;
        }
        hr = get_hv_bar2(ice_root, tab_bar, &var, fn_chrome ? L"VerticalTabStripRegionView" : L"EdgeVerticalTabContainerView") ? 0 : -1;
        if (FAILED(hr))
        {
            hr = get_hv_bar2(ice_root, tab_bar, &var, fn_chrome ? L"TabContainerImpl" : L"EdgeTabContainerImpl") ? 0 : -1;
        }
        else if (pv)
        {
            *pv = true;
        }
    #ifdef _DEBUG
        else
        {
            DebugLog(L"get_hv_bar2 can not find Tabbar?");
        }
    #endif
    } while (0);
    if (pCondition)
    {
        pCondition->Release();
    }
    if (ice_root)
    {
        ice_root->Release();
    }
    return hr;
}

/* 得到标签页的事件指针, 当标签没激活时把active参数设为标签序号 */
static bool
mouse_on_tab(RECT *pr, const POINT *pt, int *active)
{
    IUIAutomationCondition *pCondition = NULL;
    IUIAutomationElementArray *pFoundArray = NULL;
    IUIAutomationElement *tab_bar = NULL;
    bool res = false;
    do
    {
        int idx;
        int c = 0;
        bool pv = false;
        HWND hwnd = WindowFromPoint(*pt);
        HRESULT hr = get_tab_bars(&tab_bar, hwnd, &pv);
        if (FAILED(hr) || !tab_bar || !g_uia)
        {
            break;
        }
        if (!pv)
        {
            VARIANT var = {};
            var.vt = VT_I4;
            var.lVal = UIA_TabItemControlTypeId;
            hr = g_uia->CreatePropertyCondition(UIA_ControlTypePropertyId, var, &pCondition);
            if (FAILED(hr))
            {
                DebugLog(L"mouse_on_tab, CreatePropertyCondition false!");
                break;
            }
            hr = tab_bar->FindAll(TreeScope_Children, pCondition, &pFoundArray);
            if (FAILED(hr))
            {
                DebugLog(L"mouse_on_tab, FindAll false!");
                break;
            }
        }
        else if (!get_hv_bars(tab_bar, &pFoundArray))
        {
            DebugLog(L"mouse_on_tab, get_hv_bars false!");
            break;
        }
        if (pFoundArray)
        {
            hr = pFoundArray->get_Length(&c);
            if (FAILED(hr) || c == 0 || c > 1024)
            {
                DebugLog(L"mouse_on_tab, get_Length false, c = {}", c);
                break;
            }
        #ifdef _DEBUG
            if (pv)
            {
                DebugLog(L"mouse_on_tab, c = {}", c);
            }
        #endif
        }
        for (idx = 0; idx < c; ++idx)
        {
            IUIAutomationElement *tmp = NULL;
            hr = pFoundArray->GetElement(idx, &tmp);
            if (SUCCEEDED(hr) && tmp)
            {
                RECT rc;
                hr = tmp->get_CurrentBoundingRectangle(&rc);
                if (SUCCEEDED(hr) && PtInRect(&rc, *pt))
                {
                    if (pr != NULL)
                    {
                        *pr = rc;
                    }
                    if (active != NULL)
                    {
                        BOOL sel = TRUE;
                        IUnknown *m_pattern = NULL;
                        hr = tmp->GetCurrentPattern(UIA_SelectionItemPatternId, &m_pattern);
                        if (FAILED(hr) || !m_pattern)
                        {
                            DebugLog(L"mouse_on_tab, GetCurrentPattern failed!");
                            tmp->Release();
                            break;
                        }
                        hr = ((IUIAutomationSelectionItemPattern *)m_pattern)->get_CurrentIsSelected(&sel);
                        m_pattern->Release();
                        if (FAILED(hr))
                        {
                            DebugLog(L"mouse_on_tab, get_CurrentIsSelected failed!");
                            tmp->Release();
                            break;
                        }
                        if (!sel)
                        {
                            hr = get_botton_flags(tmp, pt, active, ON_CLOSED_FLAGS);
                            if (FAILED(hr))
                            {
                                RECT rc_dup = {0};
                            #ifdef _DEBUG
                                DebugLog(L"Maybe not closed button");
                            #endif
                                memcpy(&rc_dup, &rc, sizeof(RECT));
                                rc_dup.right -= 26;
                                if (PtInRect(&rc_dup, *pt))
                                {
                                    *active = idx + 1;
                                }
                                else
                                {
                                    *active = ON_CLOSED_FLAGS;
                                }
                            }
                            else if (*active == 0)
                            {
                                *active = idx + 1;
                            }
                        }
                    }
                    res = true;
                }
                tmp->Release();
                if (res)
                {
                    break;
                }
            }
        }
    } while (0);
    if (pCondition)
    {
        pCondition->Release();
    }
    if (tab_bar)
    {
        tab_bar->Release();
    }
    if (pFoundArray)
    {
        pFoundArray->Release();
    }
    return res;
}

static LRESULT CALLBACK
message_function(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        MSG   *msg = (MSG *) lParam;
        switch (msg->message)
        {
            case WM_MOUSEHOVER:
            {
                if (tab_event)
                {
                    int active = 0;
                    if (mouse_on_tab(NULL, &msg->pt, &active))
                    {
                        if (active > 0 && active != ON_CLOSED_FLAGS)
                        {
                        #ifdef _DEBUG
                            DebugLog(L"mouse on inactive tab {}", active);
                        #endif
                            send_click(MOUSEEVENTF_LEFTDOWN);
                        }
                    }
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }
    return CallNextHookEx(message_hook, nCode, wParam, lParam);
}

static LRESULT CALLBACK
mouse_function(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        PMOUSEHOOKSTRUCT pmouse = (PMOUSEHOOKSTRUCT) lParam;
        if (GetWindowThreadProcessId(pmouse->hwnd, NULL) != GetCurrentThreadId())
        {
            return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
        }
        switch (wParam)
        {
            case WM_MOUSEMOVE:
            {
                if (tab_event)
                {
                    TRACKMOUSEEVENT MouseEvent;
                    MouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
                    MouseEvent.dwFlags = TME_HOVER | TME_LEAVE;
                    MouseEvent.hwndTrack = pmouse->hwnd;
                    MouseEvent.dwHoverTime = mouse_time;
                    TrackMouseEvent(&MouseEvent);
                }
                break;
            }
            case WM_RBUTTONUP:
            {
                if (!right_click || KEY_DOWN(VK_SHIFT))
                {
                    break;
                }
                if (!mouse_on_tab(NULL, &pmouse->pt, NULL))
                {
                    break;
                }
                send_key_click(MOUSEEVENTF_MIDDLEDOWN);
                return 1;
            }
            default:
            {
                break;
            }
        }
    }
    return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
}

void
un_uia(void)
{
    if (mouse_hook)
    {
        UnhookWindowsHookEx(mouse_hook);
        mouse_hook = NULL;
    }
    if (message_hook)
    {
        UnhookWindowsHookEx(message_hook);
        message_hook = NULL;
    }
    if (g_uia)
    {
        g_uia->Release();
        g_uia = NULL;
    #ifdef _DEBUG
        DebugLog(L"un_uia executing");
    #endif
    }
    CoUninitialize();
}

static bool
init_uia(void)
{
    HRESULT hr = CoInitialize(nullptr);
    if (SUCCEEDED(hr))
    {
        hr = CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER, IID_IUIAutomation, reinterpret_cast<void **>(&g_uia));
    }
    return (SUCCEEDED(hr) && (g_uia != NULL));
}

void threads_on_tabs(void)
{
    bool failed = false;
    tab_event = (mouse_time = config.GetHoverTabDelay()) > 0;
    right_click = config.IsRightClickClose();
    if ((tab_event || right_click) && init_uia())
    {
        const std::wstring& ini = GetIniPath();
        fn_chrome = !(ini.ends_with(L"msedge++.ini") || ini.ends_with(L"msedge++(example).ini"));
        if (tab_event)
        {
            message_hook = SetWindowsHookExW(WH_GETMESSAGE, message_function, hInstance, GetCurrentThreadId());
            if (message_hook == NULL)
            {
                DebugLog(L"SetWindowsHookEx WH_GETMESSAGE failed");
                failed = true;
            }
        }
        if (right_click)
        {
            mouse_hook = failed ? NULL : SetWindowsHookExW(WH_MOUSE, mouse_function, hInstance, GetCurrentThreadId());
            if (mouse_hook == NULL)
            {
                DebugLog(L"SetWindowsHookEx WH_MOUSE failed");
                failed = true;
            }
        }
    #ifdef _DEBUG
        if (!failed)
        {
            DebugLog(L"threads_on_tabs ok, processid = {}", GetCurrentProcessId());
        }
    #endif
    }
}
