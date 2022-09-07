#include <drivers/keyboard.h>
#include <gdt/gdt.h>
#include <idt/idt.h>
#include <memory/hal.h>
#include <memory/memory.h>
#include <memory/pic.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <threading/pit.h>
#include <threading/tasking.h>
#include <vga/tty.h>

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
  asm volatile("sti");
  printf("Kernel initialized!\n");
  paging_init();
  tasking_init();
  panic("Reached end of main(), but tasking was not started.");
}

int cu_pid = 0;
void late_init() {
    //printf("[Late init] Starting...\n");
  int pid = 0;
  pid = START("kgb_init", keyboard_init);
//  tasking_print_all();
  while (is_pid_running(pid))
    schedule_noirq();
  while (1) {
    START_AND_WAIT("_kernel_loop", _kernel_loop);
    printf("The terminal has crashed. Restarting it...");
  }
  panic("Reached end of late_init()\n");
}

static char c = 0;
static char *buffer = 0;
static uint16_t loc = 0;

void _kernel_loop() {
    buffer = (char *) malloc(256);
    char *prompt = "(VlaDOS(PizDOS)) $ ";
    uint8_t prompt_size = strlen(prompt);
    printf("Start up keyboard...\n");
    prompt:
    printf("%s", prompt);
    memset(buffer, 0, 256);
    while (1) {
        if (!keyboard_enabled()) {
            schedule_noirq();
            panic("Keyboard not enabled!");
            continue;
        }
    c = keyboard_get_key();
    if (!c){continue;}

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
        printf("LevOS4.0\nThis is the kernel terminal.\nDon't do anything "
               "stupid.\n");
        printf(
            "Commands available: help; reboot; read; malloc; ps; fl; cd; ls\n"
            "clear; reset; time; v ; kill\n");
        goto prompt;
      }
    }
    buffer[loc++] = c;
    buffer[loc] = 0;
    putchar(c);
  }
}
