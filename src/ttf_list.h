#ifndef _List_H
#define _List_H
 
#ifdef TTF_EXTERN
#  undef TTF_EXTERN
#  define TTF_EXTERN
#else
#  define TTF_EXTERN extern
#endif

struct  Node;
typedef struct  Node *PtrToNode;
typedef PtrToNode List;
typedef PtrToNode Position;

/* fonts list */
TTF_EXTERN List  ttf_list;

#ifdef __cplusplus
extern "C" {
#endif
TTF_EXTERN unsigned __stdcall install_fonts(void * pParam);
TTF_EXTERN void __stdcall uninstall_fonts(List *PtrLi);
#ifdef __cplusplus
}
#endif

#endif   /* _List_H */
