#include "on_tabs.h"
#include "inipara.h"
#include <stdio.h>
#include <process.h>
#include <windows.h>
#include <UIAutomation.h>

typedef HHOOK (WINAPI *SetWindowsHookExPtr)(int idHook,
              HOOKPROC  lpfn,
              HINSTANCE hMod,
              DWORD     dwThreadId);

static SetWindowsHookExPtr pSetWindowsHookEx,sSetWindowsHookExStub;
static HHOOK message_hook;
static IUIAutomation* g_uia;
static IUIAutomationElement* tab_bar;
static int mouse_time;
static bool activation;
static bool double_click;
static bool mouse_close;
volatile long g_wait = 0;
volatile long g_once = 0;

void cli_mouse(int mouse)
{
    INPUT input[1];
    if (GetSystemMetrics(SM_SWAPBUTTON) == 1)
    {
        if (mouse == MOUSEEVENTF_RIGHTDOWN)
        {
            mouse = MOUSEEVENTF_LEFTDOWN;
        }
        else if (mouse == MOUSEEVENTF_RIGHTUP)
        {
            mouse = MOUSEEVENTF_LEFTUP;
        }
    }
    memset(input, 0, sizeof(input));
    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = mouse;
    SendInput(1, input, sizeof(INPUT));
}

unsigned WINAPI
send_click(void *p)
{
    if (!g_wait)
    {
        g_wait = 1;
        int mouse_up = 0;
        int mouse_down = (int)(intptr_t)p;
        if (mouse_down == MOUSEEVENTF_LEFTDOWN)
        {
            mouse_up = MOUSEEVENTF_LEFTUP;
        }
        else if (mouse_down == MOUSEEVENTF_MIDDLEDOWN)
        {
            mouse_up = MOUSEEVENTF_MIDDLEUP;
        }
        if (mouse_up)
        {
            cli_mouse(mouse_down);
            cli_mouse(mouse_up);
        }
        Sleep(1);
        *(long volatile*)&g_wait = 0;
    }
    return (1);
}

bool find_next_child(IUIAutomationElement *pElement)
{
    HRESULT hr;
    IUIAutomationCondition * pCondition;
    IUIAutomationElement * pFound = NULL;
    VARIANT var;
    bool res = false;
    do
    {
        var.vt = VT_I4;
        var.lVal = UIA_TabControlTypeId;
        hr = g_uia->CreatePropertyCondition(UIA_ControlTypePropertyId,var, &pCondition);
        if (FAILED(hr))
        {
            break;
        } 
        hr = pElement->FindFirst(TreeScope_Children,pCondition, &pFound);
        if (FAILED(hr))
        {
            break;
        }
        if (pFound != NULL)
        {
            tab_bar = pFound;
            res = true;
        }
    }while (0);
    if (pCondition)
    {
        pCondition->Release();
    }
    return res;
}

bool mouse_on_close(RECT *pr, POINT *pt)
{
    HRESULT hr;
    IUIAutomationCondition * pCondition = NULL;
    IUIAutomationElementArray * pFoundArray = NULL;
    IUIAutomationElement *tmp = NULL;
    VARIANT var;
    bool res = false;
    if (tab_bar == NULL)
    {
        return res;
    }
    do
    {
        int c = 0;
        var.vt = VT_I4;
        var.lVal = UIA_ButtonControlTypeId;
        hr = g_uia->CreatePropertyCondition(UIA_ControlTypePropertyId,var, &pCondition);
        if (FAILED(hr))
        {
            break;
        } 
        hr = tab_bar->FindAll(TreeScope_Descendants,pCondition, &pFoundArray);
        if (FAILED(hr))
        {
            break;
        }
        hr = pFoundArray->get_Length(&c);
        if (FAILED(hr))
        {
            break;
        }
        for (int idx = 0; idx < c; idx++)
        {
            hr = pFoundArray->GetElement(idx, &tmp);
            if (FAILED(hr))
            {
                break;
            }
            hr = tmp->get_CurrentBoundingRectangle(pr);           
            if (SUCCEEDED(hr))
            {
                CONTROLTYPEID type;
                hr = tmp->get_CurrentControlType(&type);
                if (SUCCEEDED(hr) && (type == UIA_ButtonControlTypeId) && PtInRect(pr, *pt))
                {
                    res = true;
                    break;
                }
            }
        }
    }while (0);
    if (pCondition)
    {
        pCondition->Release();
    }
    if (tmp)
    {
        tmp->Release();
    }
    if (pFoundArray)
    {
        pFoundArray->Release();
    }
    return res;
}

bool mouse_on_tab(RECT *pr, POINT *pt, int *active)
{
    HRESULT hr;
    IUIAutomationCondition * pCondition = NULL;
    IUIAutomationElementArray * pFoundArray = NULL;
    IUIAutomationElement *tmp = NULL;
    VARIANT var;
    bool res = false;
    if (tab_bar == NULL)
    {
        return res;
    }
    do
    {
        int c = 0;
        var.vt = VT_I4;
        var.lVal = UIA_TabItemControlTypeId;
        hr = g_uia->CreatePropertyCondition(UIA_ControlTypePropertyId,var, &pCondition);
        if (FAILED(hr))
        {
        #ifdef _LOGDEBUG
            logmsg("CreatePropertyCondition false!\n");
        #endif
            break;
        } 
        hr = tab_bar->FindAll(TreeScope_Children,pCondition, &pFoundArray);
        if (FAILED(hr))
        {
        #ifdef _LOGDEBUG
            logmsg("FindAll false!\n");
        #endif
            break;
        }
        hr = pFoundArray->get_Length(&c);
        if (FAILED(hr))
        {
            break;
        }
        for (int idx = 0; idx < c; idx++)
        {
            hr = pFoundArray->GetElement(idx, &tmp);
            if (FAILED(hr))
            {
                break;
            }
            hr = tmp->get_CurrentBoundingRectangle(pr); 
            if (SUCCEEDED(hr) && PtInRect(pr, *pt))
            {
                tmp->get_CurrentIsKeyboardFocusable(active);
                res = true;
                break;
            }
        }
    }while (0);
    if (pCondition)
    {
        pCondition->Release();
    }
    if (tmp)
    {
        tmp->Release();
    }
    if (pFoundArray)
    {
        pFoundArray->Release();
    }
    return res;
}

bool find_ui_child(IUIAutomationElement *pElement)
{
    HRESULT hr;
    IUIAutomationElement *tmp = NULL;
    IUIAutomationCondition *pCondition = NULL;
    IUIAutomationElementArray *pFoundArray = NULL;
    VARIANT var;
    bool res = false;
    do
    {
        int c = 0;
        var.vt = VT_I4;
        var.lVal = UIA_ToolBarControlTypeId;
        hr = g_uia->CreatePropertyCondition(UIA_ControlTypePropertyId,var, &pCondition);
        if (FAILED(hr))
        {
            break;
        }
        hr = pElement->FindAll(TreeScope_Children,pCondition, &pFoundArray);
        if (FAILED(hr))
        {
            break;
        }
        hr = pFoundArray->get_Length(&c);
        if (FAILED(hr))
        {
            break;
        }
        for (int idx = 0; idx < c; idx++)
        {
            CONTROLTYPEID type = 0;
            hr = pFoundArray->GetElement(idx, &tmp);
            if (FAILED(hr))
            {
                break;
            }        
            if (find_next_child(tmp))
            {
                res = true;
                break;
            }
        }
    }while (0);
    if (pCondition)
    {
        pCondition->Release();
    }
    if (tmp)
    {
        tmp->Release();
    }
    if (pFoundArray)
    {
        pFoundArray->Release();
    }
    return res;
}

bool get_tab_bars(HWND hwnd)
{
    HRESULT hr;
    bool res = false;
    IUIAutomationElement *root = NULL;
    if (!g_wait)
    {
        g_wait = 1;
        hr = g_uia->ElementFromHandle(hwnd, &root);
        if (SUCCEEDED(hr))
        {
            res = find_ui_child(root);
        }
        Sleep(1);
    }
    if (root)
    {
        root->Release();
    }
    *(long volatile*)&g_wait = 0;
    return res;
}

LRESULT CALLBACK 
mouse_message(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        static int onclick = false;
        MSG* msg = (MSG*)lParam;
        switch (msg->message)
        {
        case WM_MOUSEHOVER: 
            if (tab_bar == NULL)
            {
                get_tab_bars(WindowFromPoint(msg->pt));
            }
            if (!(activation || mouse_close))
            {
                break;
            }
            else if (onclick)
            {
                onclick = false;
            }
            else
            {
                RECT rc;
                int active = 1;
                if (!onclick && mouse_on_tab(&rc, &msg->pt, &active))
                {
                #ifdef _LOGDEBUG
                    logmsg("mouse on tab!\n");
                #endif
                    bool on_close_button = mouse_on_close(&rc, &msg->pt);
                    if (mouse_close && on_close_button)
                    {
                        onclick = true;
                        CloseHandle((HANDLE)_beginthreadex(NULL,0,&send_click,(void *)MOUSEEVENTF_LEFTDOWN,0,NULL));
                    }
                    else if (activation && !active && !on_close_button)
                    {
                        onclick = true;
                        CloseHandle((HANDLE)_beginthreadex(NULL,0,&send_click,(void *)MOUSEEVENTF_LEFTDOWN,0,NULL));
                    }
                }
            }
            break;
        case WM_MOUSEMOVE: 
            if (!g_wait)
            {
                g_wait = 1;
                TRACKMOUSEEVENT MouseEvent;
                MouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
                MouseEvent.dwFlags = TME_HOVER | TME_LEAVE;
                MouseEvent.hwndTrack = WindowFromPoint(msg->pt);
                if (mouse_time)
                {
                    MouseEvent.dwHoverTime = (DWORD)mouse_time;
                }
                else
                {
                    MouseEvent.dwHoverTime = 100;
                }
                TrackMouseEvent(&MouseEvent);
                Sleep(0);
                *(long volatile*)&g_wait = 0;
            }
            break; 
        case WM_RBUTTONDOWN:
            InterlockedExchange(&g_wait,1);
            break;
        case WM_RBUTTONUP:
            RECT rc;
            int tmp;
            if (g_wait && !mouse_on_tab(&rc, &msg->pt, &tmp))
            {
                *(long volatile*)&g_wait = 0;
            }
            break;
        case WM_LBUTTONDBLCLK:
            if (double_click)
            {
                RECT rc;
                int active = 0;
                if (mouse_on_tab(&rc, &msg->pt, &active) && active)
                {
                    CloseHandle((HANDLE)_beginthreadex(NULL,0,&send_click,(void *)MOUSEEVENTF_MIDDLEDOWN,0,NULL));
                    onclick = true;
                }
            }
            break;
        case WM_LBUTTONUP:
            *(long volatile*)&g_wait = 0;
            break;
        default :
            break;
        }
    }
    return CallNextHookEx(message_hook, nCode, wParam, lParam);
}

bool init_uia(void)
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
        logmsg("mouse_time = 0, OnTabs will be disabled!\n");
    #endif
        activation = false;
    }
    else
    {
        activation = true;
    }
    if (read_appint(L"tabs", L"double_click_close") > 0)
    {
        double_click = true;
    }
    if (read_appint(L"tabs", L"mouse_hover_close") > 0)
    {
        mouse_close = true;
    }
    if (!(activation || double_click || mouse_close))
    {
        return false;
    }
    CoInitialize(NULL);
    hr = CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER, 
         __uuidof(IUIAutomation), (void**)&g_uia);
    return (SUCCEEDED(hr));
}

void WINAPI
un_uia(void)
{
    if (tab_bar)
    {
        tab_bar->Release();
    }
    if (g_uia)
    {
        g_uia->Release();
        CoUninitialize();
    }
    if (message_hook)
    {
        UnhookWindowsHookEx(message_hook);
    }
}

static HHOOK WINAPI 
HookSetWindowsHookEx(int idHook, HOOKPROC lpfn, HINSTANCE hMod, DWORD dwThreadId)
{
    if (idHook == WH_GETMESSAGE && !g_once)
    {
        *(long volatile*)&g_once = 1;
        sSetWindowsHookExStub(idHook, mouse_message, dll_module, dwThreadId);
    }
    return sSetWindowsHookExStub(idHook, lpfn, hMod, dwThreadId);
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
        return;
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
        if ((pSetWindowsHookEx = (SetWindowsHookExPtr)GetProcAddress(m_user32, "SetWindowsHookExW")) == NULL )
        {
            return 0;
        }
        if (!creator_hook(pSetWindowsHookEx, HookSetWindowsHookEx, (LPVOID*)&sSetWindowsHookExStub))
        {

        #ifdef _LOGDEBUG
            logmsg("creator_hook return false!\n", GetLastError());
        #endif
            return 0;
        }
    }
    return (1);
}
