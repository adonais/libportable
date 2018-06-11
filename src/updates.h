#ifndef _UPDATE_EX_H_
#  define _UPDATE_EX_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

extern bool __stdcall init_watch(void);
extern bool __stdcall _getppid(void);

#ifdef __cplusplus
}
#endif 

#endif   /* end _UPDATE_EX_H_ */