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
	.file	"prova.c"
	.text
	.align	2
	.global	unaligned_peek
	.type	unaligned_peek, %function
unaligned_peek:
	.fnstart
.LFB2:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	.save {fp}
.LCFI0:
	.setfp fp, sp, #0
	add	fp, sp, #0
.LCFI1:
	.pad #20
	sub	sp, sp, #20
.LCFI2:
	str	r0, [fp, #-16]
#APP
@ 16 "../prova.c" 1
	@unaligned=*p

@ 0 "" 2
	ldr	r3, [fp, #-16]
	ldrb	r2, [r3, #0]	@ zero_extendqisi2
	ldrb	r3, [r3, #1]	@ zero_extendqisi2
	mov	r3, r3, asl #8
	orr	r3, r3, r2
	strh	r3, [fp, #-6]	@ movhi
	ldrh	r3, [fp, #-6]
	mov	r0, r3
	add	sp, fp, #0
	ldmfd	sp!, {fp}
	bx	lr
.LFE2:
	.fnend
	.size	unaligned_peek, .-unaligned_peek
	.align	2
	.global	main
	.type	main, %function
main:
	.fnstart
.LFB3:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	.save {fp, lr}
.LCFI3:
	.setfp fp, sp, #4
	add	fp, sp, #4
.LCFI4:
	.pad #8
	sub	sp, sp, #8
.LCFI5:
	ldr	r2, .L5
	mov	r3, #50
	strb	r3, [r2, #2]
#APP
@ 24 "../prova.c" 1
	@data.instr=0x0908

@ 0 "" 2
	ldr	r2, .L5
	mov	r3, #0
	orr	r3, r3, #8
	strb	r3, [r2, #3]
	mov	r3, #0
	orr	r3, r3, #9
	strb	r3, [r2, #4]
	ldr	r3, .L5+4
	mov	r0, r3
	bl	unaligned_peek
	sub	sp, fp, #4
	ldmfd	sp!, {fp, pc}
.L6:
	.align	2
.L5:
	.word	data
	.word	data+3
.LFE3:
	.fnend
	.size	main, .-main
	.comm	data,5,1
	.ident	"GCC: (Sourcery G++ Lite 2009q1-203) 4.3.3"
	.section	.note.GNU-stack,"",%progbits
