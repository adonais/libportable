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

typedef struct _thread_data
{
    int code;
    DWORD id;
}thread_data;

static SetWindowsHookExPtr pSetWindowsHookEx,sSetWindowsHookExStub;
static HHOOK message_hook;
static IUIAutomation* g_uia;
static IUIAutomationElement* tab_bar;
static int mouse_time;
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
        cli_mouse(MOUSEEVENTF_LEFTDOWN);
        cli_mouse(MOUSEEVENTF_LEFTUP);
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

bool mouse_on_tab(RECT *pr, POINT *pt)
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
        MSG* msg = (MSG*)lParam;
        switch (msg->message)
        {
        case WM_MOUSEHOVER: 
            if (tab_bar == NULL)
            {
                get_tab_bars(WindowFromPoint(msg->pt));
            }
            else
            {
                RECT rc;
                if (mouse_on_tab(&rc, &msg->pt) && !mouse_on_close(&rc, &msg->pt))
                {
                #ifdef _LOGDEBUG
                    logmsg("mouse on tab!\n");
                #endif
                    CloseHandle((HANDLE)_beginthreadex(NULL,0,&send_click,NULL,0,NULL));
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
                MouseEvent.dwHoverTime = (DWORD)mouse_time;
                TrackMouseEvent(&MouseEvent);
                Sleep(1);
                *(long volatile*)&g_wait = 0;
            }
            break; 
        case WM_RBUTTONDOWN:
            InterlockedExchange(&g_wait,1);
            break;
        case WM_RBUTTONUP:
            RECT rc;
            if (g_wait && !mouse_on_tab(&rc, &msg->pt))
            {
                *(long volatile*)&g_wait = 0;
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

