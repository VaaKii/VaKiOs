#include <memory/memory.h>
#include <stdint.h>
#include <stddef.h>

void* memset16(void* ptr, uint16_t value, size_t num) {
    uint16_t *p = ptr;
    while (num--) {
        *p++ = value;
    }
    return ptr;
}
void* memset(void* ptr, int value, size_t num) {
    unsigned char *p = ptr;
    while (num--) {
        *p++ = (unsigned char)value;
    }
    return ptr;
}