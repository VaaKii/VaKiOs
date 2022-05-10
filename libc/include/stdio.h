#ifndef _STDIO_H
#define _STDIO_H 1

#include <sys/cdefs.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

int printf(const char* __restrict, ...);
int putchar(int);
void __itoa(unsigned i,unsigned base,char* buf);
void __itoa_s(int i,unsigned base,char* buf);
int puts(const char*);

#ifdef __cplusplus
}
#endif

#endif
