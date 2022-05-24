#include <memory/hal.h>
#include <memory/pic.h>
#include <stdio.h>
#include <stdint.h>


void hal_init()
{
    printf("HAL inited.\n");
}

void send_eoi(uint8_t irq)
{
    pic_send_eoi(irq);
}

void set_int(int i,uint32_t addr){
    printf("Setting int %d to %x\n",i,addr);
    idt_register_interrupt(i,addr);
}

uint8_t inportb(uint16_t portid){
    uint8_t ret;
    asm volatile("inb %%dx,%%al":"=a"(ret):"d"(portid));
    return ret;
}
uint16_t inportw(uint16_t portid){
    uint16_t ret;
    asm volatile("inw %%dx,%%ax":"=a"(ret):"d"(portid));
    return ret;
}

uint32_t inportl(uint16_t portid){
    uint32_t ret;
    asm volatile("inl %%dx,%%eax":"=a"(ret):"d"(portid));
    return ret;
}

void outportb(uint16_t portid,uint8_t val){
    asm volatile("outb %%al,%%dx"::"d"(portid),"a"(val));
}

void outportw(uint16_t portid,uint16_t val){
    asm volatile("outw %%ax,%%dx"::"d"(portid),"a"(val));
}

void outportl(uint16_t portid,uint32_t val){
    asm volatile("outl %%eax,%%dx"::"d"(portid),"a"(val));
}