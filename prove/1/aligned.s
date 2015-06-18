	.file	"aligned.c"
	.text
	.align	2
	.global	aligned_peek
	.type	aligned_peek, %function
aligned_peek:
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
	ldrh	r3, [r3, #0]	@ movhi
	strh	r3, [fp, #-14]	@ movhi
	ldrh	r3, [fp, #-14]
	mov	r0, r3
	sub	sp, fp, #12
	ldmfd	sp, {fp, sp, pc}
	.size	aligned_peek, .-aligned_peek
	.ident	"GCC: (GNU) 4.1.2"
