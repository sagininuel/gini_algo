	.file	"main.c"
	.text
	.globl	sum_t
	.bss
	.align 8
	.type	sum_t, @object
	.size	sum_t, 8
sum_t:
	.zero	8
	.local	current_location_counter
	.comm	current_location_counter,4,4
	.section	.rodata
.LC0:
	.string	"Current location counter"
	.section	.data.rel.local,"aw"
	.align 8
	.type	name, @object
	.size	name, 8
name:
	.quad	.LC0
	.section	.rodata
.LC1:
	.string	"Pointer to sum_t: %p\n"
.LC2:
	.string	"Sum: %d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	add@GOTPCREL(%rip), %rax
	movq	%rax, %rsi
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	add@GOTPCREL(%rip), %rax
	movq	%rax, sum_t(%rip)
	movq	sum_t(%rip), %rax
	movl	$3, %esi
	movl	$2, %edi
	call	*%rax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
