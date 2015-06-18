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
	.file	"aligned.c"
	.text
	.align	2
	.global	aligned_peek
	.type	aligned_peek, %function
aligned_peek:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #20
	str	r0, [fp, #-16]
#APP
@ 5 "../aligned.c" 1
	;qui

@ 0 "" 2
	ldr	r3, [fp, #-16]
	ldrh	r3, [r3, #0]	@ movhi
	strh	r3, [fp, #-6]	@ movhi
	ldrh	r3, [fp, #-6]
	mov	r0, r3
	add	sp, fp, #0
	ldmfd	sp!, {fp}
	bx	lr
	.size	aligned_peek, .-aligned_peek
	.ident	"GCC: (4.4.4_09.06.2010) 4.4.4"
	.section	.note.GNU-stack,"",%progbits
