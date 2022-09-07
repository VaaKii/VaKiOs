#include <gdt/gdt.h>
#include <kernel/kernel.h>
#include <kernel/signal.h>
#include <memory/memory.h>
#include <threading/pit.h>
#include <threading/tasking.h>

#include <stdint.h>
#include <stdio.h>

PROCESS *current_process = 0;
uint32_t lpid = 0;
uint8_t __enabled = 0;

void task1() {
  printf("[1]Tasking online.\n");
  _kill();
  //	while(1) schedule_noirq();kprintf("[1] Hello, I am task one!\n");
}

void task2() {
    printf("[2] Hello, I am task two! :)\n");
}

void task3() {
    printf("[3] Mutex locking memcpy and kprintf! Awesome!\n");
}

void idle_thread() {
    printf("[IDLE] I am the idle thread.\n");
  enable_task();
  __enabled = 1;
  late_init();
}

void kill(uint32_t pid) {
    if (pid == 1)
        panic("Idle can't be killed!\n");
    if (pid == current_process->pid)
        _kill();
    PROCESS *orig = current_process;
    PROCESS *process = orig;
    while (1) {
        if (process->pid == pid) {
            printf("Process %s (%d) was set to ZOMBIE.\n", process->name, pid);
            process->state = PROCESS_STATE_ZOMBIE;
            break;
        }
        process = process->next;
        if (process == orig)
            break;
    }
}

void send_sig(int sig) { current_process->notify(sig); }

int is_tasking() { return __enabled; }

PROCESS *p_proc() { return current_process; }

char *p_name() { return current_process->name; }

int p_pid() { return current_process->pid; }

void _kill() {
    if (current_process->pid == 1) {
        set_task(0);
        panic("Idle can't be killed!");
    }

    printf("Killing process %s (%d)\n", current_process->name, current_process->pid);
    set_task(0);
    free((void *)current_process->stacktop);
    free(current_process);
    pfree( (void *)current_process->registers.cr3);
    current_process->prev->next = current_process->next;
    current_process->next->prev = current_process->prev;
    set_task(1);
    schedule_noirq();
}

/* Let me introduce you to Jack, the ripper.
 * His job is to hunt down and kill zombie processes.
 * He is powerful, don't mess with him! :-)
 */

void jack_the_ripper() {
    reset:;
    PROCESS *orig = current_process;
    PROCESS *process = orig;
    while (1) {
        process = process->next;
        if (process == current_process) {
            continue;
        }
        if (process->state == PROCESS_STATE_ZOMBIE) {
            set_task(0);
            process->prev->next = process->next;
            process->next->prev = process->prev;
            free(process);
            set_task(1);
            printf("Jack killed %s (%d). One less zombie.\n", process->name, process->pid);
        }
        if (process == orig)
            goto reset;
        schedule_noirq();
    }
}

void tasking_print_all() {
    PROCESS *orig = current_process;
    PROCESS *process = orig;
    while (1) {
        printf("Process: %s (%d) %s\n", process->name, process->pid,
               process->state == PROCESS_STATE_ZOMBIE ? "ZOMBIE"
                                                      : process->state == PROCESS_STATE_ALIVE ? "ALIVE"
                                                                                              : "DEAD");
        process = process->next;
        if (process == orig)
            break;
    }
}

void __notified(int sig) {

  switch (sig) {
  case SIG_ILL:
      printf("Received SIGILL, terminating!\n");
    _kill();
    break;
  case SIG_TERM:
      printf("Received SIGTERM, terminating!\n");
    _kill();
    break;
  case SIG_SEGV:
      printf("Received SIGSEGV, terminating!\n");
    _kill();
    break;
  default:
      printf("Received unknown SIG!\n");
    return;
  }
}

int is_pid_running(uint32_t pid) {
    set_task(0);
    PROCESS *process = current_process;
    PROCESS *orig = current_process;
    int ret = 0;
    while (1) {
        if (process->pid == pid) {
            ret = 1;
            break;
        }
        process = process->next;
        if (process == orig)
            break;
    }
  set_task(1);
  return ret;
}

PROCESS *createProcess(char *name, uint32_t addr) {
    PROCESS *process = (PROCESS *) malloc(sizeof(PROCESS));
    memset(process, 0, sizeof(PROCESS));
    process->name = name;
    process->pid = ++lpid;
    process->state = PROCESS_STATE_ALIVE;
    process->notify = __notified;
    process->registers.eip = addr;
    process->registers.esp = (uint32_t) malloc(4096);
    asm volatile("mov %%cr3, %%eax" : "=a"(process->registers.cr3));
    uint32_t* stack = (uint32_t *)(process->registers.esp + 4096);
    process->stacktop = process->registers.esp;
    *--stack = 0x00000202; // eflags
    *--stack = 0x8; // cs
    *--stack = (uint32_t)addr; // eip
    *--stack = 0; // eax
    *--stack = 0; // ebx
    *--stack = 0; // ecx;
    *--stack = 0; //edx
    *--stack = 0; //esi
    *--stack = 0; //edi
    *--stack = process->registers.esp + 4096; //ebp
    *--stack = 0x10; // ds
    *--stack = 0x10; // fs
    *--stack = 0x10; // es
    *--stack = 0x10; // gs
    process->registers.esp = (uint32_t) stack;
    printf("Created task %s with esp=0x%x eip=0x%x\n", process->name, process->registers.esp, process->registers.eip);
    return process;
}

/* This adds a process while no others are running! */
void __addProcess(PROCESS *process){
    process->next = current_process->next;
    process->next->prev = process;
    process->prev = current_process;
    current_process->next = process;
}


/* add process but take care of others also! */
int addProcess(PROCESS *process) {
    set_task(0);
    __addProcess(process);
    set_task(1);
    return process->pid;
}

/* starts tasking */
void __exec() {
    asm volatile("mov %%eax, %%esp" : : "a"(current_process->registers.esp));
    asm volatile("pop %gs");
    asm volatile("pop %fs");
    asm volatile("pop %es");
    asm volatile("pop %ds");
    asm volatile("pop %ebp");
    asm volatile("pop %edi");
    asm volatile("pop %esi");
    asm volatile("pop %edx");
    asm volatile("pop %ecx");
    asm volatile("pop %ebx");
    asm volatile("pop %eax");
    asm volatile("iret");
}

void schedule_noirq() {
  if (!__enabled) return;
  asm volatile("int $0x2e");
}

void schedule() {
    // asm volatile("add $0xc, %esp");
    asm volatile("push %eax");
    asm volatile("push %ebx");
    asm volatile("push %ecx");
    asm volatile("push %edx");
    asm volatile("push %esi");
    asm volatile("push %edi");
    asm volatile("push %ebp");
    asm volatile("push %ds");
    asm volatile("push %es");
    asm volatile("push %fs");
    asm volatile("push %gs");
    asm volatile("mov %%esp, %%eax" : "=a"(current_process->registers.esp));
    current_process = current_process->next;
    asm volatile("mov %%eax, %%cr3" : : "a"(current_process->registers.cr3));
    asm volatile("mov %%eax, %%esp" : : "a"(current_process->registers.esp));
    asm volatile("pop %gs");
    asm volatile("pop %fs");
    asm volatile("pop %es");
    asm volatile("pop %ds");
    asm volatile("pop %ebp");
    asm volatile("pop %edi");
    asm volatile("pop %esi");
    asm volatile("out %%al, %%dx"
            :
            : "d"(0x20), "a"(0x20)); // send EoI to master PIC
    asm volatile("pop %edx");
    asm volatile("pop %ecx");
    asm volatile("pop %ebx");
    asm volatile("pop %eax");
    asm volatile("iret");
}

void tasking_init() {
    printf("Creating idle process\n");
    current_process = createProcess("kidle", (uint32_t) idle_thread);
    current_process->next = current_process;
    current_process->prev = current_process;
    __addProcess(createProcess("task1", (uint32_t) task1));
    __addProcess(createProcess("Jack", (uint32_t) jack_the_ripper));
    __addProcess(createProcess("task2", (uint32_t) task2));
    __addProcess(createProcess("task3", (uint32_t) task3));
    printf("[tasking init] Run exec\n");
    __exec();
    panic("Failed to start tasking!");
}
