#ifndef COBJMACROS
#define COBJMACROS
#endif

#include "inipara.h"
#include <stdio.h>
#include <process.h>
#include <windows.h>
#include "win_automation.h"

typedef HHOOK(WINAPI *SetWindowsHookExPtr)(int idHook, HOOKPROC lpfn, HINSTANCE hMod, DWORD dwThreadId);

static SetWindowsHookExPtr pSetWindowsHookEx, sSetWindowsHookExStub;
static HHOOK message_hook;
static WNDPROC oldWindowProc;
static IUIAutomation *g_uia;
static IUIAutomationCacheRequest *cache_uia;
static IUIAutomationElement *ice_root;
static uint32_t mouse_time;
static bool tab_event;
static bool double_click;
static bool mouse_close;
static bool left_click;
static bool right_click;
volatile long g_once = 0;
volatile long grb_locked = 0;

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

static void
send_key_click(int mouse)
{
    if (mouse == MOUSEEVENTF_MIDDLEDOWN)
    {
    #if defined(__GNUC__) || defined(__clang__)
        INPUT input[] =
        {
            { INPUT_KEYBOARD, {.ki = { VK_RCONTROL, 0, 0, 0, 0 } } },
            { INPUT_KEYBOARD, {.ki = { VK_F4, 0, 0, 0, 0 } } },
            { INPUT_KEYBOARD, {.ki = { VK_F4, 0, KEYEVENTF_KEYUP, 0, 0 } } },
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
        input[1].ki.wVk = input[2].ki.wVk = VK_F4;
        input[2].ki.dwFlags = input[3].ki.dwFlags = KEYEVENTF_KEYUP;
    #endif    
        SendInput(sizeof(input) / sizeof(INPUT), input, sizeof(INPUT));
    }
}

static void
send_click(int mouse)
{
    static uint32_t pre_rec;
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
            static uint32_t pre_left;
            if (m_rec - pre_left > (uint32_t) GetDoubleClickTime())
            {
                pre_left = m_rec;
                send_key_click(MOUSEEVENTF_MIDDLEDOWN);
            }
        }
    }
}

static IUIAutomationElement *
find_next_child(IUIAutomationElement *pElement)
{
    IUIAutomationCondition *pCondition = NULL;
    IUIAutomationElement *pFound = NULL;
    do
    {
        VARIANT var;
        var.vt = VT_I4;
        var.lVal = UIA_TabControlTypeId;
        if (!pElement)
        {
        #ifdef _LOGDEBUG
            logmsg("%s false, cause: pElement = 0x%\n", __FUNCTION__, pElement);
        #endif             
            break;
        }
        HRESULT hr = IUIAutomation_CreatePropertyCondition(g_uia, UIA_ControlTypePropertyId, var, &pCondition);
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

static HRESULT
get_tab_bars(IUIAutomationElement **tab_bar)
{
    HRESULT hr = 1;
    IUIAutomationCondition *pCondition = NULL;
    IUIAutomationElementArray *pFoundArray = NULL;
    VARIANT var;
    do
    {
        int idx;
        int c = 0;
        var.vt = VT_I4;
        var.lVal = UIA_ToolBarControlTypeId;
        if (!ice_root)
        {
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
        hr = IUIAutomationElement_FindAllBuildCache(ice_root, TreeScope_Children, pCondition, cache_uia, &pFoundArray);
        if (FAILED(hr))
        {
        #ifdef _LOGDEBUG
            logmsg("%s_IUIAutomationElement_FindAll false, cause: %lu\n", __FUNCTION__, GetLastError());
        #endif             
            break;
        }
        hr = IUIAutomationElementArray_get_Length(pFoundArray, &c);
        if (FAILED(hr) || c == 0 || c > 64)
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
            if (SUCCEEDED(hr) && (*tab_bar = find_next_child(tmp)) != NULL)
            {                
                hr = 0;
                break;
            }
        }
    } while (0);
    if (pCondition)
    {
        IUIAutomationCondition_Release(pCondition);
    }
    if (pFoundArray)
    {
        IUIAutomationElement_Release(pFoundArray);
    }
    return hr;
}

/* 得到标签页的事件指针, 当标签没激活时把active参数设为标签序号 */
static bool
mouse_on_tab(RECT *pr, POINT *pt, int *active)
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
        var.vt = VT_I4;
        var.lVal = UIA_TabItemControlTypeId;
        hr = get_tab_bars(&tab_bar);
        if (FAILED(hr))
        {
        #ifdef _LOGDEBUG
            logmsg("get_tab_bars false, tab_bar = 0x%x!\n", tab_bar);
        #endif
            break;
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
            logmsg("FindAll false!\n");
        #endif
            break;
        }
        hr = IUIAutomationElementArray_get_Length(pFoundArray, &c);
        if (FAILED(hr) || c == 0 || c > 64)
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
                hr = IUIAutomationElement_get_CurrentBoundingRectangle(tmp, (tagRECT *)&rc); 
                if (SUCCEEDED(hr) && PtInRect(&rc, *pt))
                {                 
                    res = true;     
                    if (pr != NULL)
                    {
                        *pr = rc;
                    }   
                    if (active != NULL)
                    {
                        unsigned long m_tmp = 0;
                        hr = IUIAutomationElement_GetCurrentPattern(tmp, UIA_LegacyIAccessiblePatternId, &m_pattern);
                        if (FAILED(hr))
                        {
                        #ifdef _LOGDEBUG
                            logmsg("%s_IUIAutomationElement_GetCurrentPattern false!\n", __FUNCTION__);
                        #endif  
                            break;                            
                        }
                        hr = IUIAutomationLegacyIAccessiblePattern_get_CurrentState((IUIAutomationLegacyIAccessiblePattern *)m_pattern, &m_tmp);
                        if (FAILED(hr))
                        {
                        #ifdef _LOGDEBUG
                            logmsg("%s_IUIAutomationLegacyIAccessiblePattern_get_CurrentState false!\n", __FUNCTION__);
                        #endif  
                            break;
                        }                        
                        if (m_tmp == 0x200000)
                        {
                            *active = idx + 1;
                        }
                    }
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
        IUIAutomationLegacyIAccessiblePattern_Release(m_pattern); 
    }    
    return res;
}

static HRESULT
find_root_ui(HWND hwnd, IUIAutomationElement **proot)
{
    HRESULT hr;
    do 
    {
        hr = IUIAutomation_CreateCacheRequest(g_uia, &cache_uia);        
        if (FAILED(hr))
        {
        #ifdef _LOGDEBUG
            logmsg("%s_IUIAutomation_CreateCacheRequest false!\n", __FUNCTION__);
        #endif
            break;
        }
        hr = IUIAutomation_ElementFromHandleBuildCache(g_uia, hwnd, cache_uia, proot);
        if (FAILED(hr))
        {
        #ifdef _LOGDEBUG
            logmsg("%s_IUIAutomation_ElementFromHandleBuildCache NULL!\n", __FUNCTION__);
        #endif
        }                       
    }while (0);
    return hr;
}

LRESULT CALLBACK
newWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_LBUTTONDBLCLK:
        {
            POINT pos;
            if (!double_click)
            {
                break;
            }
            if (!GetCursorPos(&pos))
            {
            #ifdef _LOGDEBUG
                logmsg("GetCursorPos(&pos) false, cause:%lu!\n", GetLastError());
            #endif                 
                break;
            }            
            if (!mouse_on_tab(NULL, &pos, NULL))
            {
            #ifdef _LOGDEBUG
                logmsg("WM_LBUTTONDBLCLK[mouse_on_tab return null]\n");
            #endif
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
        }
            return 1;         
        case WM_RBUTTONUP:
        {
            POINT pos;
            if (!right_click || KEY_DOWN(VK_SHIFT))
            {
                break;
            }
            if (!GetCursorPos(&pos))
            {
            #ifdef _LOGDEBUG
                logmsg("GetCursorPos(&pos) false from %s!\n", __FUNCTION__);
            #endif                 
                break;
            }
            if (!mouse_on_tab(NULL, &pos, NULL))
            {
            #ifdef _LOGDEBUG
                logmsg("%s_mouse_on_tab false!\n", __FUNCTION__);
            #endif                
                break;
            }
            send_key_click(MOUSEEVENTF_MIDDLEDOWN);
        }
            return 1;     
        default:          
            break;
    }
    return CallWindowProc(oldWindowProc, handle, message, wParam, lParam);
}

static LRESULT CALLBACK
mouse_message(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        HWND  hwnd = NULL;
        MSG   *msg = (MSG *) lParam;
        switch (msg->message)
        {
            case WM_MOUSEMOVE:
            {
                TRACKMOUSEEVENT MouseEvent;
                MouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
                MouseEvent.dwFlags = TME_HOVER | TME_LEAVE;
                MouseEvent.hwndTrack = WindowFromPoint(msg->pt);
                if (mouse_time)
                {
                    MouseEvent.dwHoverTime = mouse_time;
                }
                else
                {
                    MouseEvent.dwHoverTime = HOVER_DEFAULT;
                }
                TrackMouseEvent(&MouseEvent);
                Sleep(0); 
                if (!grb_locked)
                {
                    DWORD m_pid = 0;
                    GetWindowThreadProcessId(MouseEvent.hwndTrack, &m_pid);
                    if (m_pid == GetCurrentProcessId())
                    {
                        HRESULT hr;
                        *(long volatile *) &grb_locked = 1;
                        hwnd = MouseEvent.hwndTrack;
                        hr = find_root_ui(hwnd, &ice_root);
                        if (SUCCEEDED(hr))
                        {
                        #ifdef _LOGDEBUG
                            logmsg("UI ice_root = 0x%x\n", ice_root);
                        #endif                            
                        }
                    }
                    if (hwnd && (double_click || right_click))
                    {
                        oldWindowProc = (WNDPROC) SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR) newWindowProc);
                        if (oldWindowProc)
                        {
                        #ifdef _LOGDEBUG
                            logmsg("mouse into browser window. hwnd = 0x%x\n", hwnd);
                        #endif
                        }
                    }
                }
            }  
                break;
            case WM_MOUSEHOVER:
                if (tab_event || mouse_close)
                {
                    RECT rc;
                    bool in;
                    int active = 0;
                    if (!mouse_on_tab(&rc, &msg->pt, &active))
                    {
                    #ifdef _LOGDEBUG
                        logmsg("WM_MOUSEHOVER[mouse not on tab!]\n");
                    #endif                        
                        break;
                    }
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
                        send_click(MOUSEEVENTF_LEFTDOWN);
                    }
                }
                break;          
            default:
                break;
        }
    }
    return CallNextHookEx(message_hook, nCode, wParam, lParam);
}

static HHOOK WINAPI
HookSetWindowsHookEx(int idHook, HOOKPROC lpfn, HINSTANCE hMod, DWORD dwThreadId)
{
    if (idHook == WH_GETMESSAGE && !g_once)
    {
        *(long volatile *) &g_once = 1;
        message_hook = sSetWindowsHookExStub(idHook, mouse_message, dll_module, dwThreadId);
        if (message_hook != NULL)
        {
            return message_hook;
        }
    }
    return sSetWindowsHookExStub(idHook, lpfn, hMod, dwThreadId);
}

static bool
init_uia(void)
{
    HRESULT hr;
    if (message_hook != NULL)
    {
    #ifdef _LOGDEBUG
        logmsg("message_hook used\n");
    #endif
        return false;
    }
    mouse_time = read_appint(L"tabs", L"mouse_time");
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
    if (read_appint(L"tabs", L"double_click_close") > 0)
    {
        double_click = true;
    }
    if (read_appint(L"tabs", L"mouse_hover_close") > 0)
    {
        mouse_close = true;
    }    
    if (tab_event && double_click && read_appint(L"tabs", L"left_click_close") > 0)
    {
        left_click = true;
    }
    if (read_appint(L"tabs", L"right_click_close") > 0)
    {
        right_click = true;
    }
    if (!(tab_event || double_click || mouse_close || right_click))
    {
        return false;
    }
    CoInitialize(NULL);
    hr = CoCreateInstance(&CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, &IID_IUIAutomation, (void **) &g_uia);
#ifdef _LOGDEBUG
    if (FAILED(hr))
    {
        print_process_module(GetCurrentProcessId());
    }
#endif
    return SUCCEEDED(hr);
}

void WINAPI
un_uia(void)
{
    if (ice_root)
    {
        IUIAutomationElement_Release(ice_root);
        ice_root = NULL;
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
    }
    if (message_hook)
    {
        UnhookWindowsHookEx(message_hook);
    }
}

void WINAPI
threads_on_win7(void)
{
    if (!init_uia())
    {
        return;
    }
    message_hook = SetWindowsHookExW(WH_GETMESSAGE, mouse_message, dll_module, GetCurrentThreadId());
    if (message_hook == NULL)
    {
    #ifdef _LOGDEBUG
        logmsg("SetWindowsHookEx false, error = %lu!\n", GetLastError());
    #endif
    }
}

unsigned WINAPI
threads_on_win10(void *lparam)
{
    if (init_uia())
    {
        HMODULE m_user32 = GetModuleHandleW(L"user32.dll");
        if (!m_user32)
        {
            return 0;
        }
        if ((pSetWindowsHookEx = (SetWindowsHookExPtr) GetProcAddress(m_user32, "SetWindowsHookExW")) == NULL)
        {
            return 0;
        }
        if (!creator_hook((void *) pSetWindowsHookEx, (void *) HookSetWindowsHookEx, (LPVOID *) &sSetWindowsHookExStub))
        {
        #ifdef _LOGDEBUG
            logmsg("creator_hook return false in %s!\n", __FUNCTION__);
        #endif
            return 0;
        }
    }
    return (1);
}
