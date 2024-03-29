#include <stdint.h>
#include <stdio.h>
#include <memory/memory.h>
#include <threading/pit.h>

#define MAX_PAGE_ALIGNED_ALLOCS 32

uint32_t last_alloc = 0;
uint32_t heap_end = 0;
uint32_t heap_begin = 0;
uint32_t pheap_begin = 0;
uint32_t pheap_end = 0;
uint8_t *pheap_desc = 0;
uint32_t memory_used = 0;

void mm_init(uint32_t kernel_end){
    last_alloc = kernel_end + 0x1000;
    heap_begin = last_alloc;
    pheap_end = 0x400000;
    pheap_begin = pheap_end - (MAX_PAGE_ALIGNED_ALLOCS * 4096);
    heap_end = pheap_begin;
    memset((char*)heap_begin, 0, heap_end - heap_begin);
    pheap_desc = (uint8_t*)malloc(MAX_PAGE_ALIGNED_ALLOCS);
    printf("Kernel heap start at: 0x%x\n", last_alloc);
}

void free(void *mem)
{
    alloc_t *alloc = (mem - sizeof(alloc_t));
    memory_used -= alloc->size + sizeof(alloc_t);
    alloc->status = 0;
}

void pfree(void *mem)
{
    if(mem < (void *)pheap_begin || mem > (void *)pheap_end) return;
    /* Determine which page is it */
    uint32_t ad = (uint32_t)mem;
    ad -= pheap_begin;
    ad /= 4096;
    /* Now, ad has the id of the page */
    pheap_desc[ad] = 0;
    return;
}

char* malloc(size_t size){
    if(!size) return 0;

    uint8_t *mem = (uint8_t*)heap_begin;
    while((uint32_t)mem < last_alloc){
        alloc_t *a = (alloc_t*)mem;

        if(!a->size){
            goto nalloc;
        }

        if(a->status){
            mem += a->size;
            mem += sizeof(alloc_t);
            mem += 4;
            continue;
        }

        if(a->size >= size){
            a->status = 1;

            //printf("RE:Allocated %d bytes from 0x%x to 0x%x\n", size, mem + sizeof(alloc_t), mem + sizeof(alloc_t) + size);
            memset(mem + sizeof(alloc_t), 0, size);
            memory_used += size + sizeof(alloc_t);
            return ((char*)mem + sizeof(alloc_t));
        }

        mem += a->size;
        mem += sizeof(alloc_t);
        mem += 4;
    }

    nalloc:;
    if(last_alloc+size+sizeof(alloc_t) >= heap_end){
        set_task(0);
        panic("Cannot allocate %d bytes! Out of memory.\n", size);
    }
    alloc_t *alloc = (alloc_t*)last_alloc;
    alloc->status = 1;
    alloc->size = size;

    last_alloc += size;
    last_alloc += sizeof(alloc_t);
    last_alloc += 4;
    //printf("Allocated %d bytes from 0x%x to 0x%x\n", size, (uint32_t)alloc + sizeof(alloc_t), last_alloc);
    memory_used += size + 4 + sizeof(alloc_t);
    memset((char*)(uint32_t)last_alloc + sizeof(alloc_t), 0, size);
    return ((char*)(uint32_t)last_alloc + sizeof(alloc_t));
}