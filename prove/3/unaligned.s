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
	.file	"unaligned.c"
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
#APP
@ 35 "../unaligned.c" 1
	;qui

@ 0 "" 2
	ldr	r3, [fp, #-16]
	str	r3, [fp, #-8]
	ldr	r3, [fp, #-8]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldrb	r3, [r3, #1]	@ zero_extendqisi2
	mov	r3, r3, asl #8
	orr	r3, r3, r2
	mov	r3, r3, asl #16
	mov	r3, r3, asr #16
	mov	r3, r3, asl #16
	mov	r3, r3, lsr #16
	mov	r3, r3, asl #16
	mov	r3, r3, lsr #16
	mov	r0, r3
	add	sp, fp, #0
	ldmfd	sp!, {fp}
	bx	lr
	.size	unaligned_peek, .-unaligned_peek
	.align	2
	.global	unaligned_poke
	.type	unaligned_poke, %function
unaligned_poke:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #20
	mov	r3, r0
	str	r1, [fp, #-20]
	strh	r3, [fp, #-14]	@ movhi
#APP
@ 41 "../unaligned.c" 1
	;qui

@ 0 "" 2
	ldr	r3, [fp, #-20]
	str	r3, [fp, #-8]
	ldrh	r2, [fp, #-14]
	ldr	r3, [fp, #-8]
	and	r1, r2, #255
	mov	r0, #0
	orr	r1, r0, r1
	strb	r1, [r3, #0]
	mov	r2, r2, lsr #8
	mov	r2, r2, asl #16
	mov	r2, r2, lsr #16
	mov	r1, #0
	orr	r2, r1, r2
	strb	r2, [r3, #1]
	add	sp, fp, #0
	ldmfd	sp!, {fp}
	bx	lr
	.size	unaligned_poke, .-unaligned_poke
	.align	2
	.global	unaligned_copy
	.type	unaligned_copy, %function
unaligned_copy:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #20
	str	r0, [fp, #-16]
	str	r1, [fp, #-20]
#APP
@ 47 "../unaligned.c" 1
	;qui

@ 0 "" 2
	ldr	r3, [fp, #-20]
	str	r3, [fp, #-12]
	ldr	r3, [fp, #-16]
	str	r3, [fp, #-8]
	ldr	r3, [fp, #-12]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldrb	r3, [r3, #1]	@ zero_extendqisi2
	mov	r3, r3, asl #8
	orr	r3, r3, r2
	mov	r3, r3, asl #16
	mov	r3, r3, asr #16
	mov	r3, r3, asl #16
	mov	r2, r3, lsr #16
	ldr	r3, [fp, #-8]
	and	r1, r2, #255
	mov	r0, #0
	orr	r1, r0, r1
	strb	r1, [r3, #0]
	mov	r2, r2, lsr #8
	mov	r2, r2, asl #16
	mov	r2, r2, lsr #16
	mov	r1, #0
	orr	r2, r1, r2
	strb	r2, [r3, #1]
	add	sp, fp, #0
	ldmfd	sp!, {fp}
	bx	lr
	.size	unaligned_copy, .-unaligned_copy
	.align	2
	.global	main
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #8
	ldr	r3, .L9
	strh	r3, [fp, #-6]	@ movhi
	ldr	r3, .L9+4
	mov	r2, #50
	strb	r2, [r3, #2]
	ldr	r3, .L9+4
	mov	r2, #0
	orr	r2, r2, #8
	strb	r2, [r3, #3]
	mov	r2, #0
	orr	r2, r2, #9
	strb	r2, [r3, #4]
	ldr	r0, .L9+8
	bl	unaligned_peek
	ldr	r0, .L9+12
	ldr	r1, .L9+8
	bl	unaligned_poke
	sub	r3, fp, #6
	mov	r0, r3
	ldr	r1, .L9+8
	bl	unaligned_copy
	sub	sp, fp, #4
	ldmfd	sp!, {fp, pc}
.L10:
	.align	2
.L9:
	.word	1284
	.word	data
	.word	data+3
	.word	1798
	.size	main, .-main
	.ident	"GCC: (4.4.4_09.06.2010) 4.4.4"
	.section	.note.GNU-stack,"",%progbits
