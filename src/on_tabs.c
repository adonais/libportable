#ifndef COBJMACROS
#define COBJMACROS
#endif

#include "inipara.h"
#include <stdio.h>
#include <process.h>
#include <windows.h>
#include "win_automation.h"

#define WAIT_TIMES 9000
typedef HHOOK(WINAPI *SetWindowsHookExPtr)(int idHook, HOOKPROC lpfn, HINSTANCE hMod, DWORD dwThreadId);

static SetWindowsHookExPtr pSetWindowsHookEx, sSetWindowsHookExStub;
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
volatile long g_once = 0;

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
get_tab_bars(IUIAutomationElement **tab_bar, HWND hwnd)
{
    HRESULT hr = 1;
    IUIAutomationCondition *pCondition = NULL;
    IUIAutomationElementArray *pFoundArray = NULL;
	IUIAutomationElement *ice_root = NULL;
    VARIANT var;
    do
    {
        int idx;
        int c = 0;
        var.vt = VT_I4;
        var.lVal = UIA_ToolBarControlTypeId;
        if (!(g_uia && cache_uia && hwnd))
        {
            break;
        }
		hr = IUIAutomation_ElementFromHandle(g_uia, hwnd, &ice_root);
        if (FAILED(hr) || !ice_root)
        {
        #ifdef _LOGDEBUG
            logmsg("%s_IUIAutomation_ElementFromHandleBuildCache false, cause: %lu\n", __FUNCTION__, GetLastError());
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
        hr = IUIAutomationElement_FindAllBuildCache(ice_root, TreeScope_Children, pCondition, cache_uia, &pFoundArray);
        if (FAILED(hr))
        {
        #ifdef _LOGDEBUG
            logmsg("%s_IUIAutomationElement_FindAll false, cause: %lu\n", __FUNCTION__, GetLastError());
        #endif             
            break;
        }
        hr = IUIAutomationElementArray_get_Length(pFoundArray, &c);
        if (FAILED(hr) || c == 0 || c > VALUE_LEN)
        {
        #ifdef _LOGDEBUG
            logmsg("%s_IUIAutomationElementArray_get_Length false, c = %d!\n", __FUNCTION__, c);
        #endif
            hr = 1;
            break;
        }          
        for (idx = 0; idx < c; idx++)
        {
            IUIAutomationElement *tmp = NULL;
            hr = IUIAutomationElementArray_GetElement(pFoundArray, idx, &tmp);
            if (SUCCEEDED(hr) && (*tab_bar = find_next_child(tmp, UIA_TabControlTypeId)) != NULL)
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
	if (ice_root)
	{
		IUIAutomationElement_Release(ice_root);
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
        HWND hwnd = WindowFromPoint(*pt);
        var.vt = VT_I4;
        var.lVal = UIA_TabItemControlTypeId;
        hr = get_tab_bars(&tab_bar, hwnd);
        if (FAILED(hr))
        {
        #ifdef _LOGDEBUG
            logmsg("get_tab_bars false, tab_bar = 0x%x!\n", tab_bar);
        #endif
            break;
        }
        if (!(g_uia && cache_uia && tab_bar))
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
                hr = IUIAutomationElement_get_CurrentBoundingRectangle(botton, (tagRECT *)&rc); 
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

static LRESULT CALLBACK
message_function(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        MSG   *msg = (MSG *) lParam;
        switch (msg->message)
        {    
            case WM_MOUSEHOVER:
                if (tab_event || mouse_close || button_new)
                {
                    RECT rc;
                    int active = 0;
                    if (mouse_on_tab(&rc, &msg->pt, &active))
                    {
                        bool in;
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
                        send_key_click(WM_NCLBUTTONDBLCLK);
                    }
                }
                break;           
            default:
                break;
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
            }  
                break;                
            case WM_NCLBUTTONDBLCLK:
            {
                if (!left_new || KEY_DOWN(VK_SHIFT))
                {
                    break;
                }
                send_key_click(WM_NCLBUTTONDBLCLK);
            }
                 return 1;  
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
            }
                return 1;   
            case WM_NCRBUTTONDOWN: 
            {
                if (!right_double || KEY_DOWN(VK_SHIFT))
                {
                    break;
                }
                send_key_click(WM_NCRBUTTONDOWN);
            }
                return 1;                           
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
            }
                return 1;             
            default:
                break;
        }
    }
    return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
}

static HHOOK WINAPI
HookSetWindowsHookEx(int idHook, HOOKPROC lpfn, HINSTANCE hMod, DWORD dwThreadId)
{
    if (idHook == WH_GETMESSAGE && !g_once)
    {
        *(long volatile *) &g_once = 1;
        mouse_hook = SetWindowsHookExW(WH_MOUSE, mouse_function, dll_module, dwThreadId);
        message_hook = SetWindowsHookExW(WH_GETMESSAGE, message_function, dll_module, dwThreadId);
        if (mouse_hook && message_hook && remove_hook((void **)&pSetWindowsHookEx))
        {
        #ifdef _LOGDEBUG
            logmsg("remove_hook(pSetWindowsHookEx)\n");
        #endif 
            return NULL; 
        }
    }
    return sSetWindowsHookExStub(idHook, lpfn, hMod, dwThreadId);
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
    if (message_hook || mouse_hook)
    {
    #ifdef _LOGDEBUG
        logmsg("message hook used\n");
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
    if (tab_event && double_click && read_appint(L"tabs", L"left_click_close") > 0)
    {
        left_click = true;
    }
    if (read_appint(L"tabs", L"right_click_close") > 0)
    {
        right_click = true;
    }
    if (read_appint(L"tabs", L"double_click_new") > 0)
    {
        left_new = true;
    } 
    if (read_appint(L"tabs", L"mouse_hover_close") > 0)
    {
        mouse_close = true;
    }       
    if (read_appint(L"tabs", L"mouse_hover_new") > 0)
    {
        button_new = true;
    } 
    if (read_appint(L"tabs", L"right_click_recover") > 0)
    {
        right_double = true;
    }            
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

void WINAPI
threads_on_win7(void)
{
    if (!init_uia())
    {
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
        Sleep(WAIT_TIMES);
    }
#ifdef _LOGDEBUG
    logmsg("threads_on_win10 exit!\n");
#endif    
    return (1);
}
