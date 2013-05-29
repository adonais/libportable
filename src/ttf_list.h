#ifndef _List_H
#define _List_H
  
#include "inipara.h"
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
  
typedef WCHAR *ElementType;
struct  Node;
typedef struct  Node *PtrToNode;
typedef PtrToNode List;
typedef PtrToNode Position;
  
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
    memset( TmpCell,0,sizeof( struct Node ) );
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
  
#endif   /* end _List_H */
