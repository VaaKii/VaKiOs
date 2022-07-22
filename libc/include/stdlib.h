#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void abort(void);
extern void error (int __status, int __errnum, const char *__format, ...)
__attribute__ ((__format__ (__printf__, 3, 4)));

#ifdef __cplusplus
}
#endif

#endif
