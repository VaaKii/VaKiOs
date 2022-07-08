#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <vga/tty.h>
#include <gdt/gdt.h>
#include <idt/idt.h>
#include <memory/hal.h>
#include <memory/pic.h>
#include <memory/memory.h>
#include <drivers/keyboard.h>
#include <threading/tasking.h>
#include <threading/pit.h>

#include <string.h>

extern uint32_t kernel_end;
extern uint32_t kernel_base;

void _kernel_loop();


void kernel_main(void) {
	terminal_initialize();
    mm_init(&kernel_end);
	gdt_init();
    idt_init();
    exception_init();
    hal_init();
    pic_init();
    pit_init();
    printf("before sti");
    asm volatile("sti");
    printf("Kernel initialized!\n");
    tasking_init();
    panic("Reached end of main(), but tasking was not started.");
    for(;;);
}


int cu_pid = 0;
void late_init()
{
    int pid = 0;
    pid = START("kbd_init", keyboard_init);
    while(is_pid_running(pid))schedule_noirq();
    while(1) {
        START_AND_WAIT("_kernel_loop", _kernel_loop);
        printf("dasd");
        printf("\n\nThe terminal has crashed. Restarting it...\n\n");
    }
    panic("Reached end of late_init()\n");
}

static char c = 0;
static char* buffer = 0;
static uint16_t loc = 0;

void _kernel_loop(){
    buffer = (char*)malloc(256);
    char* prompt = "(VaKiOS) $ ";
    uint8_t prompt_size = strlen(prompt);
    printf("Start up keyboard...\n");
    prompt:
    printf("%s", prompt);
    memset(buffer,0,256);
    while(1) {
        if (!keyboard_enabled()) {
            schedule_noirq();
            continue;
        }
        c = keyboard_get_key();
        if (!c) continue;
        if (c == '\r') {
            putchar(' ');
            buffer[loc--] = 0;
            continue;
        }
        if (c == '\n') {
            putchar(c);
            buffer[loc] = 0;
            loc = 0;
            uint32_t n = strsplit(buffer, ' ');
            if (strcmp(buffer, "help") == 0) {
                printf("LevOS4.0\nThis is the kernel terminal.\nDon't do anything stupid.\n");
                printf("Commands available: help; reboot; read; malloc; ps; fl; cd; ls\n"
                       "clear; reset; time; v; kill\n");
                goto prompt;
            }
        }
        buffer[loc++] = c;
        buffer[loc] = 0;
        putchar(c);
    }
}



