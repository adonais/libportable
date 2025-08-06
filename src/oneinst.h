#ifndef _ONE_INST_H_
#  define _ONE_INST_H_

#ifdef __cplusplus
extern "C" {
#endif

BOOL __stdcall init_crthook(void);
void __stdcall uninit_crthook(void);

#ifdef __cplusplus
}
#endif 

#endif   /* end _SAFE_EX_H_ */
