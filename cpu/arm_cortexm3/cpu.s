	.syntax unified
	.cpu cortex-m3
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 2
	.eabi_attribute 18, 4
	.thumb
	.file	"cpu.c"
	.section	.debug_abbrev,"",%progbits
.Ldebug_abbrev0:
	.section	.debug_info,"",%progbits
.Ldebug_info0:
	.section	.debug_line,"",%progbits
.Ldebug_line0:
	.text
.Ltext0:
	.align	2
	.global	timer_init
	.thumb
	.thumb_func
	.type	timer_init, %function
timer_init:
.LFB32:
	.file 1 "cpu.c"
	.loc 1 31 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	@ lr needed for prologue
	.loc 1 33 0
	movs	r0, #0
	bx	lr
.LFE32:
	.size	timer_init, .-timer_init
	.align	2
	.global	dram_init
	.thumb
	.thumb_func
	.type	dram_init, %function
dram_init:
.LFB33:
	.loc 1 36 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	@ lr needed for prologue
	.loc 1 38 0
	movs	r0, #0
	bx	lr
.LFE33:
	.size	dram_init, .-dram_init
	.align	2
	.global	print_cpuinfo
	.thumb
	.thumb_func
	.type	print_cpuinfo, %function
print_cpuinfo:
.LFB34:
	.loc 1 41 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	@ lr needed for prologue
	.loc 1 43 0
	movs	r0, #0
	bx	lr
.LFE34:
	.size	print_cpuinfo, .-print_cpuinfo
	.align	2
	.global	checkboard
	.thumb
	.thumb_func
	.type	checkboard, %function
checkboard:
.LFB35:
	.loc 1 46 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	@ lr needed for prologue
	.loc 1 48 0
	movs	r0, #0
	bx	lr
.LFE35:
	.size	checkboard, .-checkboard
	.align	2
	.global	flash_init
	.thumb
	.thumb_func
	.type	flash_init, %function
flash_init:
.LFB36:
	.loc 1 51 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	@ lr needed for prologue
	.loc 1 53 0
	movs	r0, #0
	bx	lr
.LFE36:
	.size	flash_init, .-flash_init
	.section	.ramcode,"ax",%progbits
	.align	2
	.global	test_func
	.thumb
	.thumb_func
	.type	test_func, %function
test_func:
.LFB37:
	.loc 1 58 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	.loc 1 59 0
	movw	r0, #:lower16:.LC0
	movw	r3, #:lower16:my_puts
	.loc 1 58 0
	push	{r4, lr}
.LCFI0:
	.loc 1 59 0
	movt	r3, #:upper16:my_puts
	movt	r0, #:upper16:.LC0
	blx	r3
	.loc 1 60 0
	pop	{r4, pc}
.LFE37:
	.size	test_func, .-test_func
	.text
	.align	2
	.global	arch_cpu_init
	.thumb
	.thumb_func
	.type	arch_cpu_init, %function
arch_cpu_init:
.LFB31:
	.loc 1 6 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
.LCFI1:
	.loc 1 11 0
	bl	SystemCoreClockUpdate
	.loc 1 17 0
	movw	r2, #:lower16:-536600576
	movt	r2, #:upper16:-536600576
	ldr	r3, [r2, #48]
	.loc 1 18 0
	movw	r1, #:lower16:1073762304
	.loc 1 17 0
	bic	r3, r3, #64
	str	r3, [r2, #48]
	.loc 1 18 0
	movt	r1, #:upper16:1073762304
	movs	r4, #0
	.loc 1 19 0
	movs	r3, #3
	.loc 1 18 0
	str	r4, [r1, #84]
	.loc 1 25 0
	mov	r0, #115200
	.loc 1 19 0
	str	r3, [r1, #12]
	.loc 1 25 0
	bl	my_uart_init
	.loc 1 28 0
	mov	r0, r4
	pop	{r4, pc}
.LFE31:
	.size	arch_cpu_init, .-arch_cpu_init
	.section	.rodata.str1.4,"aMS",%progbits,1
	.align	2
.LC0:
	.ascii	"Hello\012\000"
	.section	.debug_frame,"",%progbits
.Lframe0:
	.4byte	.LECIE0-.LSCIE0
.LSCIE0:
	.4byte	0xffffffff
	.byte	0x1
	.ascii	"\000"
	.uleb128 0x1
	.sleb128 -4
	.byte	0xe
	.byte	0xc
	.uleb128 0xd
	.uleb128 0x0
	.align	2
.LECIE0:
.LSFDE0:
	.4byte	.LEFDE0-.LASFDE0
.LASFDE0:
	.4byte	.Lframe0
	.4byte	.LFB32
	.4byte	.LFE32-.LFB32
	.align	2
.LEFDE0:
.LSFDE2:
	.4byte	.LEFDE2-.LASFDE2
.LASFDE2:
	.4byte	.Lframe0
	.4byte	.LFB33
	.4byte	.LFE33-.LFB33
	.align	2
.LEFDE2:
.LSFDE4:
	.4byte	.LEFDE4-.LASFDE4
.LASFDE4:
	.4byte	.Lframe0
	.4byte	.LFB34
	.4byte	.LFE34-.LFB34
	.align	2
.LEFDE4:
.LSFDE6:
	.4byte	.LEFDE6-.LASFDE6
.LASFDE6:
	.4byte	.Lframe0
	.4byte	.LFB35
	.4byte	.LFE35-.LFB35
	.align	2
.LEFDE6:
.LSFDE8:
	.4byte	.LEFDE8-.LASFDE8
.LASFDE8:
	.4byte	.Lframe0
	.4byte	.LFB36
	.4byte	.LFE36-.LFB36
	.align	2
.LEFDE8:
.LSFDE10:
	.4byte	.LEFDE10-.LASFDE10
.LASFDE10:
	.4byte	.Lframe0
	.4byte	.LFB37
	.4byte	.LFE37-.LFB37
	.byte	0x4
	.4byte	.LCFI0-.LFB37
	.byte	0xe
	.uleb128 0x8
	.byte	0x8e
	.uleb128 0x1
	.byte	0x84
	.uleb128 0x2
	.align	2
.LEFDE10:
.LSFDE12:
	.4byte	.LEFDE12-.LASFDE12
.LASFDE12:
	.4byte	.Lframe0
	.4byte	.LFB31
	.4byte	.LFE31-.LFB31
	.byte	0x4
	.4byte	.LCFI1-.LFB31
	.byte	0xe
	.uleb128 0x8
	.byte	0x8e
	.uleb128 0x1
	.byte	0x84
	.uleb128 0x2
	.align	2
.LEFDE12:
	.text
.Letext0:
	.section	.debug_loc,"",%progbits
.Ldebug_loc0:
.LLST5:
	.4byte	.LFB37
	.4byte	.LCFI0
	.2byte	0x1
	.byte	0x5d
	.4byte	.LCFI0
	.4byte	.LFE37
	.2byte	0x2
	.byte	0x7d
	.sleb128 8
	.4byte	0x0
	.4byte	0x0
.LLST6:
	.4byte	.LFB31
	.4byte	.LCFI1
	.2byte	0x1
	.byte	0x5d
	.4byte	.LCFI1
	.4byte	.LFE31
	.2byte	0x2
	.byte	0x7d
	.sleb128 8
	.4byte	0x0
	.4byte	0x0
	.file 2 "CMSIS/a2fxxxm3.h"
	.file 3 "/usr/local/arm-toolchain/bin/../lib/gcc/arm-none-eabi/4.2.3/../../../../arm-none-eabi/include/stdint.h"
	.file 4 "CMSIS/core_cm3.h"
	.section	.debug_info
	.4byte	0x534
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.LASF79
	.byte	0x1
	.4byte	.LASF80
	.4byte	.LASF81
	.4byte	0x0
	.4byte	.Ldebug_line0
	.uleb128 0x2
	.byte	0x4
	.byte	0x5
	.ascii	"int\000"
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.4byte	.LASF0
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.4byte	.LASF1
	.uleb128 0x3
	.byte	0x1
	.byte	0x8
	.4byte	.LASF2
	.uleb128 0x3
	.byte	0x2
	.byte	0x5
	.4byte	.LASF3
	.uleb128 0x3
	.byte	0x2
	.byte	0x7
	.4byte	.LASF4
	.uleb128 0x3
	.byte	0x4
	.byte	0x5
	.4byte	.LASF5
	.uleb128 0x4
	.4byte	.LASF82
	.byte	0x3
	.byte	0x54
	.4byte	0x5d
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.4byte	.LASF6
	.uleb128 0x3
	.byte	0x8
	.byte	0x5
	.4byte	.LASF7
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.4byte	.LASF8
	.uleb128 0x5
	.byte	0x4
	.byte	0x7
	.uleb128 0x6
	.4byte	0x52
	.4byte	0x85
	.uleb128 0x7
	.4byte	0x72
	.byte	0x17
	.byte	0x0
	.uleb128 0x8
	.4byte	0x52
	.uleb128 0x9
	.4byte	0x85
	.uleb128 0xa
	.byte	0x58
	.byte	0x2
	.2byte	0x1e3
	.4byte	0x1e3
	.uleb128 0xb
	.4byte	.LASF9
	.byte	0x2
	.2byte	0x1e4
	.4byte	0x8a
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xb
	.4byte	.LASF10
	.byte	0x2
	.2byte	0x1e5
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0xb
	.4byte	.LASF11
	.byte	0x2
	.2byte	0x1e6
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0xb
	.4byte	.LASF12
	.byte	0x2
	.2byte	0x1e7
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0xb
	.4byte	.LASF13
	.byte	0x2
	.2byte	0x1e8
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0xb
	.4byte	.LASF14
	.byte	0x2
	.2byte	0x1e9
	.4byte	0x8a
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.uleb128 0xb
	.4byte	.LASF15
	.byte	0x2
	.2byte	0x1eb
	.4byte	0x8a
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0xb
	.4byte	.LASF16
	.byte	0x2
	.2byte	0x1ec
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x1c
	.uleb128 0xb
	.4byte	.LASF17
	.byte	0x2
	.2byte	0x1ed
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x20
	.uleb128 0xb
	.4byte	.LASF18
	.byte	0x2
	.2byte	0x1ee
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x24
	.uleb128 0xb
	.4byte	.LASF19
	.byte	0x2
	.2byte	0x1ef
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x28
	.uleb128 0xb
	.4byte	.LASF20
	.byte	0x2
	.2byte	0x1f0
	.4byte	0x8a
	.byte	0x2
	.byte	0x23
	.uleb128 0x2c
	.uleb128 0xb
	.4byte	.LASF21
	.byte	0x2
	.2byte	0x1f2
	.4byte	0x8a
	.byte	0x2
	.byte	0x23
	.uleb128 0x30
	.uleb128 0xb
	.4byte	.LASF22
	.byte	0x2
	.2byte	0x1f3
	.4byte	0x8a
	.byte	0x2
	.byte	0x23
	.uleb128 0x34
	.uleb128 0xb
	.4byte	.LASF23
	.byte	0x2
	.2byte	0x1f4
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x38
	.uleb128 0xb
	.4byte	.LASF24
	.byte	0x2
	.2byte	0x1f5
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x3c
	.uleb128 0xb
	.4byte	.LASF25
	.byte	0x2
	.2byte	0x1f6
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x40
	.uleb128 0xb
	.4byte	.LASF26
	.byte	0x2
	.2byte	0x1f7
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x44
	.uleb128 0xb
	.4byte	.LASF27
	.byte	0x2
	.2byte	0x1f8
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x48
	.uleb128 0xb
	.4byte	.LASF28
	.byte	0x2
	.2byte	0x1f9
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x4c
	.uleb128 0xb
	.4byte	.LASF29
	.byte	0x2
	.2byte	0x1fa
	.4byte	0x8a
	.byte	0x2
	.byte	0x23
	.uleb128 0x50
	.uleb128 0xb
	.4byte	.LASF30
	.byte	0x2
	.2byte	0x1fb
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x54
	.byte	0x0
	.uleb128 0xc
	.2byte	0x24c
	.byte	0x2
	.2byte	0x386
	.4byte	0x46e
	.uleb128 0xb
	.4byte	.LASF31
	.byte	0x2
	.2byte	0x387
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xb
	.4byte	.LASF32
	.byte	0x2
	.2byte	0x388
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0xb
	.4byte	.LASF33
	.byte	0x2
	.2byte	0x389
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0xb
	.4byte	.LASF34
	.byte	0x2
	.2byte	0x38a
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0xb
	.4byte	.LASF35
	.byte	0x2
	.2byte	0x38b
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0xb
	.4byte	.LASF36
	.byte	0x2
	.2byte	0x38c
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.uleb128 0xb
	.4byte	.LASF37
	.byte	0x2
	.2byte	0x38d
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0xb
	.4byte	.LASF38
	.byte	0x2
	.2byte	0x38e
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x1c
	.uleb128 0xb
	.4byte	.LASF39
	.byte	0x2
	.2byte	0x38f
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x20
	.uleb128 0xb
	.4byte	.LASF40
	.byte	0x2
	.2byte	0x390
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x24
	.uleb128 0xb
	.4byte	.LASF41
	.byte	0x2
	.2byte	0x391
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x28
	.uleb128 0xb
	.4byte	.LASF42
	.byte	0x2
	.2byte	0x392
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x2c
	.uleb128 0xb
	.4byte	.LASF43
	.byte	0x2
	.2byte	0x393
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x30
	.uleb128 0xb
	.4byte	.LASF44
	.byte	0x2
	.2byte	0x394
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x34
	.uleb128 0xb
	.4byte	.LASF45
	.byte	0x2
	.2byte	0x395
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x38
	.uleb128 0xb
	.4byte	.LASF46
	.byte	0x2
	.2byte	0x396
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x3c
	.uleb128 0xb
	.4byte	.LASF47
	.byte	0x2
	.2byte	0x397
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x40
	.uleb128 0xb
	.4byte	.LASF48
	.byte	0x2
	.2byte	0x398
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x44
	.uleb128 0xb
	.4byte	.LASF49
	.byte	0x2
	.2byte	0x399
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x48
	.uleb128 0xb
	.4byte	.LASF50
	.byte	0x2
	.2byte	0x39a
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x4c
	.uleb128 0xb
	.4byte	.LASF51
	.byte	0x2
	.2byte	0x39b
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x50
	.uleb128 0xb
	.4byte	.LASF52
	.byte	0x2
	.2byte	0x39c
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x54
	.uleb128 0xb
	.4byte	.LASF53
	.byte	0x2
	.2byte	0x39d
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x58
	.uleb128 0xb
	.4byte	.LASF54
	.byte	0x2
	.2byte	0x39e
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x5c
	.uleb128 0xb
	.4byte	.LASF55
	.byte	0x2
	.2byte	0x39f
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x60
	.uleb128 0xb
	.4byte	.LASF56
	.byte	0x2
	.2byte	0x3a0
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x64
	.uleb128 0xb
	.4byte	.LASF57
	.byte	0x2
	.2byte	0x3a1
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x68
	.uleb128 0xb
	.4byte	.LASF58
	.byte	0x2
	.2byte	0x3a2
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x6c
	.uleb128 0xb
	.4byte	.LASF59
	.byte	0x2
	.2byte	0x3a3
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x70
	.uleb128 0xb
	.4byte	.LASF60
	.byte	0x2
	.2byte	0x3a4
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x74
	.uleb128 0xb
	.4byte	.LASF61
	.byte	0x2
	.2byte	0x3a5
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x78
	.uleb128 0xb
	.4byte	.LASF62
	.byte	0x2
	.2byte	0x3a6
	.4byte	0x85
	.byte	0x2
	.byte	0x23
	.uleb128 0x7c
	.uleb128 0xb
	.4byte	.LASF63
	.byte	0x2
	.2byte	0x3a7
	.4byte	0x85
	.byte	0x3
	.byte	0x23
	.uleb128 0x80
	.uleb128 0xb
	.4byte	.LASF64
	.byte	0x2
	.2byte	0x3a8
	.4byte	0x85
	.byte	0x3
	.byte	0x23
	.uleb128 0x84
	.uleb128 0xb
	.4byte	.LASF65
	.byte	0x2
	.2byte	0x3a9
	.4byte	0x8a
	.byte	0x3
	.byte	0x23
	.uleb128 0x88
	.uleb128 0xb
	.4byte	.LASF66
	.byte	0x2
	.2byte	0x3aa
	.4byte	0x8a
	.byte	0x3
	.byte	0x23
	.uleb128 0x8c
	.uleb128 0xb
	.4byte	.LASF67
	.byte	0x2
	.2byte	0x3ab
	.4byte	0x85
	.byte	0x3
	.byte	0x23
	.uleb128 0x90
	.uleb128 0xb
	.4byte	.LASF68
	.byte	0x2
	.2byte	0x3ac
	.4byte	0x8a
	.byte	0x3
	.byte	0x23
	.uleb128 0x94
	.uleb128 0xb
	.4byte	.LASF69
	.byte	0x2
	.2byte	0x3ad
	.4byte	0x8a
	.byte	0x3
	.byte	0x23
	.uleb128 0x98
	.uleb128 0xb
	.4byte	.LASF70
	.byte	0x2
	.2byte	0x3ae
	.4byte	0x85
	.byte	0x3
	.byte	0x23
	.uleb128 0x9c
	.uleb128 0xb
	.4byte	.LASF71
	.byte	0x2
	.2byte	0x3af
	.4byte	0x75
	.byte	0x3
	.byte	0x23
	.uleb128 0xa0
	.uleb128 0xb
	.4byte	.LASF72
	.byte	0x2
	.2byte	0x3b0
	.4byte	0x47e
	.byte	0x3
	.byte	0x23
	.uleb128 0x100
	.byte	0x0
	.uleb128 0x6
	.4byte	0x52
	.4byte	0x47e
	.uleb128 0x7
	.4byte	0x72
	.byte	0x52
	.byte	0x0
	.uleb128 0x8
	.4byte	0x46e
	.uleb128 0xd
	.byte	0x1
	.4byte	.LASF73
	.byte	0x1
	.byte	0x1f
	.byte	0x1
	.4byte	0x21
	.4byte	.LFB32
	.4byte	.LFE32
	.byte	0x1
	.byte	0x5d
	.uleb128 0xd
	.byte	0x1
	.4byte	.LASF74
	.byte	0x1
	.byte	0x24
	.byte	0x1
	.4byte	0x21
	.4byte	.LFB33
	.4byte	.LFE33
	.byte	0x1
	.byte	0x5d
	.uleb128 0xd
	.byte	0x1
	.4byte	.LASF75
	.byte	0x1
	.byte	0x29
	.byte	0x1
	.4byte	0x21
	.4byte	.LFB34
	.4byte	.LFE34
	.byte	0x1
	.byte	0x5d
	.uleb128 0xd
	.byte	0x1
	.4byte	.LASF76
	.byte	0x1
	.byte	0x2e
	.byte	0x1
	.4byte	0x21
	.4byte	.LFB35
	.4byte	.LFE35
	.byte	0x1
	.byte	0x5d
	.uleb128 0xd
	.byte	0x1
	.4byte	.LASF77
	.byte	0x1
	.byte	0x33
	.byte	0x1
	.4byte	0x21
	.4byte	.LFB36
	.4byte	.LFE36
	.byte	0x1
	.byte	0x5d
	.uleb128 0xe
	.byte	0x1
	.4byte	.LASF83
	.byte	0x1
	.byte	0x3a
	.byte	0x1
	.4byte	.LFB37
	.4byte	.LFE37
	.4byte	.LLST5
	.uleb128 0xf
	.byte	0x1
	.4byte	.LASF78
	.byte	0x1
	.byte	0x6
	.byte	0x1
	.4byte	0x21
	.4byte	.LFB31
	.4byte	.LFE31
	.4byte	.LLST6
	.uleb128 0x10
	.4byte	.LASF84
	.byte	0x4
	.2byte	0x6ce
	.4byte	0x532
	.byte	0x1
	.byte	0x1
	.uleb128 0x8
	.4byte	0x21
	.byte	0x0
	.section	.debug_abbrev
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x52
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0x0
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0x0
	.byte	0x0
	.uleb128 0x4
	.uleb128 0x16
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x5
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x6
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x7
	.uleb128 0x21
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x8
	.uleb128 0x35
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x9
	.uleb128 0x26
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xa
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xd
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0xc
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0x5
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xd
	.uleb128 0x2e
	.byte	0x0
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0xe
	.uleb128 0x2e
	.byte	0x0
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0xf
	.uleb128 0x2e
	.byte	0x0
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x10
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.section	.debug_pubnames,"",%progbits
	.4byte	0x7b
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x538
	.4byte	0x483
	.ascii	"timer_init\000"
	.4byte	0x49a
	.ascii	"dram_init\000"
	.4byte	0x4b1
	.ascii	"print_cpuinfo\000"
	.4byte	0x4c8
	.ascii	"checkboard\000"
	.4byte	0x4df
	.ascii	"flash_init\000"
	.4byte	0x4f6
	.ascii	"test_func\000"
	.4byte	0x50b
	.ascii	"arch_cpu_init\000"
	.4byte	0x0
	.section	.debug_aranges,"",%progbits
	.4byte	0x24
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0x0
	.2byte	0x0
	.2byte	0x0
	.4byte	.Ltext0
	.4byte	.Letext0-.Ltext0
	.4byte	.LFB37
	.4byte	.LFE37-.LFB37
	.4byte	0x0
	.4byte	0x0
	.section	.debug_str,"MS",%progbits,1
.LASF49:
	.ascii	"MSS_CLK_CR\000"
.LASF45:
	.ascii	"SYSTICK_CR\000"
.LASF25:
	.ascii	"TIM64_BGLOADVAL_U\000"
.LASF30:
	.ascii	"TIM64_MODE\000"
.LASF41:
	.ascii	"IAP_CR\000"
.LASF43:
	.ascii	"SOFT_RST_CR\000"
.LASF3:
	.ascii	"short int\000"
.LASF54:
	.ascii	"MSS_CCC_SR\000"
.LASF22:
	.ascii	"TIM64_VAL_L\000"
.LASF35:
	.ascii	"FAB_PROT_SIZE_CR\000"
.LASF59:
	.ascii	"FAB_APB_HIWORD_DR\000"
.LASF79:
	.ascii	"GNU C 4.2.3\000"
.LASF39:
	.ascii	"CLR_MSS_SR\000"
.LASF20:
	.ascii	"TIM2_MIS\000"
.LASF13:
	.ascii	"TIM1_RIS\000"
.LASF82:
	.ascii	"uint32_t\000"
.LASF10:
	.ascii	"TIM1_LOADVAL\000"
.LASF65:
	.ascii	"RED_REP_LOW_LOCS0\000"
.LASF68:
	.ascii	"RED_REP_LOW_LOCS1\000"
.LASF33:
	.ascii	"ENVM_REMAP_SYS_CR\000"
.LASF44:
	.ascii	"DEVICE_SR\000"
.LASF16:
	.ascii	"TIM2_LOADVAL\000"
.LASF28:
	.ascii	"TIM64_RIS\000"
.LASF7:
	.ascii	"long long int\000"
.LASF8:
	.ascii	"long long unsigned int\000"
.LASF55:
	.ascii	"MSS_RCOSC_CR\000"
.LASF73:
	.ascii	"timer_init\000"
.LASF5:
	.ascii	"long int\000"
.LASF77:
	.ascii	"flash_init\000"
.LASF60:
	.ascii	"LOOPBACK_CR\000"
.LASF51:
	.ascii	"MSS_CCC_MUX_CR\000"
.LASF12:
	.ascii	"TIM1_CTRL\000"
.LASF70:
	.ascii	"FABRIC_CR\000"
.LASF42:
	.ascii	"SOFT_IRQ_CR\000"
.LASF56:
	.ascii	"VRPSM_CR\000"
.LASF75:
	.ascii	"print_cpuinfo\000"
.LASF2:
	.ascii	"unsigned char\000"
.LASF36:
	.ascii	"FAB_PROT_BASE_CR\000"
.LASF62:
	.ascii	"GPIN_SOURCE_CR\000"
.LASF1:
	.ascii	"signed char\000"
.LASF64:
	.ascii	"RED_REP_ADDR0\000"
.LASF67:
	.ascii	"RED_REP_ADDR1\000"
.LASF74:
	.ascii	"dram_init\000"
.LASF0:
	.ascii	"unsigned int\000"
.LASF53:
	.ascii	"MSS_CCC_DLY_CR\000"
.LASF83:
	.ascii	"test_func\000"
.LASF37:
	.ascii	"AHB_MATRIX_CR\000"
.LASF46:
	.ascii	"EMC_MUX_CR\000"
.LASF58:
	.ascii	"FAB_IF_CR\000"
.LASF81:
	.ascii	"/home/vlad/uboot/u-boot/cpu/arm_cortexm3\000"
.LASF52:
	.ascii	"MSS_CCC_PLL_CR\000"
.LASF19:
	.ascii	"TIM2_RIS\000"
.LASF4:
	.ascii	"short unsigned int\000"
.LASF9:
	.ascii	"TIM1_VAL\000"
.LASF76:
	.ascii	"checkboard\000"
.LASF27:
	.ascii	"TIM64_CTRL\000"
.LASF24:
	.ascii	"TIM64_LOADVAL_L\000"
.LASF40:
	.ascii	"EFROM_CR\000"
.LASF66:
	.ascii	"RED_REP_HIGH_LOCS0\000"
.LASF69:
	.ascii	"RED_REP_HIGH_LOCS1\000"
.LASF50:
	.ascii	"MSS_CCC_DIV_CR\000"
.LASF71:
	.ascii	"RESERVED1\000"
.LASF6:
	.ascii	"long unsigned int\000"
.LASF32:
	.ascii	"ENVM_CR\000"
.LASF84:
	.ascii	"ITM_RxBuffer\000"
.LASF63:
	.ascii	"TEST_SR\000"
.LASF47:
	.ascii	"EMC_CS_0_CR\000"
.LASF78:
	.ascii	"arch_cpu_init\000"
.LASF23:
	.ascii	"TIM64_LOADVAL_U\000"
.LASF31:
	.ascii	"ESRAM_CR\000"
.LASF34:
	.ascii	"ENVM_REMAP_FAB_CR\000"
.LASF14:
	.ascii	"TIM1_MIS\000"
.LASF21:
	.ascii	"TIM64_VAL_U\000"
.LASF61:
	.ascii	"MSS_IO_BANK_CR\000"
.LASF29:
	.ascii	"TIM64_MIS\000"
.LASF17:
	.ascii	"TIM2_BGLOADVAL\000"
.LASF38:
	.ascii	"MSS_SR\000"
.LASF57:
	.ascii	"RESERVED\000"
.LASF80:
	.ascii	"cpu.c\000"
.LASF72:
	.ascii	"IOMUX_CR\000"
.LASF18:
	.ascii	"TIM2_CTRL\000"
.LASF15:
	.ascii	"TIM2_VAL\000"
.LASF48:
	.ascii	"EMC_CS_1_CR\000"
.LASF26:
	.ascii	"TIM64_BGLOADVAL_L\000"
.LASF11:
	.ascii	"TIM1_BGLOADVAL\000"
	.ident	"GCC: (Sourcery G++ Lite 2008q1-126) 4.2.3"
