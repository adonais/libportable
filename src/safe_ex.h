#ifndef _SAFE_EX_H_
#  define _SAFE_EX_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned  __stdcall init_safed(void * pParam);
extern bool __stdcall is_child_of(const uint32_t parent);

#ifdef __cplusplus
}
#endif 

#endif   /* end _SAFE_EX_H_ */