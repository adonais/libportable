#define TTF_EXTERN

#include "ttf_list.h"
#include "inipara.h"
#include <shlwapi.h>
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef WCHAR *ElementType;
  
struct Node
{
    WCHAR Element[VALUE_LEN+1];
    Position Next;
};
  
typedef BOOL (* remove_fn)(struct Node const * V);
  
List
init_listing( List Li )
{
    Li = (List)SYS_MALLOC( sizeof( struct Node ) );
    return Li;
}
  
  
int
IsEmpty( List Li )
{
    return Li->Next == NULL;
}
  
int
IsLast( Position P, List Li )
{
    return P->Next == NULL;
}
  
Position
Find( ElementType X, List Li )
{
    Position P;
  
    P = Li->Next;
    while( P != NULL && wcscmp(P->Element, X) )
        P = P->Next;
  
    return P;
}
  
Position
FindPrevious( ElementType X, List Li )
{
    Position P;
  
    P = Li;
    while( P->Next != NULL && wcscmp(P->Next->Element, X) )
        P = P->Next;
  
    return P;
}
  
void
Delete( ElementType X, List Li )
{
    Position P, TmpCell;
  
    P = FindPrevious( X, Li );
  
    if( !IsLast( P, Li ) )  /* Assumption of header use */
    {
        TmpCell = P->Next;
        P->Next = TmpCell->Next;
		if (TmpCell)
		{
			SYS_FREE(TmpCell);
		}
    }
}
  
Position
Ender(List Li)
{
    Position p ;
    p= Li;
    while(p->Next != NULL)
        p = p->Next;
    return p;
}
  
void
Insert( ElementType X, List Li, Position P )
{
    Position TmpCell;
  
    TmpCell = (Position)SYS_MALLOC( sizeof( struct Node ) );
    wcsncpy( TmpCell->Element, X, sizeof(TmpCell->Element)/sizeof(WCHAR) );
    TmpCell->Next = P->Next;
    P->Next = TmpCell;
}
  
void
add_node( ElementType X, List Li )
{
    Position TmpCell;
    Position P = Ender(Li);
    TmpCell = (Position)SYS_MALLOC( sizeof( struct Node ) );
    wcsncpy( TmpCell->Element, X, sizeof(TmpCell->Element)/sizeof(WCHAR) );
    TmpCell->Next = NULL;
    P->Next = TmpCell;
}
  
void
DeleteList( List *head )
{ 
    PtrToNode *curr;
    for (curr = head; *curr; )
    {
        PtrToNode entry = *curr; 
        if ( entry != NULL ) 
        {
            *curr = entry->Next; 
            if (entry)
            {
				SYS_FREE(entry);
            }
        }
        else
        {
            curr = &entry->Next; 
        }
    }
}

void
deletelist_if( List *head ,remove_fn rm )
{ 
    PtrToNode *curr;
    for (curr = head; *curr; )
    {
        PtrToNode entry = *curr; 
        if (rm(entry))
        {
            *curr = entry->Next; 
            if (entry)
            {
				SYS_FREE(entry);
            }
        }
        else
        {
            curr = &entry->Next; 
        }
    }
}
  
void
printList( List Li )
{
    Position P;
    char *saved_locale = NULL;
    P = Li->Next;  /* Header assumed */
    while( P != NULL )
    {
        WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), P->Element, (DWORD)wcslen(P->Element), NULL, NULL);
        wprintf( L"%ls",L" ");
        P = P->Next;
    }
    wprintf(L"\r\n");
}
  
Position
Header( List Li )
{
    return Li;
}
  
Position
First( List Li )
{
    return Li->Next;
}
  
Position
Advance( Position P )
{
    return P->Next;
}
  
ElementType
Retrieve( Position P )
{
    return P->Element;
}

/* 查找字体列表到链表 */
void find_fonts(LPCWSTR parent)
{
    HANDLE h_file = NULL;
    WIN32_FIND_DATAW fd;
    WCHAR filepathname[VALUE_LEN+1] = {0};
    WCHAR sub[VALUE_LEN+1] = {0};
    if( parent[wcslen(parent) -1] != L'\\' )
        _snwprintf(filepathname,VALUE_LEN, L"%ls\\*.*", parent);
    else
        _snwprintf(filepathname,VALUE_LEN, L"%ls*.*", parent);
    h_file = FindFirstFileW(filepathname, &fd);
    if(h_file != INVALID_HANDLE_VALUE)
    {
        do
        {
			if(	wcscmp(fd.cFileName, L".") == 0 ||
				wcscmp(fd.cFileName, L"..")== 0 )
				continue;
            if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                int m = _snwprintf(sub,VALUE_LEN, L"%ls\\%ls",parent, fd.cFileName);
				sub[m] = L'\0';
                find_fonts(sub);
            }
            else if( PathMatchSpecW(fd.cFileName, L"*.ttf") ||
				     PathMatchSpecW(fd.cFileName, L"*.ttc") ||
				     PathMatchSpecW(fd.cFileName, L"*.otf") )
            {
                WCHAR font_path[VALUE_LEN+1] = {0};
                _snwprintf(font_path, VALUE_LEN, L"%s\\%s", parent, fd.cFileName);
				if (ttf_list)
				{
					add_node(font_path, ttf_list);
				}
            }
        } while(FindNextFileW(h_file, &fd) != 0 || GetLastError() != ERROR_NO_MORE_FILES);
        FindClose(h_file); h_file = NULL;
    }
	return;
}

/* 加载字体到进程 */
void load_fonts(List *Li_header)
{
	PtrToNode *curr;
	for (curr = Li_header; *curr; )
	{
		Position ttf_element = *curr;
		DWORD	 numFonts = 0;
		AddFontResourceExW(ttf_element->Element,FR_PRIVATE,&numFonts);
		*curr = ttf_element->Next; 
	}
}

unsigned WINAPI install_fonts(void * pParam)
{
	WCHAR fonts_path[VALUE_LEN+1];
	if ( read_appkey(L"Env",L"DiyFontPath",fonts_path,sizeof(fonts_path)) )
	{
		PathToCombineW(fonts_path,VALUE_LEN);
		if ( PathFileExistsW(fonts_path) )
		{
			/* 初始化字体存储链表 */
			struct	Node fonts_header;
			ttf_list = init_listing( &fonts_header );
			if (ttf_list)
			{
				find_fonts(fonts_path);
				load_fonts(&ttf_list);
			}
		}
	}
	return (1);
}

void WINAPI uninstall_fonts(List *PtrLi)
{
	PtrToNode *curr;
	for (curr = PtrLi; *curr; )
	{
		Position pfonts = *curr; 
		if ( wcslen(pfonts->Element)>0 )
		{
			RemoveFontResourceExW(pfonts->Element,FR_PRIVATE,NULL);
		}
        *curr = pfonts->Next; 
        if (pfonts)
        {
			SYS_FREE(pfonts);
        }
	}
}
