#include <stdio.h>
#include <stdint.h>
#include <idt/idt.h>
#include <threading/tasking.h>
#include <memory/hal.h>

static uint32_t idt_location = 0;
static uint32_t idtr_location = 0;
static uint16_t idt_size = 0x800;

static uint8_t test_success = 0;
static uint32_t test_timeout = 0x1000;

extern void _set_idtr();

void __idt_default_handler();
void __idt_test_handler();

static uint8_t  __idt_setup = 0;

void idt_init()
{
    idt_location = 0x2000;
    printf("Location: 0x%x\n", idt_location);
    idtr_location = 0x10F0;
    printf("IDTR Location: 0x%x\n", idtr_location);
    __idt_setup = 1;
    for(uint8_t i = 0;i < 255;i++){
        idt_register_interrupt(i,(uint32_t)&__idt_default_handler);
    }
    idt_register_interrupt(0x2f,(uint32_t)&__idt_test_handler);
    idt_register_interrupt(0x2e,(uint32_t)&schedule);
    printf("Registered all interrupts to default handler\n");
    *(uint16_t*)idtr_location = idt_size - 1;
    *(uint32_t*)(idtr_location + 2) = idt_location;
    printf("IDTR.size = 0x%x IDTR.offset = 0x%x\n", *(uint16_t*)idtr_location, *(uint32_t*)(idtr_location + 2));
    _set_idtr();
    printf("IDT set, testing...\n");
    asm volatile("int $0x2f");
    while(test_timeout-- != 0)
    {
        if(test_success != 0)
        {
            printf("Test success! Disabling INT#0x2F\n");
            idt_register_interrupt(0x2f,(uint32_t)&__idt_default_handler);
            break;
        }
    }
    if(!test_success) panic ("Test failed!(timeout)");
    return;
}

void __idt_test_handler()
{
    INT_START;
    test_success = 1;
    INT_END;
}

void idt_register_interrupt(uint8_t i, uint32_t callback)
{
    if(!__idt_setup) panic("IDT not setup!");
    *(uint16_t*)(idt_location + 8*i + 0) = (uint16_t)(callback & 0x0000ffff);
    *(uint16_t*)(idt_location + 8*i + 2) = (uint16_t)0x8;
    *(uint8_t*)(idt_location + 8*i + 4) = 0x00;
    *(uint8_t*) (idt_location + 8*i + 5) = 0x8e;
    *(uint16_t*)(idt_location + 8*i + 6) = (uint16_t)((callback & 0xffff0000) >> 16);
    if(test_success) printf("Registered INT#%d\n", i);
    return;
}
