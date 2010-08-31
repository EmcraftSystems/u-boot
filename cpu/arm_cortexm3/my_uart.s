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
	.file	"my_uart.c"
	.section	.debug_abbrev,"",%progbits
.Ldebug_abbrev0:
	.section	.debug_info,"",%progbits
.Ldebug_info0:
	.section	.debug_line,"",%progbits
.Ldebug_line0:
	.text
.Ltext0:
	.align	2
	.global	my_uart_init
	.thumb
	.thumb_func
	.type	my_uart_init, %function
my_uart_init:
.LFB31:
	.file 1 "my_uart.c"
	.loc 1 11 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
.LVL0:
	push	{r4}
.LCFI0:
	.loc 1 15 0
	mvn	r3, #48
	mov	r4, #1073741824
	strb	r3, [r4, #8]
	.loc 1 19 0
	movs	r2, #3
	.loc 1 21 0
	adds	r3, r3, #49
	.loc 1 19 0
	strb	r2, [r4, #12]
	.loc 1 21 0
	strb	r3, [r4, #16]
	.loc 1 23 0
	strb	r3, [r4, #4]
	.loc 1 26 0
	movw	r3, #:lower16:g_FrequencyPCLK0
	movt	r3, #:upper16:g_FrequencyPCLK0
	ldr	r2, [r3, #0]
	add	r1, r0, r0, lsr #31
	lsrs	r2, r2, #4
	add	r2, r2, r1, asr #1
	.loc 1 28 0
	ldrb	r3, [r4, #12]	@ zero_extendqisi2
	.loc 1 26 0
	udiv	r2, r2, r0
	uxth	r2, r2
.LVL1:
	.loc 1 28 0
	orr	r3, r3, #128
	.loc 1 30 0
	uxtb	r1, r2
	.loc 1 31 0
	lsrs	r2, r2, #8
.LVL2:
	.loc 1 28 0
	strb	r3, [r4, #12]
	.loc 1 30 0
	strb	r1, [r4, #0]
	.loc 1 31 0
	strb	r2, [r4, #4]
	.loc 1 33 0
	ldrb	r3, [r4, #12]	@ zero_extendqisi2
	.loc 1 11 0
	@ lr needed for prologue
	.loc 1 33 0
	and	r3, r3, #127
	strb	r3, [r4, #12]
	.loc 1 34 0
	pop	{r4}
	bx	lr
.LFE31:
	.size	my_uart_init, .-my_uart_init
	.align	2
	.global	my_uart_putc
	.thumb
	.thumb_func
	.type	my_uart_putc, %function
my_uart_putc:
.LFB32:
	.loc 1 37 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
.LVL3:
	push	{r4, r5, r6, lr}
.LCFI1:
	mov	r5, r0
.LVL4:
.L5:
	.loc 1 37 0
	movw	r4, #999
	b	.L6
.L14:
	.loc 1 45 0
	movs	r0, #5
	bl	my_udelay
	.loc 1 41 0
	cbz	r4, .L7
	subs	r4, r4, #1
.LVL5:
.L6:
	.loc 1 42 0
	mov	r3, #1073741824
	ldrb	r2, [r3, #20]	@ zero_extendqisi2
	tst	r2, #32
	beq	.L14
.L7:
	.loc 1 48 0
	uxtb	r2, r5
	mov	r3, #1073741824
	.loc 1 50 0
	cmp	r5, #10
	.loc 1 48 0
	strb	r2, [r3, #0]
	.loc 1 50 0
	bne	.L12
	adds	r5, r5, #3
	b	.L5
.LVL6:
.L12:
	.loc 1 53 0
	pop	{r4, r5, r6, pc}
.LFE32:
	.size	my_uart_putc, .-my_uart_putc
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
	.4byte	.LFB31
	.4byte	.LFE31-.LFB31
	.byte	0x4
	.4byte	.LCFI0-.LFB31
	.byte	0xe
	.uleb128 0x4
	.byte	0x84
	.uleb128 0x1
	.align	2
.LEFDE0:
.LSFDE2:
	.4byte	.LEFDE2-.LASFDE2
.LASFDE2:
	.4byte	.Lframe0
	.4byte	.LFB32
	.4byte	.LFE32-.LFB32
	.byte	0x4
	.4byte	.LCFI1-.LFB32
	.byte	0xe
	.uleb128 0x10
	.byte	0x8e
	.uleb128 0x1
	.byte	0x86
	.uleb128 0x2
	.byte	0x85
	.uleb128 0x3
	.byte	0x84
	.uleb128 0x4
	.align	2
.LEFDE2:
	.text
.Letext0:
	.section	.debug_loc,"",%progbits
.Ldebug_loc0:
.LLST0:
	.4byte	.LFB31-.Ltext0
	.4byte	.LCFI0-.Ltext0
	.2byte	0x1
	.byte	0x5d
	.4byte	.LCFI0-.Ltext0
	.4byte	.LFE31-.Ltext0
	.2byte	0x2
	.byte	0x7d
	.sleb128 4
	.4byte	0x0
	.4byte	0x0
.LLST1:
	.4byte	.LVL1-.Ltext0
	.4byte	.LVL2-.Ltext0
	.2byte	0x1
	.byte	0x52
	.4byte	0x0
	.4byte	0x0
.LLST2:
	.4byte	.LFB32-.Ltext0
	.4byte	.LCFI1-.Ltext0
	.2byte	0x1
	.byte	0x5d
	.4byte	.LCFI1-.Ltext0
	.4byte	.LFE32-.Ltext0
	.2byte	0x2
	.byte	0x7d
	.sleb128 16
	.4byte	0x0
	.4byte	0x0
.LLST3:
	.4byte	.LVL3-.Ltext0
	.4byte	.LVL4-.Ltext0
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL4-.Ltext0
	.4byte	.LFE32-.Ltext0
	.2byte	0x1
	.byte	0x55
	.4byte	0x0
	.4byte	0x0
	.file 2 "/usr/local/arm-toolchain/bin/../lib/gcc/arm-none-eabi/4.2.3/../../../../arm-none-eabi/include/stdint.h"
	.file 3 "CMSIS/a2fxxxm3.h"
	.file 4 "CMSIS/core_cm3.h"
	.file 5 "CMSIS/system_a2fxxxm3.h"
	.section	.debug_info
	.4byte	0x2a2
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.LASF33
	.byte	0x1
	.4byte	.LASF34
	.4byte	.LASF35
	.4byte	.Ltext0
	.4byte	.Letext0
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
	.uleb128 0x4
	.4byte	.LASF4
	.byte	0x2
	.byte	0x2e
	.4byte	0x45
	.uleb128 0x3
	.byte	0x1
	.byte	0x8
	.4byte	.LASF2
	.uleb128 0x3
	.byte	0x2
	.byte	0x5
	.4byte	.LASF3
	.uleb128 0x4
	.4byte	.LASF5
	.byte	0x2
	.byte	0x3a
	.4byte	0x5e
	.uleb128 0x3
	.byte	0x2
	.byte	0x7
	.4byte	.LASF6
	.uleb128 0x3
	.byte	0x4
	.byte	0x5
	.4byte	.LASF7
	.uleb128 0x4
	.4byte	.LASF8
	.byte	0x2
	.byte	0x54
	.4byte	0x77
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.4byte	.LASF9
	.uleb128 0x3
	.byte	0x8
	.byte	0x5
	.4byte	.LASF10
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.4byte	.LASF11
	.uleb128 0x5
	.byte	0x4
	.byte	0x7
	.uleb128 0x6
	.4byte	0x3a
	.uleb128 0x7
	.byte	0x4
	.byte	0x3
	.byte	0xd7
	.4byte	0xc9
	.uleb128 0x8
	.ascii	"RBR\000"
	.byte	0x3
	.byte	0xd8
	.4byte	0xc9
	.uleb128 0x8
	.ascii	"THR\000"
	.byte	0x3
	.byte	0xd9
	.4byte	0x8f
	.uleb128 0x8
	.ascii	"DLR\000"
	.byte	0x3
	.byte	0xda
	.4byte	0x8f
	.uleb128 0x9
	.4byte	.LASF12
	.byte	0x3
	.byte	0xdb
	.4byte	0x6c
	.byte	0x0
	.uleb128 0xa
	.4byte	0x8f
	.uleb128 0x7
	.byte	0x4
	.byte	0x3
	.byte	0xdf
	.4byte	0xf8
	.uleb128 0x8
	.ascii	"DMR\000"
	.byte	0x3
	.byte	0xe0
	.4byte	0x8f
	.uleb128 0x8
	.ascii	"IER\000"
	.byte	0x3
	.byte	0xe1
	.4byte	0x8f
	.uleb128 0x9
	.4byte	.LASF13
	.byte	0x3
	.byte	0xe2
	.4byte	0x6c
	.byte	0x0
	.uleb128 0x7
	.byte	0x4
	.byte	0x3
	.byte	0xe6
	.4byte	0x122
	.uleb128 0x8
	.ascii	"IIR\000"
	.byte	0x3
	.byte	0xe7
	.4byte	0x8f
	.uleb128 0x8
	.ascii	"FCR\000"
	.byte	0x3
	.byte	0xe8
	.4byte	0x8f
	.uleb128 0x9
	.4byte	.LASF14
	.byte	0x3
	.byte	0xe9
	.4byte	0x6c
	.byte	0x0
	.uleb128 0xb
	.byte	0x20
	.byte	0x3
	.byte	0xd5
	.4byte	0x214
	.uleb128 0xc
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xc
	.4byte	0xce
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0xc
	.4byte	0xf8
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0xd
	.ascii	"LCR\000"
	.byte	0x3
	.byte	0xec
	.4byte	0x8f
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0xe
	.4byte	.LASF15
	.byte	0x3
	.byte	0xed
	.4byte	0x3a
	.byte	0x2
	.byte	0x23
	.uleb128 0xd
	.uleb128 0xe
	.4byte	.LASF16
	.byte	0x3
	.byte	0xee
	.4byte	0x53
	.byte	0x2
	.byte	0x23
	.uleb128 0xe
	.uleb128 0xd
	.ascii	"MCR\000"
	.byte	0x3
	.byte	0xef
	.4byte	0x8f
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0xe
	.4byte	.LASF17
	.byte	0x3
	.byte	0xf0
	.4byte	0x3a
	.byte	0x2
	.byte	0x23
	.uleb128 0x11
	.uleb128 0xe
	.4byte	.LASF18
	.byte	0x3
	.byte	0xf1
	.4byte	0x53
	.byte	0x2
	.byte	0x23
	.uleb128 0x12
	.uleb128 0xd
	.ascii	"LSR\000"
	.byte	0x3
	.byte	0xf2
	.4byte	0xc9
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.uleb128 0xe
	.4byte	.LASF19
	.byte	0x3
	.byte	0xf3
	.4byte	0x3a
	.byte	0x2
	.byte	0x23
	.uleb128 0x15
	.uleb128 0xe
	.4byte	.LASF20
	.byte	0x3
	.byte	0xf4
	.4byte	0x53
	.byte	0x2
	.byte	0x23
	.uleb128 0x16
	.uleb128 0xd
	.ascii	"MSR\000"
	.byte	0x3
	.byte	0xf5
	.4byte	0xc9
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0xe
	.4byte	.LASF21
	.byte	0x3
	.byte	0xf6
	.4byte	0x3a
	.byte	0x2
	.byte	0x23
	.uleb128 0x19
	.uleb128 0xe
	.4byte	.LASF22
	.byte	0x3
	.byte	0xf7
	.4byte	0x53
	.byte	0x2
	.byte	0x23
	.uleb128 0x1a
	.uleb128 0xd
	.ascii	"SR\000"
	.byte	0x3
	.byte	0xf8
	.4byte	0x8f
	.byte	0x2
	.byte	0x23
	.uleb128 0x1c
	.uleb128 0xe
	.4byte	.LASF23
	.byte	0x3
	.byte	0xf9
	.4byte	0x3a
	.byte	0x2
	.byte	0x23
	.uleb128 0x1d
	.uleb128 0xe
	.4byte	.LASF24
	.byte	0x3
	.byte	0xfa
	.4byte	0x53
	.byte	0x2
	.byte	0x23
	.uleb128 0x1e
	.byte	0x0
	.uleb128 0xf
	.byte	0x1
	.4byte	.LASF25
	.byte	0x1
	.byte	0xb
	.byte	0x1
	.4byte	.LFB31
	.4byte	.LFE31
	.4byte	.LLST0
	.4byte	0x24a
	.uleb128 0x10
	.4byte	.LASF27
	.byte	0x1
	.byte	0xa
	.4byte	0x25
	.byte	0x1
	.byte	0x50
	.uleb128 0x11
	.4byte	.LASF28
	.byte	0x1
	.byte	0xc
	.4byte	0x5e
	.4byte	.LLST1
	.byte	0x0
	.uleb128 0xf
	.byte	0x1
	.4byte	.LASF26
	.byte	0x1
	.byte	0x25
	.byte	0x1
	.4byte	.LFB32
	.4byte	.LFE32
	.4byte	.LLST2
	.4byte	0x27e
	.uleb128 0x12
	.ascii	"c\000"
	.byte	0x1
	.byte	0x24
	.4byte	0x27e
	.4byte	.LLST3
	.uleb128 0x13
	.4byte	.LASF29
	.byte	0x1
	.byte	0x26
	.4byte	0x25
	.byte	0x1
	.byte	0x54
	.byte	0x0
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.4byte	.LASF30
	.uleb128 0x14
	.4byte	.LASF31
	.byte	0x4
	.2byte	0x6d0
	.4byte	0x293
	.byte	0x1
	.byte	0x1
	.uleb128 0x6
	.4byte	0x25
	.uleb128 0x15
	.4byte	.LASF32
	.byte	0x5
	.byte	0x16
	.4byte	0x6c
	.byte	0x1
	.byte	0x1
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
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
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
	.uleb128 0x35
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x7
	.uleb128 0x17
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x8
	.uleb128 0xd
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x9
	.uleb128 0xd
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
	.uleb128 0xa
	.uleb128 0x26
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xb
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xc
	.uleb128 0xd
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0xd
	.uleb128 0xd
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0xe
	.uleb128 0xd
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0xf
	.uleb128 0x2e
	.byte	0x1
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
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x10
	.uleb128 0x5
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x11
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x12
	.uleb128 0x5
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x13
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x14
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
	.uleb128 0x15
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
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
	.4byte	0x30
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x2a6
	.4byte	0x214
	.ascii	"my_uart_init\000"
	.4byte	0x24a
	.ascii	"my_uart_putc\000"
	.4byte	0x0
	.section	.debug_aranges,"",%progbits
	.4byte	0x1c
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0x0
	.2byte	0x0
	.2byte	0x0
	.4byte	.Ltext0
	.4byte	.Letext0-.Ltext0
	.4byte	0x0
	.4byte	0x0
	.section	.debug_str,"MS",%progbits,1
.LASF16:
	.ascii	"RESERVED4\000"
.LASF28:
	.ascii	"baud_val\000"
.LASF33:
	.ascii	"GNU C 4.2.3\000"
.LASF12:
	.ascii	"RESERVED0\000"
.LASF13:
	.ascii	"RESERVED1\000"
.LASF14:
	.ascii	"RESERVED2\000"
.LASF26:
	.ascii	"my_uart_putc\000"
.LASF17:
	.ascii	"RESERVED5\000"
.LASF18:
	.ascii	"RESERVED6\000"
.LASF19:
	.ascii	"RESERVED7\000"
.LASF20:
	.ascii	"RESERVED8\000"
.LASF21:
	.ascii	"RESERVED9\000"
.LASF10:
	.ascii	"long long int\000"
.LASF2:
	.ascii	"unsigned char\000"
.LASF35:
	.ascii	"/home/vlad/uboot/u-boot/cpu/arm_cortexm3\000"
.LASF9:
	.ascii	"long unsigned int\000"
.LASF6:
	.ascii	"short unsigned int\000"
.LASF32:
	.ascii	"g_FrequencyPCLK0\000"
.LASF31:
	.ascii	"ITM_RxBuffer\000"
.LASF29:
	.ascii	"wait\000"
.LASF34:
	.ascii	"my_uart.c\000"
.LASF27:
	.ascii	"baud_rate\000"
.LASF0:
	.ascii	"unsigned int\000"
.LASF30:
	.ascii	"char\000"
.LASF4:
	.ascii	"uint8_t\000"
.LASF25:
	.ascii	"my_uart_init\000"
.LASF22:
	.ascii	"RESERVED10\000"
.LASF23:
	.ascii	"RESERVED11\000"
.LASF24:
	.ascii	"RESERVED12\000"
.LASF3:
	.ascii	"short int\000"
.LASF5:
	.ascii	"uint16_t\000"
.LASF8:
	.ascii	"uint32_t\000"
.LASF7:
	.ascii	"long int\000"
.LASF1:
	.ascii	"signed char\000"
.LASF11:
	.ascii	"long long unsigned int\000"
.LASF15:
	.ascii	"RESERVED3\000"
	.ident	"GCC: (Sourcery G++ Lite 2008q1-126) 4.2.3"
