#ifndef _ONE_INST_H_
#  define _ONE_INST_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef LPWSTR *(*CommandLineToArgvWptr)(LPCWSTR pline, int *numargs);

BOOL __stdcall init_crthook(void);
void __stdcall uninit_crthook(void);
int __stdcall mp_argument_cmp(LPCWSTR s1, LPCWSTR s2);

#ifdef __cplusplus
}
#endif 

#endif   /* end _SAFE_EX_H_ */
