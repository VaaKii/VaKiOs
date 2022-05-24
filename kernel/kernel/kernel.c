#include <stdio.h>

#include <vga/tty.h>
#include <gdt/gdt.h>
#include <idt/idt.h>
#include <memory/hal.h>
#include <memory/pic.h>
#include <memory/memory.h>

extern uint32_t kernel_end;
extern uint32_t kernel_base;

void kernel_main(void) {
	terminal_initialize();
    mm_init(&kernel_end);
	gdt_init();
    idt_init();
    hal_init();
    pic_init();
    asm volatile("sti");
    printf("Kernel loaded.\n");
}
