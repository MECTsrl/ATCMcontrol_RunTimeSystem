	.arch armv5te
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 6
	.eabi_attribute 18, 4
	.file	"attribute.c"
	.comm	data,5,4
	.text
	.align	2
	.global	unaligned_peek
	.type	unaligned_peek, %function
unaligned_peek:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #20
	str	r0, [fp, #-16]
	ldr	r3, [fp, #-16]
	ldrh	r3, [r3, #0]	@ movhi
	strh	r3, [fp, #-6]	@ movhi
	ldrh	r3, [fp, #-6]
	mov	r0, r3
	add	sp, fp, #0
	ldmfd	sp!, {fp}
	bx	lr
	.size	unaligned_peek, .-unaligned_peek
	.align	2
	.global	main
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	ldr	r3, .L5
	mov	r2, #50
	strb	r2, [r3, #2]
	ldr	r3, .L5
	mov	r2, #0
	orr	r2, r2, #8
	strb	r2, [r3, #3]
	mov	r2, #0
	orr	r2, r2, #9
	strb	r2, [r3, #4]
	ldr	r0, .L5+4
	bl	unaligned_peek
	ldmfd	sp!, {fp, pc}
.L6:
	.align	2
.L5:
	.word	data
	.word	data+3
	.size	main, .-main
	.ident	"GCC: (4.4.4_09.06.2010) 4.4.4"
	.section	.note.GNU-stack,"",%progbits
