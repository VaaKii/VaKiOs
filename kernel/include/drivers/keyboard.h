
#ifndef __KEYBOAD_H
#define __KEYBOAD_H
#include <stdint.h>
#include <memory/hal.h>


extern void keyboard_init();
extern uint8_t keyboard_enabled();
extern char keyboard_get_key();
extern uint8_t keyboard_to_ascii(uint8_t key);

#endif //__KEYBOAD_H
