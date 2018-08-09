	.syntax unified
	.arch armv7-a
	.eabi_attribute 27, 3
	.eabi_attribute 28, 1
	.fpu vfpv3-d16
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 6
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.thumb
	.file	"gtod_loop.c"
	.global	trace_fd
	.data
	.align	2
	.type	trace_fd, %object
	.size	trace_fd, 4
trace_fd:
	.word	-1
	.global	marker_fd
	.align	2
	.type	marker_fd, %object
	.size	marker_fd, 4
marker_fd:
	.word	-1
	.section	.rodata
	.align	2
.LC0:
	.ascii	"/proc/mounts\000"
	.align	2
.LC1:
	.ascii	"r\000"
	.align	2
.LC2:
	.ascii	"debugfs\000"
	.align	2
.LC3:
	.ascii	"%*s %256s %99s %*s %*d %*d\012\000"
	.text
	.align	2
	.thumb
	.thumb_func
	.type	find_debugfs, %function
find_debugfs:
	@ args = 0, pretend = 0, frame = 104
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	sub	sp, sp, #104
	add	r7, sp, #0
	movw	r3, #:lower16:debugfs_found.5475
	movt	r3, #:upper16:debugfs_found.5475
	ldr	r3, [r3]
	cmp	r3, #0
	beq	.L2
	movw	r3, #:lower16:debugfs.5474
	movt	r3, #:upper16:debugfs.5474
	b	.L9
.L2:
	movw	r0, #:lower16:.LC0
	movt	r0, #:upper16:.LC0
	movw	r1, #:lower16:.LC1
	movt	r1, #:upper16:.LC1
	bl	fopen
	str	r0, [r7, #100]
	ldr	r3, [r7, #100]
	cmp	r3, #0
	bne	.L4
	movs	r3, #0
	b	.L9
.L4:
	b	.L5
.L7:
	mov	r3, r7
	mov	r0, r3
	movw	r1, #:lower16:.LC2
	movt	r1, #:upper16:.LC2
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	bne	.L5
	b	.L6
.L5:
	mov	r3, r7
	ldr	r0, [r7, #100]
	movw	r1, #:lower16:.LC3
	movt	r1, #:upper16:.LC3
	movw	r2, #:lower16:debugfs.5474
	movt	r2, #:upper16:debugfs.5474
	bl	__isoc99_fscanf
	mov	r3, r0
	cmp	r3, #2
	beq	.L7
.L6:
	ldr	r0, [r7, #100]
	bl	fclose
	mov	r3, r7
	mov	r0, r3
	movw	r1, #:lower16:.LC2
	movt	r1, #:upper16:.LC2
	bl	strcmp
	mov	r3, r0
	cmp	r3, #0
	beq	.L8
	movs	r3, #0
	b	.L9
.L8:
	movw	r3, #:lower16:debugfs_found.5475
	movt	r3, #:upper16:debugfs_found.5475
	movs	r2, #1
	str	r2, [r3]
	movw	r3, #:lower16:debugfs.5474
	movt	r3, #:upper16:debugfs.5474
.L9:
	mov	r0, r3
	adds	r7, r7, #104
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
	.size	find_debugfs, .-find_debugfs
	.section	.rodata
	.align	2
.LC4:
	.ascii	"1\000"
	.align	2
.LC5:
	.ascii	"Before GTOD\012\000"
	.align	2
.LC6:
	.ascii	"After GTOD\012\000"
	.align	2
.LC7:
	.ascii	"0\000"
	.align	2
.LC8:
	.ascii	"%ld\012\000"
	.align	2
.LC9:
	.ascii	"ignore this number (trying to trick the compiler) %"
	.ascii	"ld\012\000"
	.text
	.align	2
	.global	gt
	.thumb
	.thumb_func
	.type	gt, %function
gt:
	@ args = 0, pretend = 0, frame = 8000040
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	sub	sp, sp, #7995392
	sub	sp, sp, #4640
	sub	sp, sp, #8
	add	r7, sp, #0
	movw	r3, #:lower16:trace_fd
	movt	r3, #:upper16:trace_fd
	ldr	r3, [r3]
	mov	r0, r3
	movw	r1, #:lower16:.LC4
	movt	r1, #:upper16:.LC4
	movs	r2, #1
	bl	write
	movw	r3, #:lower16:marker_fd
	movt	r3, #:upper16:marker_fd
	ldr	r3, [r3]
	mov	r0, r3
	movw	r1, #:lower16:.LC5
	movt	r1, #:upper16:.LC5
	movs	r2, #12
	bl	write
	add	r3, r7, #4640
	add	r3, r3, #8
	sub	r3, r3, #4640
	mov	r0, r3
	movs	r1, #0
	bl	gettimeofday
	add	r3, r7, #4640
	add	r3, r3, #8
	sub	r3, r3, #4640
	ldr	r3, [r3]
	add	r2, r7, #7995392
	add	r2, r2, #4640
	str	r3, [r2]
	add	r3, r7, #4640
	add	r3, r3, #8
	sub	r3, r3, #4640
	ldr	r3, [r3, #4]
	movw	r2, #4636
	movt	r2, 122
	add	r2, r7, r2
	str	r3, [r2]
	movw	r3, #60892
	movt	r3, 65413
	movw	r2, #4648
	movt	r2, 122
	add	r2, r7, r2
	add	r3, r3, r2
	movs	r2, #0
	str	r2, [r3]
	b	.L11
.L12:
	movw	r3, #60892
	movt	r3, 65413
	movw	r2, #4648
	movt	r2, 122
	add	r2, r7, r2
	add	r3, r3, r2
	ldr	r2, [r3]
	add	r3, r7, #4640
	add	r3, r3, #8
	sub	r3, r3, #4640
	lsls	r2, r2, #3
	add	r3, r3, r2
	mov	r0, r3
	movs	r1, #0
	bl	gettimeofday
	movw	r3, #60892
	movt	r3, 65413
	movw	r2, #4648
	movt	r2, 122
	add	r2, r7, r2
	add	r3, r3, r2
	ldr	r3, [r3]
	adds	r2, r3, #1
	movw	r3, #60892
	movt	r3, 65413
	movw	r1, #4648
	movt	r1, 122
	add	r1, r7, r1
	add	r3, r3, r1
	str	r2, [r3]
.L11:
	movw	r3, #60892
	movt	r3, 65413
	movw	r2, #4648
	movt	r2, 122
	add	r2, r7, r2
	add	r3, r3, r2
	ldr	r2, [r3]
	movw	r3, #16959
	movt	r3, 15
	cmp	r2, r3
	ble	.L12
	movw	r3, #:lower16:marker_fd
	movt	r3, #:upper16:marker_fd
	ldr	r3, [r3]
	mov	r0, r3
	movw	r1, #:lower16:.LC6
	movt	r1, #:upper16:.LC6
	movs	r2, #11
	bl	write
	movw	r3, #:lower16:trace_fd
	movt	r3, #:upper16:trace_fd
	ldr	r3, [r3]
	mov	r0, r3
	movw	r1, #:lower16:.LC7
	movt	r1, #:upper16:.LC7
	movs	r2, #1
	bl	write
	add	r2, r7, #4640
	add	r2, r2, #8
	sub	r2, r2, #4640
	movw	r3, #4600
	movt	r3, 122
	add	r3, r3, r2
	ldr	r3, [r3]
	movw	r2, #4632
	movt	r2, 122
	add	r2, r7, r2
	str	r3, [r2]
	add	r2, r7, #4640
	add	r2, r2, #8
	sub	r2, r2, #4640
	movw	r3, #4604
	movt	r3, 122
	add	r3, r3, r2
	ldr	r3, [r3]
	movw	r2, #4628
	movt	r2, 122
	add	r2, r7, r2
	str	r3, [r2]
	movw	r3, #4628
	movt	r3, 122
	add	r3, r7, r3
	ldr	r2, [r3]
	movw	r3, #4636
	movt	r3, 122
	add	r3, r7, r3
	ldr	r3, [r3]
	cmp	r2, r3
	ble	.L13
	movw	r3, #4632
	movt	r3, 122
	add	r3, r7, r3
	ldr	r2, [r3]
	add	r3, r7, #7995392
	add	r3, r3, #4640
	ldr	r3, [r3]
	subs	r3, r2, r3
	movw	r0, #:lower16:.LC8
	movt	r0, #:upper16:.LC8
	mov	r1, r3
	bl	printf
	movw	r3, #4628
	movt	r3, 122
	add	r3, r7, r3
	ldr	r2, [r3]
	movw	r3, #4636
	movt	r3, 122
	add	r3, r7, r3
	ldr	r3, [r3]
	subs	r3, r2, r3
	movw	r0, #:lower16:.LC8
	movt	r0, #:upper16:.LC8
	mov	r1, r3
	bl	printf
	b	.L14
.L13:
	movw	r3, #4632
	movt	r3, 122
	add	r3, r7, r3
	ldr	r3, [r3]
	subs	r2, r3, #1
	add	r3, r7, #7995392
	add	r3, r3, #4640
	ldr	r3, [r3]
	subs	r3, r2, r3
	movw	r0, #:lower16:.LC8
	movt	r0, #:upper16:.LC8
	mov	r1, r3
	bl	printf
	movw	r3, #4628
	movt	r3, 122
	add	r3, r7, r3
	ldr	r3, [r3]
	add	r3, r3, #999424
	add	r3, r3, #576
	movw	r2, #4636
	movt	r2, 122
	add	r2, r7, r2
	ldr	r2, [r2]
	subs	r3, r3, r2
	movw	r0, #:lower16:.LC8
	movt	r0, #:upper16:.LC8
	mov	r1, r3
	bl	printf
.L14:
	movw	r3, #60892
	movt	r3, 65413
	movw	r2, #4648
	movt	r2, 122
	add	r2, r7, r2
	add	r3, r3, r2
	movs	r2, #2
	str	r2, [r3]
	b	.L15
.L16:
	movw	r3, #60892
	movt	r3, 65413
	movw	r2, #4648
	movt	r2, 122
	add	r2, r7, r2
	add	r3, r3, r2
	ldr	r2, [r3]
	add	r3, r7, #4640
	add	r3, r3, #8
	sub	r3, r3, #4640
	ldr	r1, [r3, r2, lsl #3]
	movw	r3, #60892
	movt	r3, 65413
	movw	r2, #4648
	movt	r2, 122
	add	r2, r7, r2
	add	r3, r3, r2
	ldr	r3, [r3]
	subs	r3, r3, #1
	add	r2, r7, #4640
	add	r2, r2, #8
	sub	r2, r2, #4640
	lsls	r3, r3, #3
	add	r3, r3, r2
	ldr	r3, [r3, #4]
	subs	r3, r1, r3
	movw	r2, #4644
	movt	r2, 122
	add	r2, r7, r2
	ldr	r2, [r2]
	add	r3, r3, r2
	movw	r2, #4644
	movt	r2, 122
	add	r2, r7, r2
	str	r3, [r2]
	movw	r3, #60892
	movt	r3, 65413
	movw	r2, #4648
	movt	r2, 122
	add	r2, r7, r2
	add	r3, r3, r2
	ldr	r3, [r3]
	adds	r2, r3, #1
	movw	r3, #60892
	movt	r3, 65413
	movw	r1, #4648
	movt	r1, 122
	add	r1, r7, r1
	add	r3, r3, r1
	str	r2, [r3]
.L15:
	movw	r3, #60892
	movt	r3, 65413
	movw	r2, #4648
	movt	r2, 122
	add	r2, r7, r2
	add	r3, r3, r2
	ldr	r2, [r3]
	movw	r3, #16959
	movt	r3, 15
	cmp	r2, r3
	ble	.L16
	movw	r0, #:lower16:.LC9
	movt	r0, #:upper16:.LC9
	movw	r3, #4644
	movt	r3, 122
	add	r3, r7, r3
	ldr	r1, [r3]
	bl	printf
	movw	r3, #4632
	movt	r3, 122
	add	r3, r7, r3
	ldr	r3, [r3]
	fmsr	s15, r3	@ int
	fsitod	d6, s15
	movw	r3, #4628
	movt	r3, 122
	add	r3, r7, r3
	ldr	r3, [r3]
	fmsr	s15, r3	@ int
	fsitod	d7, s15
	fldd	d5, .L18
	fdivd	d7, d7, d5
	faddd	d7, d6, d7
	movw	r3, #4616
	movt	r3, 122
	add	r3, r7, r3
	fstd	d7, [r3]
	movw	r3, #4616
	movt	r3, 122
	add	r3, r7, r3
	ldrd	r2, [r3]
	fmdrr	d7, r2, r3
	fcpyd	d0, d7
	add	r7, r7, #7995392
	add	r7, r7, #4640
	adds	r7, r7, #8
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
.L19:
	.align	3
.L18:
	.word	0
	.word	1093567616
	.size	gt, .-gt
	.section	.rodata
	.align	2
.LC10:
	.ascii	"/tracing/tracing_on\000"
	.align	2
.LC11:
	.ascii	"/tracing/trace_marker\000"
	.text
	.align	2
	.global	main
	.thumb
	.thumb_func
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 264
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r4, r5, r7, lr}
	sub	sp, sp, #264
	add	r7, sp, #0
	bl	find_debugfs
	str	r0, [r7, #260]
	ldr	r3, [r7, #260]
	cmp	r3, #0
	beq	.L21
	adds	r3, r7, #4
	mov	r0, r3
	ldr	r1, [r7, #260]
	bl	strcpy
	adds	r3, r7, #4
	mov	r0, r3
	bl	strlen
	mov	r3, r0
	mov	r2, r3
	adds	r3, r7, #4
	add	r2, r2, r3
	movw	r3, #:lower16:.LC10
	movt	r3, #:upper16:.LC10
	mov	r5, r2
	mov	r4, r3
	ldmia	r4!, {r0, r1, r2, r3}
	str	r0, [r5]	@ unaligned
	str	r1, [r5, #4]	@ unaligned
	str	r2, [r5, #8]	@ unaligned
	str	r3, [r5, #12]	@ unaligned
	ldr	r0, [r4]	@ unaligned
	str	r0, [r5, #16]	@ unaligned
	adds	r3, r7, #4
	mov	r0, r3
	movs	r1, #1
	bl	open
	mov	r2, r0
	movw	r3, #:lower16:trace_fd
	movt	r3, #:upper16:trace_fd
	str	r2, [r3]
	adds	r3, r7, #4
	mov	r0, r3
	ldr	r1, [r7, #260]
	bl	strcpy
	adds	r3, r7, #4
	mov	r0, r3
	bl	strlen
	mov	r3, r0
	mov	r2, r3
	adds	r3, r7, #4
	add	r2, r2, r3
	movw	r3, #:lower16:.LC11
	movt	r3, #:upper16:.LC11
	mov	r5, r2
	mov	r4, r3
	ldmia	r4!, {r0, r1, r2, r3}
	str	r0, [r5]	@ unaligned
	str	r1, [r5, #4]	@ unaligned
	str	r2, [r5, #8]	@ unaligned
	str	r3, [r5, #12]	@ unaligned
	ldr	r0, [r4]	@ unaligned
	str	r0, [r5, #16]	@ unaligned
	ldrh	r3, [r4, #4]	@ unaligned
	strh	r3, [r5, #20]	@ unaligned
	adds	r3, r7, #4
	mov	r0, r3
	movs	r1, #1
	bl	open
	mov	r2, r0
	movw	r3, #:lower16:marker_fd
	movt	r3, #:upper16:marker_fd
	str	r2, [r3]
.L21:
	bl	gt
	movs	r3, #0
	mov	r0, r3
	add	r7, r7, #264
	mov	sp, r7
	@ sp needed
	pop	{r4, r5, r7, pc}
	.size	main, .-main
	.local	debugfs_found.5475
	.comm	debugfs_found.5475,4,4
	.local	debugfs.5474
	.comm	debugfs.5474,257,4
	.ident	"GCC: (Debian 4.9.2-10) 4.9.2"
	.section	.note.GNU-stack,"",%progbits
