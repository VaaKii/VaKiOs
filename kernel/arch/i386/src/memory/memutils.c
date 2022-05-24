#include <memory/memory.h>
#include <stdint.h>
#include <stddef.h>

void *memset16(void *ptr, uint16_t value, size_t num) {
    uint16_t *p = ptr;
    while (num--) {
        *p++ = value;
    }
    return ptr;
}

void *memset(void *ptr, int value, size_t num) {
    unsigned char *p = ptr;
    while (num--) {
        *p++ = (unsigned char) value;
    }
    return ptr;
}

void*memcpy(void*dst,const void*src,size_t n){
    size_t i=0;
    for(;i<(n&~7);i+=8)*(uint64_t*)((char*)dst+i)=*(const uint64_t*)((const char*)src+i);
    if(i&4){           *(uint32_t*)((char*)dst+i)=*(const uint32_t*)((const char*)src+i);i+=4;}
    if(i&2){           *(uint16_t*)((char*)dst+i)=*(const uint16_t*)((const char*)src+i);i+=2;}
    if(i&1){           *(uint8_t *)((char*)dst+i)=*(const uint8_t *)((const char*)src+i);}
    return dst;
}