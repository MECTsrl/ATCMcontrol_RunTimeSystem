	.file	"unaligned.c"
	.text
	.align	2
	.global	unaligned_peek
	.type	unaligned_peek, %function
unaligned_peek:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	sub	sp, sp, #16
	str	r0, [fp, #-24]
#APP
	;qui

	ldr	r3, [fp, #-24]
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-16]
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
	sub	sp, fp, #12
	ldmfd	sp, {fp, sp, pc}
	.size	unaligned_peek, .-unaligned_peek
	.align	2
	.global	unaligned_poke
	.type	unaligned_poke, %function
unaligned_poke:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	sub	sp, sp, #16
	mov	r3, r0
	str	r1, [fp, #-28]
	strh	r3, [fp, #-22]	@ movhi
#APP
	;qui

	ldr	r3, [fp, #-28]
	str	r3, [fp, #-16]
	ldrh	r0, [fp, #-22]
	ldr	ip, [fp, #-16]
	and	r1, r0, #255
	mov	r3, #0
	mov	r2, r3
	mov	r3, r1
	orr	r3, r2, r3
	strb	r3, [ip, #0]
	mov	r3, r0, lsr #8
	mov	r3, r3, asl #16
	mov	r1, r3, lsr #16
	mov	r3, #0
	mov	r2, r3
	mov	r3, r1
	orr	r3, r2, r3
	strb	r3, [ip, #1]
	sub	sp, fp, #12
	ldmfd	sp, {fp, sp, pc}
	.size	unaligned_poke, .-unaligned_poke
	.align	2
	.global	unaligned_copy
	.type	unaligned_copy, %function
unaligned_copy:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	sub	sp, sp, #16
	str	r0, [fp, #-24]
	str	r1, [fp, #-28]
#APP
	;qui

	ldr	r3, [fp, #-28]
	str	r3, [fp, #-20]
	ldr	r3, [fp, #-24]
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-20]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldrb	r3, [r3, #1]	@ zero_extendqisi2
	mov	r3, r3, asl #8
	orr	r3, r3, r2
	mov	r3, r3, asl #16
	mov	r3, r3, asr #16
	mov	r3, r3, asl #16
	mov	r0, r3, lsr #16
	ldr	ip, [fp, #-16]
	and	r1, r0, #255
	mov	r3, #0
	mov	r2, r3
	mov	r3, r1
	orr	r3, r2, r3
	strb	r3, [ip, #0]
	mov	r3, r0, lsr #8
	mov	r3, r3, asl #16
	mov	r1, r3, lsr #16
	mov	r3, #0
	mov	r2, r3
	mov	r3, r1
	orr	r3, r2, r3
	strb	r3, [ip, #1]
	sub	sp, fp, #12
	ldmfd	sp, {fp, sp, pc}
	.size	unaligned_copy, .-unaligned_copy
	.align	2
	.global	main
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	sub	sp, sp, #8
	ldr	r3, .L9
	strh	r3, [fp, #-14]	@ movhi
	ldr	r2, .L9+4
	mov	r3, #50
	strb	r3, [r2, #2]
	ldr	r2, .L9+4
	mov	r3, #0
	orr	r3, r3, #8
	strb	r3, [r2, #3]
	mov	r3, #0
	orr	r3, r3, #9
	strb	r3, [r2, #4]
	ldr	r0, .L9+8
	bl	unaligned_peek
	ldr	r0, .L9+12
	ldr	r1, .L9+8
	bl	unaligned_poke
	sub	r3, fp, #14
	mov	r0, r3
	ldr	r1, .L9+8
	bl	unaligned_copy
	sub	sp, fp, #12
	ldmfd	sp, {fp, sp, pc}
.L10:
	.align	2
.L9:
	.word	1284
	.word	data
	.word	data+3
	.word	1798
	.size	main, .-main
	.comm	data,5,1
	.ident	"GCC: (GNU) 4.1.2"
