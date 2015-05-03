#ifndef _SAFE_EX_H_
#  define _SAFE_EX_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uintptr_t __stdcall getid_parental(uint32_t m_pid);
extern unsigned  __stdcall init_safed(void * pParam);
extern void      __stdcall safe_end(void);

#ifdef __cplusplus
}
#endif 

#endif   /* end _SAFE_EX_H_ */