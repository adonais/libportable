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
static IUIAutomationCacheRequest *cache_uia;
static uint32_t mouse_time;
static bool tab_event;
static bool double_click;
static bool mouse_close;
static bool left_click;
static bool right_click;
static bool left_new;
static bool button_new;
static bool right_double;

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define ON_BUTTON_FLAGS 999
#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

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

static IUIAutomationElement *
find_next_child(IUIAutomationElement *pElement, long uia_id)
{
    IUIAutomationCondition *pCondition = NULL;
    IUIAutomationElement *pFound = NULL;
    do
    {
        HRESULT hr = -1;
        VARIANT var;
        var.vt = VT_I4;
        var.lVal = uia_id;
        if (!(pElement && g_uia && cache_uia))
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
        hr = IUIAutomationElement_FindFirstBuildCache(pElement, TreeScope_Children, pCondition, cache_uia, &pFound);
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
    return pFound;
}

static bool
get_hv_bars(IUIAutomationElement *ice_root, IUIAutomationElement **tab_bar, const VARIANT *pvar)
{
    bool ret = false;
    IUIAutomationCondition *pCondition = NULL;
    IUIAutomationElementArray *pFoundArray = NULL;
    HRESULT hr = IUIAutomation_CreatePropertyCondition(g_uia, UIA_ControlTypePropertyId, *pvar, &pCondition);
    do
    {
        int c = 0;
        if (FAILED(hr))
        {
        #ifdef _LOGDEBUG
            logmsg("%s_IUIAutomation_CreatePropertyCondition false, cause: %lu\n", __FUNCTION__, GetLastError());
        #endif
            break;
        }
        hr = IUIAutomationElement_FindAllBuildCache(ice_root, TreeScope_Children, pCondition, cache_uia, &pFoundArray);
        if (FAILED(hr))
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
            if (SUCCEEDED(hr))
            {
                if ((*tab_bar = find_next_child(tmp, UIA_TabControlTypeId)) != NULL)
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
    if (!(g_uia && cache_uia && hwnd))
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

/* 得到标签页的事件指针, 当标签没激活时把active参数设为标签序号 */
static bool
mouse_on_tab(RECT *pr, const POINT *pt, int *active)
{
    HRESULT hr;
    VARIANT var;
    IUIAutomationCondition *pCondition = NULL;
    IUIAutomationElementArray *pFoundArray = NULL;
    IUIAutomationElement *tab_bar = NULL;
    IUnknown *m_pattern = NULL;
    bool res = false;
    do
    {
        int idx;
        int c = 0;
        HWND hwnd = WindowFromPoint(*pt);
        var.vt = VT_I4;
        var.lVal = UIA_TabItemControlTypeId;
        hr = get_tab_bars(&tab_bar, hwnd, NULL);
        if (FAILED(hr) || !tab_bar)
        {
        #ifdef _LOGDEBUG
            logmsg("get_tab_bars false, tab_bar = 0x%x, hr = %ld!\n", tab_bar, hr);
        #endif
            break;
        }
        if (!(g_uia && cache_uia))
        {
        #ifdef _LOGDEBUG
            logmsg("point exist null vaule!\n");
        #endif
            break;
        }
        if (button_new && active != NULL)
        {
             /* 获取新建标签按钮 */
            IUIAutomationElement *botton = find_next_child(tab_bar, UIA_ButtonControlTypeId);
            if (botton != NULL)
            {
                RECT rc;
                hr = IUIAutomationElement_get_CurrentBoundingRectangle(botton, &rc);
                if (SUCCEEDED(hr) && PtInRect(&rc, *pt))
                {
                    *active = ON_BUTTON_FLAGS;
                }
                IUIAutomationElement_Release(botton);
                botton = NULL;
            }
        }
        hr = IUIAutomation_CreatePropertyCondition(g_uia, UIA_ControlTypePropertyId, var, &pCondition);
        if (FAILED(hr))
        {
        #ifdef _LOGDEBUG
            logmsg("CreatePropertyCondition false!\n");
        #endif
            break;
        }
        hr = IUIAutomationElement_FindAllBuildCache(tab_bar, TreeScope_Children, pCondition, cache_uia,&pFoundArray);
        if (FAILED(hr))
        {
        #ifdef _LOGDEBUG
            logmsg("IUIAutomationElement_FindAllBuildCache false!\n");
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
        for (idx = 0; idx < c; idx++)
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
                        hr = IUIAutomationElement_GetCurrentPattern(tmp, UIA_SelectionItemPatternId, &m_pattern);
                        if (FAILED(hr))
                        {
                        #ifdef _LOGDEBUG
                            logmsg("%s_IUIAutomationElement_GetCurrentPattern failed!\n", __FUNCTION__);
                        #endif
                            break;
                        }
                        hr = IUIAutomationSelectionItemPattern_get_CurrentIsSelected((IUIAutomationSelectionItemPattern*)m_pattern, &sel);
                        if (FAILED(hr))
                        {
                        #ifdef _LOGDEBUG
                            logmsg("%s_IUIAutomationSelectionItemPattern_get_CurrentIsSelected failed!\n", __FUNCTION__);
                        #endif
                            break;
                        }
                        if (!sel)
                        {
                            *active = idx + 1;
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
    if (tab_bar)
    {
        IUIAutomationElement_Release(tab_bar);
    }
    if (m_pattern)
    {
        IUIAutomationSelectionItemPattern_Release(m_pattern);
    }
    return res;
}

static BOOL
mouse_on_vtab(const POINT *pt)
{
    IUIAutomationElement *tab_bar = NULL;
    bool ret = false;
    do
    {
        bool v = false;
        HWND hwnd = WindowFromPoint(*pt);
        HRESULT hr = get_tab_bars(&tab_bar, hwnd, &v);
        if (FAILED(hr) || !tab_bar)
        {
        #ifdef _LOGDEBUG
            logmsg("get_tab_bars false, tab_bar = 0x%x, hr = %ld!\n", tab_bar, hr);
        #endif
            break;
        }
        if (!(g_uia && cache_uia))
        {
        #ifdef _LOGDEBUG
            logmsg("point exist null vaule!\n");
        #endif
            break;
        }
        if (v)
        {
            RECT rc = {0};
            hr = IUIAutomationElement_get_CurrentBoundingRectangle(tab_bar, &rc);
            if (SUCCEEDED(hr) && PtInRect(&rc, *pt))
            {
                ret = true;
            }
        }
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
                        bool in = false;
                        rc.right -= 26;
                        in = PtInRect(&rc, msg->pt);
                        if (tab_event && in && active > 0)
                        {
                        #ifdef _LOGDEBUG
                            logmsg("mouse on inactive tab[%d]\n", active);
                        #endif
                            send_click(MOUSEEVENTF_LEFTDOWN);
                        }
                        else if (mouse_close && !in)
                        {
                            rc.top += 6;
                            rc.right += 18;
                            rc.bottom -= 6;
                            if (PtInRect(&rc, msg->pt))
                            {
                                send_click(MOUSEEVENTF_LEFTDOWN);
                            }
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
                if (mouse_time)
                {
                    MouseEvent.dwHoverTime = mouse_time;
                }
                else
                {
                    MouseEvent.dwHoverTime = HOVER_DEFAULT;
                }
                TrackMouseEvent(&MouseEvent);
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
                if (mouse_on_vtab(&pmouse->pt))
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
    if (cache_uia)
    {
        IUIAutomationCacheRequest_Release(cache_uia);
        cache_uia = NULL;
    }
    if (g_uia)
    {
        IUIAutomation_Release(g_uia);
        CoUninitialize();
        g_uia = NULL;
    #ifdef _LOGDEBUG
        logmsg("IUIAutomation_Release(g_uia)\n");
    #endif
    }
}

static bool
init_uia(void)
{
    HRESULT hr;
    ini_cache plist  = iniparser_create_cache(ini_portable_path, false, true);
    if (!plist)
    {
        return false;
    }
    mouse_time = inicache_read_int("tabs", "mouse_time", &plist);
    if (mouse_time < 0)
    {
        mouse_time = 300;
    }
    if (!mouse_time)
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
    if (inicache_read_int("tabs", "double_click_close", &plist) > 0)
    {
        double_click = true;
    }
    if (tab_event && double_click && inicache_read_int("tabs", "left_click_close", &plist) > 0)
    {
        left_click = true;
    }
    if (inicache_read_int("tabs", "right_click_close", &plist) > 0)
    {
        right_click = true;
    }
    if (inicache_read_int("tabs", "double_click_new", &plist) > 0)
    {
        left_new = true;
    }
    if (inicache_read_int("tabs", "mouse_hover_close", &plist) > 0)
    {
        mouse_close = true;
    }
    if (inicache_read_int("tabs", "mouse_hover_new", &plist) > 0)
    {
        button_new = true;
    }
    if (inicache_read_int("tabs", "right_click_recover", &plist) > 0)
    {
        right_double = true;
    }
    iniparser_destroy_cache(&plist);
    if (!(tab_event || double_click || mouse_close || right_click || left_new || button_new || right_double))
    {
        return false;
    }
    CoInitialize(NULL);
    hr = CoCreateInstance(&CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, &IID_IUIAutomation, (void **) &g_uia);
    if (SUCCEEDED(hr) && g_uia)
    {
        hr = IUIAutomation_CreateCacheRequest(g_uia, &cache_uia);
    }
    return SUCCEEDED(hr);
}

static unsigned WINAPI
threads_on_win10(void *lparam)
{
    WNDINFO  ff_info = { 0 };
    ff_info.hPid = GetCurrentProcessId();
    if (!get_moz_hwnd(&ff_info))
    {
        return 0;
    }
    if (!init_uia())
    {
    #ifdef _LOGDEBUG
        logmsg("win10 uia error!\n");
    #endif
    }
    return (1);
}

void WINAPI
threads_on_tabs(void)
{
    if (is_specialapp(L"Iceweasel.exe") || is_specialapp(L"firefox.exe"))
    {
        DWORD ver = get_os_version();
        if (ver <= 601 && !init_uia())
        {
        #ifdef _LOGDEBUG
            logmsg("win7 uia error!\n");
        #endif
            return;
        }
        mouse_hook = SetWindowsHookExW(WH_MOUSE, mouse_function, dll_module, GetCurrentThreadId());
        if (mouse_hook == NULL)
        {
        #ifdef _LOGDEBUG
            logmsg("SetWindowsHookEx false, error = %lu!\n", GetLastError());
        #endif
        }
        message_hook = SetWindowsHookExW(WH_GETMESSAGE, message_function, dll_module, GetCurrentThreadId());
        if (message_hook == NULL)
        {
        #ifdef _LOGDEBUG
            logmsg("SetWindowsHookEx false, error = %lu!\n", GetLastError());
        #endif
        }
        if (ver > 601)
        {
            CloseHandle((HANDLE)_beginthreadex(NULL, 0, &threads_on_win10, NULL, 0, NULL));
        }
    }
}
