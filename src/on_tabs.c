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
static int mouse_time;
static bool activation;
static bool double_click;
static bool mouse_close;
volatile long g_once = 0;

void 
send_click(int mouse)
{
    INPUT input[2];
    fzero(&input, sizeof(input));
    input[0].type=input[1].type=INPUT_MOUSE;
    input[0].mi.dwFlags=mouse;
    if (mouse == MOUSEEVENTF_LEFTDOWN)
    {
        input[1].mi.dwFlags=MOUSEEVENTF_LEFTUP;
    }
    else if (mouse == MOUSEEVENTF_MIDDLEDOWN)
    {
        input[1].mi.dwFlags=MOUSEEVENTF_MIDDLEUP;
    }
    else
    {
        return;
    }
    if (mouse == MOUSEEVENTF_LEFTDOWN)
    {
        SendInput(2,input,sizeof(INPUT));
    }
    /* 增加一次点击,避免与双击关闭标签功能冲突 */
    if (double_click)
    {
        SendInput(2,input,sizeof(INPUT));
    }
}

IUIAutomationElement* find_next_child(IUIAutomationElement *pElement)
{
    HRESULT hr;
    IUIAutomationCondition* pCondition;
    IUIAutomationElement* pFound = NULL;
    VARIANT var;
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
    }while (0);
    if (pCondition)
    {
        pCondition->Release();
    }
    return pFound;
}

IUIAutomationElement* find_ui_child(IUIAutomationElement *pElement)
{
    HRESULT hr;
    IUIAutomationElement* tmp = NULL;
    IUIAutomationCondition* pCondition = NULL;
    IUIAutomationElementArray* pFoundArray = NULL;
    IUIAutomationElement* pFound = NULL;
    VARIANT var;
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
            hr = pFoundArray->GetElement(idx, &tmp);
            if (FAILED(hr))
            {
                break;
            }        
            if ((pFound = find_next_child(tmp)) != NULL)
            {
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
    return pFound;
}

IUIAutomationElement* get_tab_bars(HWND hwnd)
{
    HRESULT hr;
    IUIAutomationElement* pFound = NULL;
    IUIAutomationElement* root = NULL;
    hr = g_uia->ElementFromHandle(hwnd, &root);
    if (SUCCEEDED(hr))
    {
        pFound = find_ui_child(root);
    }
    if (root)
    {
        root->Release();
    }
    return pFound;
}

/* 此函数不兼容firefox 59.0或以上版本 
bool mouse_on_close(RECT *pr, POINT *pt)
{
    HRESULT hr;
    IUIAutomationCondition* pCondition = NULL;
    IUIAutomationElementArray* pFoundArray = NULL;
    IUIAutomationElement* tmp = NULL;
    IUIAutomationElement* tab_bar = NULL;
    VARIANT var;
    bool res = false;
    if ((tab_bar = get_tab_bars(WindowFromPoint(*pt))) == NULL)
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
    if (tab_bar)
    {
        tab_bar->Release();
    }
    return res;
}
*/

/* 得到标签页的事件指针, 当标签已激活时把active参数设为1 */
bool mouse_on_tab(RECT *pr, POINT *pt, int *active)
{
    HRESULT hr;
    IUIAutomationCondition* pCondition = NULL;
    IUIAutomationElementArray* pFoundArray = NULL;
    IUIAutomationElement* tmp = NULL;
    IUIAutomationElement* tab_bar = NULL;
    VARIANT var;
    bool res = false;
    if ((tab_bar = get_tab_bars(WindowFromPoint(*pt))) == NULL)
    {
    #ifdef _LOGDEBUG
        logmsg("tab_bar is null from mouse_on_tab, res return false!\n");
    #endif
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
                if (active != NULL)
                {
                    tmp->get_CurrentIsKeyboardFocusable(active);
                }
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
    if (tab_bar)
    {
        tab_bar->Release();
    }
    return res;
}

LRESULT CALLBACK 
mouse_message(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        /* 忽略多余控件产生的鼠标消息 */
        static bool m_ingore = false;
        /* 当鼠标在同一区域移动时,不产生多余的开销 */
        static bool b_track = true;
        MSG* msg = (MSG*)lParam;
        if ((msg->message == WM_MOUSEMOVE) && b_track)
        {
            b_track =  false;
            m_ingore = false;
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
                MouseEvent.dwHoverTime = HOVER_DEFAULT;
            }
            TrackMouseEvent(&MouseEvent);
            Sleep(0);
            return CallNextHookEx(message_hook, nCode, wParam, lParam);
        }
        switch (msg->message)
        {
        case WM_MOUSEHOVER:
            if (!(activation || mouse_close))
            {
                break;
            }
            if (!m_ingore)
            {
                RECT rc, rc_t;
                int active = 1;
                if (mouse_on_tab(&rc, &msg->pt, &active))
                {
                    bool in;
                    rc_t = rc;
                    rc_t.right -= 28;
                    in = PtInRect(&rc_t, msg->pt);
                    if ((activation && !active && in) || (mouse_close && !in))
                    {
                    #ifdef _LOGDEBUG
                        logmsg("mouse on tab, send click message!\n");
                    #endif
                        send_click(MOUSEEVENTF_LEFTDOWN);
                    }
                }
            }
            m_ingore = true;
            b_track = true;
            break;
        case WM_LBUTTONDBLCLK:
            if (!m_ingore && double_click)
            {
            #ifdef _LOGDEBUG
                logmsg("WM_LBUTTONDBLCLK received!\n");
            #endif
                RECT rc;
                if (mouse_on_tab(&rc, &msg->pt, NULL))
                {
                    send_click(MOUSEEVENTF_MIDDLEDOWN);
                }
                m_ingore = true;
            }
            b_track = true;
            break;
        case WM_LBUTTONUP:
            m_ingore = true;
            b_track =  true;
            break;
        case WM_MOUSELEAVE:
            m_ingore = true;
            b_track =  true;
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
        if (!creator_hook((void*)pSetWindowsHookEx, (void*)HookSetWindowsHookEx, (LPVOID*)&sSetWindowsHookExStub))
        {

        #ifdef _LOGDEBUG
            logmsg("creator_hook return false!\n", GetLastError());
        #endif
            return 0;
        }
    }
    return (1);
}
