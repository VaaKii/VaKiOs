# Declare constants for the multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Reserve a stack for the initial thread.
.section .bootstrap_stack
.align 16
stack_bottom:
.skip 32768 # 16 KiB
stack_top:

.section .text

.global _set_gdtr
.type _set_gdtr, @function
 _set_gdtr:
 	push %ebp
 	movl %esp, %ebp

 	lgdt 0x800

 	movl %ebp, %esp
 	pop %ebp
 	ret

.global _reload_segments
.type _reload_segments, @function
_reload_segments:
	push %ebp
	movl %esp, %ebp

	push %eax
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	pop %eax

	ljmp $0x8, $me
me:
	movl %ebp, %esp
	pop %ebp
	ret

# The kernel entry point.
.global _start
.type _start, @function
_start:
	movl $stack_top, %esp
    #mov $0x1337, %eax
	# Call the global constructors.
	#call _init

	# Transfer control to the main kernel.
	call kernel_main

	# Hang if kernel_main unexpectedly returns.
	cli
1:	hlt
	jmp 1b
.size _start, . - _start
