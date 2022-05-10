#include <stdio.h>

#include <vga/tty.h>
#include <gdt/gdt.h>

void kernel_main(void) {
	terminal_initialize();
    printf("Hello, world! %x \n",0x12ff);
	//gdt_init();
}
