	.file	"attribute.c"
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
	@unaligned=*p

	ldr	r3, [fp, #-24]
	ldrh	r3, [r3, #0]	@ movhi
	strh	r3, [fp, #-14]	@ movhi
	ldrh	r3, [fp, #-14]
	mov	r0, r3
	sub	sp, fp, #12
	ldmfd	sp, {fp, sp, pc}
	.size	unaligned_peek, .-unaligned_peek
	.align	2
	.global	main
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	ldr	r2, .L5
	mov	r3, #50
	strb	r3, [r2, #2]
#APP
	@data.instr=0x0908

	ldr	r2, .L5
	mov	r3, #0
	orr	r3, r3, #8
	strb	r3, [r2, #3]
	mov	r3, #0
	orr	r3, r3, #9
	strb	r3, [r2, #4]
	ldr	r0, .L5+4
	bl	unaligned_peek
	ldmfd	sp, {fp, sp, pc}
.L6:
	.align	2
.L5:
	.word	data
	.word	data+3
	.size	main, .-main
	.comm	data,5,1
	.ident	"GCC: (GNU) 4.1.2"
