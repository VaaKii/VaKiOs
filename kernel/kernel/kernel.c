#include <stdio.h>

#include <vga/tty.h>
#include <gdt/gdt.h>
#include <memory/memory.h>

extern uint32_t kernel_end;
extern uint32_t kernel_base;

void kernel_main(void) {
	terminal_initialize();
    printf("Hello, world! %x \n",0x12ff);
    mm_init(&kernel_end);
    printf("MM initialized\n");
	gdt_init();
    printf("GDT initialized\n");
}
