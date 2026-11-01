#ifndef COBJMACROS
#define COBJMACROS
#endif

#include "general.h"
#include "ini_parser.h"
#include <stdio.h>
#include <process.h>
#include <uiautomation.h>

#define WAIT_TIMES 9000

static HHOOK message_hook, mouse_hook;
static IUIAutomation *g_uia;
static uint32_t mouse_time;
static bool tab_event;
static bool double_click;
static bool mouse_close;
static bool left_click;
static bool right_click;
static bool left_new;
static bool button_new;
static bool right_double;
static bool click_mousewheel;

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define ON_CLOSED_FLAGS 998
#define ON_BUTTON_FLAGS 999
#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

#define MOUSEEVENTF_WHEEL_DOWN 0x2800
#define MOUSEEVENTF_WHEEL_UP   0x2801

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
    else if (mouse == MOUSEEVENTF_WHEEL_DOWN || mouse == MOUSEEVENTF_WHEEL_UP)
    {
        if (mouse == MOUSEEVENTF_WHEEL_DOWN)
        {
            //SendCtrlTab();
        #if defined(__GNUC__) || defined(__clang__)
            INPUT input[] =
            {
                { INPUT_KEYBOARD, {.ki = { VK_RCONTROL, 0, 0, 0, 0 } } },
                { INPUT_KEYBOARD, {.ki = { VK_TAB, 0, 0, 0, 0 } } },
                { INPUT_KEYBOARD, {.ki = { VK_TAB, 0, KEYEVENTF_KEYUP, 0, 0 } } },
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
            input[1].ki.wVk = input[2].ki.wVk = VK_TAB;
            input[2].ki.dwFlags = input[3].ki.dwFlags = KEYEVENTF_KEYUP;
        #endif
            SendInput(sizeof(input) / sizeof(INPUT), input, sizeof(INPUT));
        }
        else
        {
            //SendCtrlShiftTab();
        #if defined(__GNUC__) || defined(__clang__)
            INPUT input[] =
            {
                { INPUT_KEYBOARD, {.ki = { VK_RCONTROL, 0, 0, 0, 0 } } },
                { INPUT_KEYBOARD, {.ki = { VK_RSHIFT, 0, 0, 0, 0 } } },
                { INPUT_KEYBOARD, {.ki = { VK_TAB, 0, 0, 0, 0 } } },
                { INPUT_KEYBOARD, {.ki = { VK_RCONTROL, 0, KEYEVENTF_KEYUP, 0, 0 } } },
                { INPUT_KEYBOARD, {.ki = { VK_RSHIFT, 0, KEYEVENTF_KEYUP, 0, 0 } } },
                { INPUT_KEYBOARD, {.ki = { VK_TAB, 0, KEYEVENTF_KEYUP, 0, 0 } } }
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
            input[2].ki.wVk = input[5].ki.wVk = VK_TAB;
            input[3].ki.dwFlags = input[4].ki.dwFlags = input[5].ki.dwFlags = KEYEVENTF_KEYUP;
        #endif
            SendInput(sizeof(input) / sizeof(INPUT), input, sizeof(INPUT));
        }
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
        #ifdef _LOGDEBUG
            logmsg("%s false, cause: pElement = 0x%\n", __FUNCTION__, pElement);
        #endif
            break;
        }
        hr = IUIAutomation_CreatePropertyCondition(g_uia, UIA_ControlTypePropertyId, var, &pCondition);
        if (FAILED(hr))
        {
        #ifdef _LOGDEBUG
            logmsg("%s_IUIAutomation_CreatePropertyCondition false, cause: %lu\n", __FUNCTION__, GetLastError());
        #endif
            break;
        }
        hr = IUIAutomationElement_FindFirst(pElement, TreeScope_Children, pCondition, pout);
        if (FAILED(hr))
        {
        #ifdef _LOGDEBUG
            logmsg("%s_IUIAutomationElement_FindFirst false, cause: %lu\n", __FUNCTION__, GetLastError());
        #endif
        }
    } while (0);
    if (pCondition)
    {
        IUIAutomationCondition_Release(pCondition);
    }
    return hr;
}

static bool
get_hv_bars(IUIAutomationElement *ice_root, IUIAutomationElement **tab_bar, const VARIANT *pvar)
{
    bool ret = false;
    IUIAutomationCondition *pCondition = NULL;
    IUIAutomationElementArray *pFoundArray = NULL;
    HRESULT hr = g_uia ? IUIAutomation_CreatePropertyCondition(g_uia, UIA_ControlTypePropertyId, *pvar, &pCondition) : -1;
    do
    {
        int c = 0;
        if (FAILED(hr))
        {
            break;
        }
        hr = IUIAutomationElement_FindAll(ice_root, TreeScope_Children, pCondition, &pFoundArray);
        if (FAILED(hr) || !pFoundArray)
        {
        #ifdef _LOGDEBUG
            logmsg("%s_IUIAutomationElement_FindAll false, cause: %lu\n", __FUNCTION__, GetLastError());
        #endif
            break;
        }
        hr = IUIAutomationElementArray_get_Length(pFoundArray, &c);
        if (FAILED(hr) || !c)
        {
        #ifdef _LOGDEBUG
            logmsg("%s_IUIAutomationElementArray_get_Length false, c = %d!\n", __FUNCTION__, c);
        #endif
            break;
        }
        for (int idx = 0; idx < c; idx++)
        {
            IUIAutomationElement *tmp = NULL;
            hr = IUIAutomationElementArray_GetElement(pFoundArray, idx, &tmp);
            if (SUCCEEDED(hr) && tmp)
            {
                hr = find_next_child(tmp, UIA_TabControlTypeId, tab_bar);
                IUIAutomationElement_Release(tmp);
                if (SUCCEEDED(hr) && (*tab_bar != NULL))
                {
                    ret = true;
                    break;
                }
            }
        }
    } while(0);
    if (pCondition)
    {
        IUIAutomationCondition_Release(pCondition);
    }
    if (pFoundArray)
    {
        IUIAutomationElementArray_Release(pFoundArray);
    }
    return ret;
}

static HRESULT
get_tab_bars(IUIAutomationElement **tab_bar, HWND hwnd, bool *pv)
{
    HRESULT hr = -1;
    IUIAutomationCondition *pCondition = NULL;
    IUIAutomationElementArray *pFoundArray = NULL;
    IUIAutomationElement *ice_root = NULL;
    if (!g_uia || !hwnd)
    {
        return hr;
    }
    do
    {
        VARIANT var = {0};
        var.vt = VT_BSTR;
        var.bstrVal = L"TabsToolbar";
        hr = IUIAutomation_ElementFromHandle(g_uia, hwnd, &ice_root);
        if (FAILED(hr))
        {
        #ifdef _LOGDEBUG
            logmsg("%s_IUIAutomation_ElementFromHandleBuildCache false, cause: %lu\n", __FUNCTION__, GetLastError());
        #endif
            break;
        }
        if (!ice_root)
        {
            hr = -1;
            break;
        }
        if (!get_hv_bars(ice_root, tab_bar, &var))
        {
            fzero(&var, sizeof(VARIANT));
            var.vt = VT_I4;
            var.lVal = UIA_ToolBarControlTypeId;
            if (!get_hv_bars(ice_root, tab_bar, &var))
            {
                fzero(&var, sizeof(VARIANT));
                var.vt = VT_I4;
                var.lVal = UIA_GroupControlTypeId;
                if (!get_hv_bars(ice_root, tab_bar, &var))
                {
                    hr = -1;
                }
                else if (pv)
                {
                    *pv = true;
                }
            }
        }
    } while (0);
    if (pCondition)
    {
        IUIAutomationCondition_Release(pCondition);
    }
    if (pFoundArray)
    {
        IUIAutomationElementArray_Release(pFoundArray);
    }
    if (ice_root)
    {
        IUIAutomationElement_Release(ice_root);
    }
    return hr;
}

static HRESULT
get_botton_flags(IUIAutomationElement *tab_bar, const POINT *pt, int *active, int dv)
{
    /* 获取新建标签按钮 */
    IUIAutomationElement *botton = NULL;
    HRESULT hr = find_next_child(tab_bar, UIA_ButtonControlTypeId, &botton);
    if (SUCCEEDED(hr))
    {
        if (botton)
        {
            RECT rc = {0};
            hr = IUIAutomationElement_get_CurrentBoundingRectangle(botton, &rc);
            *active = (SUCCEEDED(hr) && PtInRect(&rc, *pt)) ? dv : 0;
            IUIAutomationElement_Release(botton);
        }
        else
        {
            *active = 0;
        #ifdef _LOGDEBUG
            logmsg("Maybe fixed tabs");
        #endif
        }
    }
    return hr;
}

static bool
has_first_child(IUIAutomationElement* pElement)
{
    bool ret = false;
    HRESULT hr = -1;
    BSTR bstrName = NULL;
    IUIAutomationTreeWalker* pControlViewWalker = NULL;
    if (!g_uia || !pElement)
    {
        return false;
    }
    hr = IUIAutomation_get_ControlViewWalker(g_uia, &pControlViewWalker);
    if (SUCCEEDED(hr) && pControlViewWalker)
    {
        IUIAutomationElement* tmp = NULL;
        hr = IUIAutomationTreeWalker_GetFirstChildElement(pControlViewWalker, pElement, &tmp);
        if (SUCCEEDED(hr) && tmp)
        {
            hr = IUIAutomationElement_get_CurrentName(tmp, &bstrName);
            if (SUCCEEDED(hr) && bstrName && *bstrName != 0)
            {
                ret = true;
            }
            IUIAutomationElement_Release(tmp);
        }
    }
    if (bstrName)
    {
        SysFreeString(bstrName);
    }
    if (pControlViewWalker) {
        IUIAutomationTreeWalker_Release(pControlViewWalker);
    }
    return ret;
}

static bool
mouse_available_rectangle(const POINT pt)
{
    HRESULT hr = -1;
    bool tab = false, mid = false;
    BSTR bstrClassName = NULL;
    BSTR bstrAutomationId = NULL;
    IUIAutomationElement* pElement = NULL;
    hr = IUIAutomation_ElementFromPoint(g_uia, pt, &pElement);
    if (SUCCEEDED(hr) && pElement)
    {
        if (e_browser_ver < 145)
        {
            CONTROLTYPEID controlTypeId = 0;
            hr = IUIAutomationElement_get_CurrentControlType(pElement, &controlTypeId);
            if (SUCCEEDED(hr) && (controlTypeId == UIA_TabControlTypeId || controlTypeId == UIA_TabItemControlTypeId))
            {
                if (has_first_child(pElement))
                {
                    tab = true;
                }
            }
        }
        else
        {
            hr = IUIAutomationElement_get_CurrentClassName(pElement, &bstrClassName);
            if (SUCCEEDED(hr) && bstrClassName)
            {
                if (!wcsncmp(bstrClassName, L"tabbrowser-tab", 14))
                {
                    tab = true;
                }
                if (mouse_close && !wcscmp(bstrClassName, L"tab-close-button close-icon"))
                {
                    tab = true;
                }
                if (button_new && !wcscmp(bstrClassName, L"toolbarbutton-1"))
                {
                    tab = true;
                }
            }
            hr = IUIAutomationElement_get_CurrentAutomationId(pElement, &bstrAutomationId);
            if (SUCCEEDED(hr) && bstrAutomationId && !wcsncmp(bstrAutomationId, L"tabbrowser-tab", 14))
            {
                mid = true;
            }
        }
    }
    if (pElement)
    {
        IUIAutomationElement_Release(pElement);
    }
    if (bstrClassName)
    {
        SysFreeString(bstrClassName);
    }
    if (bstrAutomationId)
    {
        SysFreeString(bstrAutomationId);
    }
    return (tab || mid);
}

/* 得到标签页的事件指针, 当标签没激活时把active参数设为标签序号 */
static bool
mouse_on_tab(RECT *pr, const POINT *pt, int *active)
{
    IUIAutomationCondition *pCondition = NULL;
    IUIAutomationElementArray *pFoundArray = NULL;
    IUIAutomationElement *tab_bar = NULL;
    IUIAutomationElement *group = NULL;
    bool res = false;
    do
    {
        int idx;
        HRESULT hr;
        VARIANT var;
        HWND hwnd;
        int c = 0;
        var.vt = VT_I4;
        var.lVal = UIA_TabItemControlTypeId;
        hwnd = pt ? WindowFromPoint(*pt) : NULL;
        if (!hwnd || !IsWindow(hwnd) || !is_mozclass(hwnd, NULL) || !g_uia || (e_browser_ver > 144 && !mouse_available_rectangle(*pt)))
        {
        #ifdef _LOGDEBUG
            logmsg("Not in TabBar\n");
        #endif
            return false;
        }
        hr = get_tab_bars(&tab_bar, hwnd, NULL);
        if (FAILED(hr) || !tab_bar)
        {
        #ifdef _LOGDEBUG
            logmsg("get_tab_bars false, tab_bar = 0x%x, hr = %ld!\n", tab_bar, hr);
        #endif
            break;
        }
        if (button_new && active != NULL)
        {
            get_botton_flags(tab_bar, pt, active, ON_BUTTON_FLAGS);
        }
        hr = IUIAutomation_CreatePropertyCondition(g_uia, UIA_ControlTypePropertyId, var, &pCondition);
        if (FAILED(hr))
        {
        #ifdef _LOGDEBUG
            logmsg("CreatePropertyCondition false!\n");
        #endif
            break;
        }
        hr = IUIAutomationElement_FindAll(tab_bar, TreeScope_Children, pCondition, &pFoundArray);
        if (FAILED(hr))
        {
        #ifdef _LOGDEBUG
            logmsg("IUIAutomationElement_FindAll false!\n");
        #endif
            break;
        }
        hr = IUIAutomationElementArray_get_Length(pFoundArray, &c);
        if (FAILED(hr) || c == 0 || c > VALUE_LEN)
        {
        #ifdef _LOGDEBUG
            logmsg("%s_IUIAutomationElementArray_get_Length false, c = %d!\n", __FUNCTION__, c);
        #endif
            break;
        }
        for (idx = 0; idx < c; ++idx)
        {
            IUIAutomationElement *tmp = NULL;
            hr = IUIAutomationElementArray_GetElement(pFoundArray, idx, &tmp);
            if (SUCCEEDED(hr))
            {
                RECT rc;
                hr = IUIAutomationElement_get_CurrentBoundingRectangle(tmp, &rc);
                if (SUCCEEDED(hr))
                {
                    if (!PtInRect(&rc, *pt))
                    {
                        continue;
                    }
                    if (pr != NULL)
                    {
                        *pr = rc;
                    }
                    if (active != NULL)
                    {
                        BOOL sel = TRUE;
                        IUnknown *m_pattern = NULL;
                        hr = IUIAutomationElement_GetCurrentPattern(tmp, UIA_SelectionItemPatternId, &m_pattern);
                        if (FAILED(hr) || !m_pattern)
                        {
                        #ifdef _LOGDEBUG
                            logmsg("%s_IUIAutomationElement_GetCurrentPattern failed!\n", __FUNCTION__);
                        #endif
                            break;
                        }
                        hr = IUIAutomationSelectionItemPattern_get_CurrentIsSelected((IUIAutomationSelectionItemPattern*)m_pattern, &sel);
                        IUIAutomationSelectionItemPattern_Release(m_pattern);
                        if (FAILED(hr))
                        {
                        #ifdef _LOGDEBUG
                            logmsg("%s_IUIAutomationSelectionItemPattern_get_CurrentIsSelected failed!\n", __FUNCTION__);
                        #endif
                            break;
                        }
                        if (!sel || mouse_close)
                        {
                            hr = get_botton_flags(tmp, pt, active, ON_CLOSED_FLAGS);
                            if (FAILED(hr))
                            {
                                RECT rc_dup = {0};
                            #ifdef _LOGDEBUG
                                logmsg("Maybe not closed button\n");
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
                    break;
                }
            }
        }
    } while (0);
    if (pCondition)
    {
        IUIAutomationCondition_Release(pCondition);
    }
    if (pFoundArray)
    {
        IUIAutomationElementArray_Release(pFoundArray);
    }
    if (group)
    {
        IUIAutomationElement_Release(group);
    }
    if (tab_bar)
    {
        IUIAutomationElement_Release(tab_bar);
    }
    return res;
}

/**
 * 鼠标指针在是否垂直标签栏上
 * 在垂直标签上, 返回 1
 * 不在垂直标签上, 返回 0
 * 不处于标签栏, 返回 -1
 */
static int
mouse_on_vtab(const POINT *pt)
{
    IUIAutomationElement *tab_bar = NULL;
    int ret = -1;
    do
    {
        bool v = false;
        HRESULT hr;
        HWND hwnd = pt ? WindowFromPoint(*pt) : NULL;
        if (!hwnd || !IsWindow(hwnd) || !is_mozclass(hwnd, NULL) || !g_uia || (e_browser_ver > 144 && !mouse_available_rectangle(*pt)))
        {
            return ret;
        }
        hr = get_tab_bars(&tab_bar, hwnd, &v);
        if (FAILED(hr) || !tab_bar)
        {
        #ifdef _LOGDEBUG
            logmsg("get_tab_bars false, tab_bar = 0x%x, hr = %ld!\n", tab_bar, hr);
        #endif
            break;
        }
        if (v)
        {
            RECT rc = {0};
            hr = IUIAutomationElement_get_CurrentBoundingRectangle(tab_bar, &rc);
            if (SUCCEEDED(hr) && PtInRect(&rc, *pt))
            {
                ret = 1;
                break;
            }
        }
        ret = 0;
    } while (0);
    if (tab_bar)
    {
        IUIAutomationElement_Release(tab_bar);
    }
    return ret;
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
                if (tab_event || mouse_close || button_new)
                {
                    RECT rc = {0};
                    int active = 0;
                    if (mouse_on_tab(&rc, &msg->pt, &active))
                    {
                        if (tab_event && active > 0 && active != ON_CLOSED_FLAGS)
                        {
                        #ifdef _LOGDEBUG
                            logmsg("mouse on inactive tab[%d]\n", active);
                        #endif
                            send_click(MOUSEEVENTF_LEFTDOWN);
                        }
                        else if (mouse_close && active == ON_CLOSED_FLAGS)
                        {
                        #ifdef _LOGDEBUG
                            logmsg("WM_MOUSEHOVER[mouse on closed botton!]\n");
                        #endif
                            send_click(MOUSEEVENTF_LEFTDOWN);
                        }
                        break;
                    }
                    else if (active == ON_BUTTON_FLAGS)
                    {
                    #ifdef _LOGDEBUG
                        logmsg("WM_MOUSEHOVER[mouse on new botton!]\n");
                    #endif
                        send_click(MOUSEEVENTF_LEFTDOWN);
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
        static bool wheel_tab_with_rbutton = false;
        PMOUSEHOOKSTRUCT pmouse = (PMOUSEHOOKSTRUCT) lParam;
        if (GetWindowThreadProcessId(pmouse->hwnd, NULL) != GetCurrentThreadId())
        {
            return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
        }
        switch (wParam)
        {
            case WM_MOUSEMOVE:
            {
                TRACKMOUSEEVENT MouseEvent;
                MouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
                MouseEvent.dwFlags = TME_HOVER | TME_LEAVE;
                MouseEvent.hwndTrack = pmouse->hwnd;
                MouseEvent.dwHoverTime = mouse_time;
                TrackMouseEvent(&MouseEvent);
                break;
            }
            case WM_MOUSEWHEEL:
            {
                if (click_mousewheel)
                {
                    wheel_tab_with_rbutton = KEY_DOWN(VK_RBUTTON);
                    if (wheel_tab_with_rbutton && !mouse_available_rectangle(pmouse->pt))
                    {
                        const MOUSEHOOKSTRUCTEX * pwheel = (const MOUSEHOOKSTRUCTEX *)(lParam);
                        const int delta = GET_WHEEL_DELTA_WPARAM(pwheel->mouseData);
                        if (delta > 0)
                        {
                            send_key_click(MOUSEEVENTF_WHEEL_UP);
                            PostMessage(pwheel->hwnd, WM_RBUTTONUP, 0, lParam);
                            return 1;
                        }
                        else if (delta < 0)
                        {
                            send_key_click(MOUSEEVENTF_WHEEL_DOWN);
                            PostMessage(pwheel->hwnd, WM_RBUTTONUP, 0, lParam);
                            return 1;
                        }
                    }
                }
                break;
            }
            case WM_NCLBUTTONDBLCLK:
            {
                if (!left_new || KEY_DOWN(VK_SHIFT))
                {
                    break;
                }
                send_key_click(WM_NCLBUTTONDBLCLK);
                return 1;
            }
            case WM_LBUTTONDBLCLK:
            {
                if (!double_click)
                {
                    break;
                }
                if (!mouse_on_tab(NULL, &pmouse->pt, NULL))
                {
                    break;
                }
                if (tab_event)
                {
                    send_click(MOUSEEVENTF_MIDDLEDOWN);
                }
                else
                {
                    send_key_click(MOUSEEVENTF_MIDDLEDOWN);
                }
                return 1;
            }
            case WM_NCRBUTTONDOWN:
            {
                if (!right_double || KEY_DOWN(VK_SHIFT))
                {
                    break;
                }
                send_key_click(WM_NCRBUTTONDOWN);
                return 1;
            }
            case WM_RBUTTONUP:
            {
                int ret = 0;
                if (wheel_tab_with_rbutton)
                {
                    wheel_tab_with_rbutton = false;
                    return 1;
                }
                if (e_browser_ver > 130)
                {
                    ret = mouse_on_vtab(&pmouse->pt);
                }
                if (ret == -1)
                {
                    break;
                }
                if (ret)
                {
                    if (mouse_on_tab(NULL, &pmouse->pt, NULL))
                    {
                        if (!right_click || KEY_DOWN(VK_SHIFT))
                        {
                            break;
                        }
                        if (right_click)
                        {
                            send_key_click(MOUSEEVENTF_MIDDLEDOWN);
                        }
                        break;
                    }
                    if (!right_double || KEY_DOWN(VK_SHIFT))
                    {
                        break;
                    }
                    send_key_click(WM_NCRBUTTONDOWN);
                }
                else 
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
                }
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

void WINAPI
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
        IUIAutomation_Release(g_uia);
        CoUninitialize();
        g_uia = NULL;
    #ifdef _LOGDEBUG
        logmsg("IUIAutomation_Release\n");
    #endif
    }
}

bool WINAPI
on_tabs_reload(void)
{
    ini_cache plist  = iniparser_create_cache(ini_portable_path, false, true);
    if (!plist)
    {
        return false;
    }
    mouse_time = inicache_read_int("tabs", "mouse_time", &plist);
    if (mouse_time < 0)
    {
        mouse_time = HOVER_DEFAULT;
    }
    else if (!mouse_time)
    {
    #ifdef _LOGDEBUG
        logmsg("mouse_time = 0, mouse_hover will be disabled!\n");
    #endif
        tab_event = false;
    }
    else
    {
        tab_event = true;
    }
    double_click = inicache_read_int("tabs", "double_click_close", &plist) > 0;
    left_click = (tab_event && double_click && inicache_read_int("tabs", "left_click_close", &plist) > 0);
    right_click = inicache_read_int("tabs", "right_click_close", &plist) > 0;
    left_new = inicache_read_int("tabs", "double_click_new", &plist) > 0;
    mouse_close = inicache_read_int("tabs", "mouse_hover_close", &plist) > 0;
    button_new = inicache_read_int("tabs", "mouse_hover_new", &plist) > 0;
    right_double = inicache_read_int("tabs", "right_click_recover", &plist) > 0;
    click_mousewheel = inicache_read_int("tabs", "right_click_mousewheel", &plist) > 0;
    iniparser_destroy_cache(&plist);
    if (!(tab_event || double_click || mouse_close || right_click || left_new || button_new || right_double || click_mousewheel))
    {
        return false;
    }
    return true;
}

static bool
init_uia(void)
{
    HRESULT hr = -1;
    if (on_tabs_reload())
    {
        CoInitialize(NULL);
        hr = CoCreateInstance(&CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, &IID_IUIAutomation, (void **)&g_uia);
    }
    return SUCCEEDED(hr) && (g_uia != NULL);
}

static unsigned WINAPI
threads_on_win10(void *lparam)
{
    if (!get_moz_hwnd() || !init_uia())
    {
    #ifdef _LOGDEBUG
        logmsg("win10 uia error!\n");
    #endif
    }
    return 0;
}

void WINAPI
threads_on_tabs(void)
{
    if (e_browser > MOZ_UNKOWN && e_browser != MOZ_ZEN && is_browser())
    {
        DWORD ver = get_os_version();
        if (ver > 601)
        {
            CloseHandle((HANDLE)_beginthreadex(NULL, 0, &threads_on_win10, NULL, 0, NULL));
        }
        else if (!init_uia())
        {
        #ifdef _LOGDEBUG
            logmsg("win7 uia error!\n");
        #endif
            return;
        }
        if ((mouse_hook = SetWindowsHookExW(WH_MOUSE, mouse_function, dll_module, GetCurrentThreadId())) == NULL)
        {
        #ifdef _LOGDEBUG
            logmsg("SetWindowsHookEx false, error = %lu!\n", GetLastError());
        #endif
        }
        else if ((message_hook = SetWindowsHookExW(WH_GETMESSAGE, message_function, dll_module, GetCurrentThreadId())) == NULL)
        {
        #ifdef _LOGDEBUG
            logmsg("SetWindowsHookEx false, error = %lu!\n", GetLastError());
        #endif
            UnhookWindowsHookEx(mouse_hook);
            mouse_hook = NULL;
        }
    }
}
