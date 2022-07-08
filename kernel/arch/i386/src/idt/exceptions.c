#include <idt/idt.h>
#include <stdio.h>
#include <threading/tasking.h>
#include <kernel/signal.h>
#include <threading/pit.h>
#include <stdint.h>

void exc_divide_by_zero()
{
    //kerror("Divide by zero at [0x%x:0x%x] EFLAGS: 0x%x\n", cs, eip, eflags);
    panic("Divide by zero!\n");
    if(is_tasking()) {
        panic("Notifying process %s (%d) with SIGILL\n", p_name(), p_pid());
        send_sig(SIG_ILL);
    }
}

void exc_debug()
{
    panic("Debug!\n");
    if(is_tasking()) {
        panic("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
        send_sig(SIG_TERM);
    }
}

void exc_nmi()
{
    panic("NMI\n");
    /*if(is_tasking()) {
        panic("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
        send_sig(SIG_TERM);
    }*/
    return;
}

void exc_brp()
{
    panic("Breakpoint!\n");
    return;
}

void exc_overflow()
{
    panic("Overflow!\n");
    if(is_tasking()) {
        panic("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
        send_sig(SIG_TERM);
    }
    return;
}

void exc_bound()
{
    panic("Bound range exceeded.\n");
    if(is_tasking()) {
        panic("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
        send_sig(SIG_TERM);
    }
    return;
}

void exc_invopcode()
{
    printf("Invalid opcode.\n");
    if(is_tasking()) {
        send_sig(SIG_TERM);
        printf("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
    }
    return;
}

void exc_device_not_avail()
{
    panic("Device not available.\n");
    if(is_tasking()) {
        send_sig(SIG_TERM);
        panic("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
    }
    return;
}

void exc_double_fault()
{
    set_task(0);
    panic("Double fault, halting.\n");
    for(;;);
}

void exc_coproc()
{
    set_task(0);
    panic("Coprocessor fault, halting.\n");
    for(;;);
    return;
}

void exc_invtss()
{
    panic("TSS invalid.\n");
    if(is_tasking()) {
        send_sig(SIG_TERM);
        panic("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
    }
    return;
}

void exc_segment_not_present()
{
    panic("Segment not present.\n");
    if(is_tasking()) {
        send_sig(SIG_SEGV);
        panic("Notifying process %s (%d) with SIGSEGV\n", p_name(), p_pid());
    }
    return;
}

void exc_ssf()
{
    panic("Stacksegment faulted.\n");
    if(is_tasking()) {
        send_sig(SIG_TERM);
        panic("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
    }
    return;
}

void exc_gpf()
{
    panic("General protection fault.\n");
    if(is_tasking()) {
        send_sig(SIG_TERM);
        panic("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
    }
    return;
}

void exc_pf()
{
    panic("Page fault in %s (%d)\n",p_name(), p_pid());
    if(is_tasking()) {
        send_sig(SIG_TERM);
        panic("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
    }
    return;
}

void exc_reserved()
{
    set_task(0);
    panic("This shouldn't happen. Halted.\n");
    for(;;);
    return;
}

void exception_init()
{
    printf("Installing exceptions handlers\n");
    idt_register_interrupt(0, (uint32_t)exc_divide_by_zero);
    idt_register_interrupt(1, (uint32_t)exc_debug);
    idt_register_interrupt(2, (uint32_t)exc_nmi);
    idt_register_interrupt(3, (uint32_t)exc_brp);
    idt_register_interrupt(4, (uint32_t)exc_overflow);
    idt_register_interrupt(5, (uint32_t)exc_bound);
    idt_register_interrupt(6, (uint32_t)exc_invopcode);
    idt_register_interrupt(7, (uint32_t)exc_device_not_avail);
    idt_register_interrupt(8, (uint32_t)exc_double_fault);
    idt_register_interrupt(9, (uint32_t)exc_coproc);
    idt_register_interrupt(10, (uint32_t)exc_invtss);
    idt_register_interrupt(11, (uint32_t)exc_segment_not_present);
    idt_register_interrupt(12, (uint32_t)exc_ssf);
    idt_register_interrupt(13, (uint32_t)exc_gpf);
    idt_register_interrupt(14, (uint32_t)exc_pf);
    idt_register_interrupt(15, (uint32_t)exc_reserved);
    return;
}