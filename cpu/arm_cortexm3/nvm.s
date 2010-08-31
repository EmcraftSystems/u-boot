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
	.file	"nvm.c"
	.section	.debug_abbrev,"",%progbits
.Ldebug_abbrev0:
	.section	.debug_info,"",%progbits
.Ldebug_info0:
	.section	.debug_line,"",%progbits
.Ldebug_line0:
	.text
.Ltext0:
	.section	.ramcode,"ax",%progbits
	.align	2
	.thumb
	.thumb_func
	.type	my_putc, %function
my_putc:
.LFB31:
	.file 1 "nvm.c"
	.loc 1 39 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
.LVL0:
	.loc 1 47 0
	ldr	r1, .L13
	movw	r3, #:lower16:1125899907
	ldr	r2, [r1, #0]
	movt	r3, #:upper16:1125899907
	umull	r1, r2, r3, r2
	.loc 1 39 0
	push	{lr}
.LCFI0:
	.loc 1 47 0
	lsrs	r2, r2, #18
	lsls	r3, r2, #2
	.loc 1 39 0
	mov	lr, r0
	.loc 1 47 0
	adds	r0, r3, r2
.LVL1:
	.loc 1 44 0
	mov	r3, #1073741824
	ldrb	r2, [r3, #20]	@ zero_extendqisi2
	.loc 1 47 0
	movw	r1, #999
.LVL2:
	.loc 1 44 0
	tst	r2, #32
	bne	.L3
.L12:
	.loc 1 47 0
	movw	r3, #:lower16:1073762304
	movt	r3, #:upper16:1073762304
	str	r0, [r3, #4]
.L5:
	.loc 1 48 0
	movw	r3, #:lower16:1073762304
	movt	r3, #:upper16:1073762304
	ldr	r2, [r3, #0]
	cmp	r2, #0
	bne	.L5
	.loc 1 43 0
	cbz	r1, .L3
	.loc 1 44 0
	mov	r3, #1073741824
	ldrb	r2, [r3, #20]	@ zero_extendqisi2
	.loc 1 43 0
	subs	r1, r1, #1
	.loc 1 44 0
	tst	r2, #32
	beq	.L12
.L3:
	.loc 1 51 0
	uxtb	r2, lr
	mov	r3, #1073741824
	strb	r2, [r3, #0]
	.loc 1 52 0
	pop	{pc}
.L14:
	.align	2
.L13:
	.word	g_FrequencyPCLK0
.LFE31:
	.size	my_putc, .-my_putc
	.align	2
	.thumb
	.thumb_func
	.type	mss_nvm_exec_cmd, %function
mss_nvm_exec_cmd:
.LFB32:
	.loc 1 56 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
.LVL3:
	.loc 1 66 0
	movw	r3, #:lower16:1048448
	movt	r3, #:upper16:1048448
	and	r3, r0, r3
	.loc 1 56 0
	push	{r4, r5, r6, lr}
.LCFI1:
	.loc 1 63 0
	movw	r4, #:lower16:1611661312
	.loc 1 66 0
	orrs	r1, r1, r3
.LVL4:
	.loc 1 63 0
	movt	r4, #:upper16:1611661312
	mov	r2, #-1
	str	r2, [r4, #0]
	.loc 1 66 0
	str	r1, [r4, #4]
	.loc 1 79 0
	ldr	r1, .L34
	movw	r3, #:lower16:1125899907
	ldr	r2, [r1, #0]
	movt	r3, #:upper16:1125899907
	umull	r1, r2, r3, r2
	lsrs	r2, r2, #18
	lsls	r3, r2, #2
	adds	r5, r3, r2
	movs	r0, #0
.LVL5:
.L16:
	.loc 1 74 0
	movw	r3, #:lower16:1611661312
	movt	r3, #:upper16:1611661312
	ldr	r1, [r3, #0]
	.loc 1 75 0
	ands	r4, r1, #65537
	beq	.L17
	.loc 1 79 0
	movw	r3, #:lower16:1073762304
	movt	r3, #:upper16:1073762304
	str	r5, [r3, #4]
.L19:
	.loc 1 80 0
	movw	r3, #:lower16:1073762304
	movt	r3, #:upper16:1073762304
	ldr	r2, [r3, #0]
	cmp	r2, #0
	bne	.L19
	.loc 1 73 0
	adds	r0, r0, #1
	cmp	r0, #1000
	bne	.L16
	ldr	r3, .L34+4
	str	r1, [r3, #0]
	.loc 1 84 0
	cbz	r4, .L25
	.loc 1 85 0
	movs	r0, #97
.LVL6:
	bl	my_putc
	mov	r0, #-1
	b	.L24
.LVL7:
.L17:
	.loc 1 98 0
	ldr	r3, .L34+4
	str	r1, [r3, #0]
.L25:
	.loc 1 97 0
	movw	r0, #:lower16:50332416
.LVL8:
	movt	r0, #:upper16:50332416
	and	r0, r1, r0
	cbnz	r0, .L33
.L24:
	.loc 1 111 0
	pop	{r4, r5, r6, pc}
.L33:
	.loc 1 98 0
	movs	r0, #98
	bl	my_putc
	mov	r0, #-1
	b	.L24
.L35:
	.align	2
.L34:
	.word	g_FrequencyPCLK0
	.word	.LANCHOR0
.LFE32:
	.size	mss_nvm_exec_cmd, .-mss_nvm_exec_cmd
	.align	2
	.global	nvm_write
	.thumb
	.thumb_func
	.type	nvm_write, %function
nvm_write:
.LFB33:
	.loc 1 121 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
.LVL9:
	.loc 1 127 0
	cmp	r0, #262144
	.loc 1 121 0
	push	{r4, r5, r6, r7, r8, r9, sl, lr}
.LCFI2:
	.loc 1 121 0
	mov	r9, r1
	mov	r7, r2
	.loc 1 127 0
	bhi	.L58
.LVL10:
	.loc 1 137 0
	cmp	r2, #0
	beq	.L59
.LVL11:
	.loc 1 122 0
	mov	r8, #0
.LVL12:
	add	r4, r0, #1610612736
.LVL13:
	mov	r6, r8
	b	.L42
.LVL14:
.L60:
	.loc 1 150 0
	adds	r3, r5, #1
	uxth	r3, r3
.LVL15:
	.loc 1 152 0
	adds	r6, r6, #1
	.loc 1 137 0
	cmp	r7, r3
	bls	.L57
.LVL16:
.L42:
	.loc 1 122 0
	uxth	r5, r6
	.loc 1 138 0
	cbz	r5, .L43
	tst	r4, #127
	bne	.L45
.L43:
	.loc 1 140 0
	mov	r0, r4
.LVL17:
	mov	r1, #33554432
	bl	mss_nvm_exec_cmd
	.loc 1 141 0
	cmp	r0, #0
	blt	.L57
.LVL18:
.L45:
	.loc 1 147 0
	ldrb	r3, [r6, r9]	@ zero_extendqisi2
	.loc 1 150 0
	subs	r2, r7, #1
	cmp	r6, r2
	.loc 1 147 0
	strb	r3, [r4], #1
	.loc 1 150 0
	beq	.L47
	tst	r4, #127
	bne	.L60
.L47:
	.loc 1 151 0
	subs	r0, r4, #1
.LVL19:
	mov	r1, #268435456
	bl	mss_nvm_exec_cmd
	.loc 1 152 0
	cmp	r0, #0
	blt	.L57
	adds	r3, r5, #1
	uxth	r8, r3
	mov	r3, r8
	adds	r6, r6, #1
	.loc 1 137 0
	cmp	r7, r3
	bhi	.L42
.LVL20:
.L57:
	mov	r0, r8
.LVL21:
.L39:
	.loc 1 160 0
	pop	{r4, r5, r6, r7, r8, r9, sl, pc}
.LVL22:
.L58:
	.loc 1 127 0
	movs	r0, #0
.LVL23:
	b	.L39
.LVL24:
.L59:
	.loc 1 137 0
	mov	r0, r2
.LVL25:
	b	.L39
.LFE33:
	.size	nvm_write, .-nvm_write
	.text
	.align	2
	.global	nvm_init
	.thumb
	.thumb_func
	.type	nvm_init, %function
nvm_init:
.LFB35:
	.loc 1 172 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	.loc 1 174 0
	movw	r3, #:lower16:1611661312
	movt	r3, #:upper16:1611661312
	movs	r2, #0
	str	r2, [r3, #8]
	.loc 1 172 0
	@ lr needed for prologue
	.loc 1 177 0
	str	r2, [r3, #16]
	.loc 1 178 0
	str	r2, [r3, #20]
	.loc 1 181 0
	str	r2, [r3, #36]
	.loc 1 184 0
	str	r2, [r3, #0]
	.loc 1 185 0
	bx	lr
.LFE35:
	.size	nvm_init, .-nvm_init
	.align	2
	.global	nvm_read
	.thumb
	.thumb_func
	.type	nvm_read, %function
nvm_read:
.LFB34:
	.loc 1 165 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
.LVL26:
	mov	r3, r0
	push	{r4, lr}
.LCFI3:
	.loc 1 166 0
	mov	r0, r1
.LVL27:
	.loc 1 165 0
	mov	r4, r2
	.loc 1 166 0
	mov	r1, r3
.LVL28:
	bl	memcpy
.LVL29:
	.loc 1 168 0
	mov	r0, r4
	pop	{r4, pc}
.LFE34:
	.size	nvm_read, .-nvm_read
	.bss
	.align	2
	.set	.LANCHOR0,. + 0
	.type	status.2439, %object
	.size	status.2439, 4
status.2439:
	.space	4
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
	.byte	0x8e
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
.LSFDE4:
	.4byte	.LEFDE4-.LASFDE4
.LASFDE4:
	.4byte	.Lframe0
	.4byte	.LFB33
	.4byte	.LFE33-.LFB33
	.byte	0x4
	.4byte	.LCFI2-.LFB33
	.byte	0xe
	.uleb128 0x20
	.byte	0x8e
	.uleb128 0x1
	.byte	0x8a
	.uleb128 0x2
	.byte	0x89
	.uleb128 0x3
	.byte	0x88
	.uleb128 0x4
	.byte	0x87
	.uleb128 0x5
	.byte	0x86
	.uleb128 0x6
	.byte	0x85
	.uleb128 0x7
	.byte	0x84
	.uleb128 0x8
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
	.4byte	.LFB34
	.4byte	.LFE34-.LFB34
	.byte	0x4
	.4byte	.LCFI3-.LFB34
	.byte	0xe
	.uleb128 0x8
	.byte	0x8e
	.uleb128 0x1
	.byte	0x84
	.uleb128 0x2
	.align	2
.LEFDE8:
	.text
.Letext0:
	.section	.debug_loc,"",%progbits
.Ldebug_loc0:
.LLST0:
	.4byte	.LFB31
	.4byte	.LCFI0
	.2byte	0x1
	.byte	0x5d
	.4byte	.LCFI0
	.4byte	.LFE31
	.2byte	0x2
	.byte	0x7d
	.sleb128 4
	.4byte	0x0
	.4byte	0x0
.LLST1:
	.4byte	.LVL0
	.4byte	.LVL1
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL1
	.4byte	.LFE31
	.2byte	0x1
	.byte	0x5e
	.4byte	0x0
	.4byte	0x0
.LLST2:
	.4byte	.LFB32
	.4byte	.LCFI1
	.2byte	0x1
	.byte	0x5d
	.4byte	.LCFI1
	.4byte	.LFE32
	.2byte	0x2
	.byte	0x7d
	.sleb128 16
	.4byte	0x0
	.4byte	0x0
.LLST3:
	.4byte	.LVL3
	.4byte	.LVL5
	.2byte	0x1
	.byte	0x50
	.4byte	0x0
	.4byte	0x0
.LLST4:
	.4byte	.LVL3
	.4byte	.LVL4
	.2byte	0x1
	.byte	0x51
	.4byte	0x0
	.4byte	0x0
.LLST5:
	.4byte	.LVL5
	.4byte	.LVL6
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL7
	.4byte	.LVL8
	.2byte	0x1
	.byte	0x50
	.4byte	0x0
	.4byte	0x0
.LLST6:
	.4byte	.LFB33
	.4byte	.LCFI2
	.2byte	0x1
	.byte	0x5d
	.4byte	.LCFI2
	.4byte	.LFE33
	.2byte	0x2
	.byte	0x7d
	.sleb128 32
	.4byte	0x0
	.4byte	0x0
.LLST7:
	.4byte	.LVL9
	.4byte	.LVL17
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL18
	.4byte	.LVL19
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL20
	.4byte	.LVL21
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL22
	.4byte	.LVL23
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL24
	.4byte	.LVL25
	.2byte	0x1
	.byte	0x50
	.4byte	0x0
	.4byte	0x0
.LLST8:
	.4byte	.LVL9
	.4byte	.LVL10
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL10
	.4byte	.LFE33
	.2byte	0x1
	.byte	0x59
	.4byte	0x0
	.4byte	0x0
.LLST9:
	.4byte	.LVL9
	.4byte	.LVL10
	.2byte	0x1
	.byte	0x52
	.4byte	.LVL10
	.4byte	.LVL11
	.2byte	0x1
	.byte	0x57
	.4byte	.LVL11
	.4byte	.LVL14
	.2byte	0x1
	.byte	0x52
	.4byte	.LVL14
	.4byte	.LVL24
	.2byte	0x1
	.byte	0x57
	.4byte	.LVL24
	.4byte	.LFE33
	.2byte	0x1
	.byte	0x52
	.4byte	0x0
	.4byte	0x0
.LLST10:
	.4byte	.LVL13
	.4byte	.LVL22
	.2byte	0x1
	.byte	0x54
	.4byte	0x0
	.4byte	0x0
.LLST11:
	.4byte	.LVL14
	.4byte	.LVL22
	.2byte	0x1
	.byte	0x55
	.4byte	0x0
	.4byte	0x0
.LLST12:
	.4byte	.LVL12
	.4byte	.LVL15
	.2byte	0x1
	.byte	0x58
	.4byte	.LVL15
	.4byte	.LVL16
	.2byte	0x1
	.byte	0x53
	.4byte	.LVL16
	.4byte	.LVL22
	.2byte	0x1
	.byte	0x58
	.4byte	0x0
	.4byte	0x0
.LLST14:
	.4byte	.LFB34
	.4byte	.LCFI3
	.2byte	0x1
	.byte	0x5d
	.4byte	.LCFI3
	.4byte	.LFE34
	.2byte	0x2
	.byte	0x7d
	.sleb128 8
	.4byte	0x0
	.4byte	0x0
.LLST15:
	.4byte	.LVL26
	.4byte	.LVL27
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL27
	.4byte	.LVL29
	.2byte	0x1
	.byte	0x53
	.4byte	0x0
	.4byte	0x0
.LLST16:
	.4byte	.LVL26
	.4byte	.LVL28
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL28
	.4byte	.LVL29
	.2byte	0x1
	.byte	0x50
	.4byte	0x0
	.4byte	0x0
.LLST17:
	.4byte	.LVL26
	.4byte	.LVL29
	.2byte	0x1
	.byte	0x52
	.4byte	.LVL29
	.4byte	.LFE34
	.2byte	0x1
	.byte	0x54
	.4byte	0x0
	.4byte	0x0
	.file 2 "/usr/local/arm-toolchain/bin/../lib/gcc/arm-none-eabi/4.2.3/../../../../arm-none-eabi/include/stdint.h"
	.file 3 "CMSIS/a2fxxxm3.h"
	.file 4 "CMSIS/core_cm3.h"
	.file 5 "CMSIS/system_a2fxxxm3.h"
	.section	.debug_info
	.4byte	0x5b2
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.LASF67
	.byte	0x1
	.4byte	.LASF68
	.4byte	.LASF69
	.4byte	0x0
	.4byte	.Ldebug_line0
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.4byte	.LASF0
	.uleb128 0x3
	.4byte	.LASF3
	.byte	0x2
	.byte	0x2e
	.4byte	0x33
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.4byte	.LASF1
	.uleb128 0x2
	.byte	0x2
	.byte	0x5
	.4byte	.LASF2
	.uleb128 0x3
	.4byte	.LASF4
	.byte	0x2
	.byte	0x3a
	.4byte	0x4c
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.4byte	.LASF5
	.uleb128 0x2
	.byte	0x4
	.byte	0x5
	.4byte	.LASF6
	.uleb128 0x3
	.4byte	.LASF7
	.byte	0x2
	.byte	0x54
	.4byte	0x65
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF8
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.4byte	.LASF9
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.4byte	.LASF10
	.uleb128 0x4
	.byte	0x4
	.byte	0x5
	.ascii	"int\000"
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF11
	.uleb128 0x5
	.byte	0x4
	.byte	0x7
	.uleb128 0x6
	.4byte	0x5a
	.uleb128 0x7
	.4byte	0x8b
	.uleb128 0x6
	.4byte	0x28
	.uleb128 0x8
	.byte	0x4
	.byte	0x3
	.byte	0xd7
	.4byte	0xcf
	.uleb128 0x9
	.ascii	"RBR\000"
	.byte	0x3
	.byte	0xd8
	.4byte	0xcf
	.uleb128 0x9
	.ascii	"THR\000"
	.byte	0x3
	.byte	0xd9
	.4byte	0x95
	.uleb128 0x9
	.ascii	"DLR\000"
	.byte	0x3
	.byte	0xda
	.4byte	0x95
	.uleb128 0xa
	.4byte	.LASF12
	.byte	0x3
	.byte	0xdb
	.4byte	0x5a
	.byte	0x0
	.uleb128 0x7
	.4byte	0x95
	.uleb128 0x8
	.byte	0x4
	.byte	0x3
	.byte	0xdf
	.4byte	0xfe
	.uleb128 0x9
	.ascii	"DMR\000"
	.byte	0x3
	.byte	0xe0
	.4byte	0x95
	.uleb128 0x9
	.ascii	"IER\000"
	.byte	0x3
	.byte	0xe1
	.4byte	0x95
	.uleb128 0xa
	.4byte	.LASF13
	.byte	0x3
	.byte	0xe2
	.4byte	0x5a
	.byte	0x0
	.uleb128 0x8
	.byte	0x4
	.byte	0x3
	.byte	0xe6
	.4byte	0x128
	.uleb128 0x9
	.ascii	"IIR\000"
	.byte	0x3
	.byte	0xe7
	.4byte	0x95
	.uleb128 0x9
	.ascii	"FCR\000"
	.byte	0x3
	.byte	0xe8
	.4byte	0x95
	.uleb128 0xa
	.4byte	.LASF14
	.byte	0x3
	.byte	0xe9
	.4byte	0x5a
	.byte	0x0
	.uleb128 0xb
	.byte	0x20
	.byte	0x3
	.byte	0xd5
	.4byte	0x21a
	.uleb128 0xc
	.4byte	0x9a
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xc
	.4byte	0xd4
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0xc
	.4byte	0xfe
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0xd
	.ascii	"LCR\000"
	.byte	0x3
	.byte	0xec
	.4byte	0x95
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0xe
	.4byte	.LASF15
	.byte	0x3
	.byte	0xed
	.4byte	0x28
	.byte	0x2
	.byte	0x23
	.uleb128 0xd
	.uleb128 0xe
	.4byte	.LASF16
	.byte	0x3
	.byte	0xee
	.4byte	0x41
	.byte	0x2
	.byte	0x23
	.uleb128 0xe
	.uleb128 0xd
	.ascii	"MCR\000"
	.byte	0x3
	.byte	0xef
	.4byte	0x95
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0xe
	.4byte	.LASF17
	.byte	0x3
	.byte	0xf0
	.4byte	0x28
	.byte	0x2
	.byte	0x23
	.uleb128 0x11
	.uleb128 0xe
	.4byte	.LASF18
	.byte	0x3
	.byte	0xf1
	.4byte	0x41
	.byte	0x2
	.byte	0x23
	.uleb128 0x12
	.uleb128 0xd
	.ascii	"LSR\000"
	.byte	0x3
	.byte	0xf2
	.4byte	0xcf
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.uleb128 0xe
	.4byte	.LASF19
	.byte	0x3
	.byte	0xf3
	.4byte	0x28
	.byte	0x2
	.byte	0x23
	.uleb128 0x15
	.uleb128 0xe
	.4byte	.LASF20
	.byte	0x3
	.byte	0xf4
	.4byte	0x41
	.byte	0x2
	.byte	0x23
	.uleb128 0x16
	.uleb128 0xd
	.ascii	"MSR\000"
	.byte	0x3
	.byte	0xf5
	.4byte	0xcf
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0xe
	.4byte	.LASF21
	.byte	0x3
	.byte	0xf6
	.4byte	0x28
	.byte	0x2
	.byte	0x23
	.uleb128 0x19
	.uleb128 0xe
	.4byte	.LASF22
	.byte	0x3
	.byte	0xf7
	.4byte	0x41
	.byte	0x2
	.byte	0x23
	.uleb128 0x1a
	.uleb128 0xd
	.ascii	"SR\000"
	.byte	0x3
	.byte	0xf8
	.4byte	0x95
	.byte	0x2
	.byte	0x23
	.uleb128 0x1c
	.uleb128 0xe
	.4byte	.LASF23
	.byte	0x3
	.byte	0xf9
	.4byte	0x28
	.byte	0x2
	.byte	0x23
	.uleb128 0x1d
	.uleb128 0xe
	.4byte	.LASF24
	.byte	0x3
	.byte	0xfa
	.4byte	0x41
	.byte	0x2
	.byte	0x23
	.uleb128 0x1e
	.byte	0x0
	.uleb128 0xf
	.byte	0x58
	.byte	0x3
	.2byte	0x1e3
	.4byte	0x36e
	.uleb128 0x10
	.4byte	.LASF25
	.byte	0x3
	.2byte	0x1e4
	.4byte	0x90
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x10
	.4byte	.LASF26
	.byte	0x3
	.2byte	0x1e5
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x10
	.4byte	.LASF27
	.byte	0x3
	.2byte	0x1e6
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x10
	.4byte	.LASF28
	.byte	0x3
	.2byte	0x1e7
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0x10
	.4byte	.LASF29
	.byte	0x3
	.2byte	0x1e8
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0x10
	.4byte	.LASF30
	.byte	0x3
	.2byte	0x1e9
	.4byte	0x90
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.uleb128 0x10
	.4byte	.LASF31
	.byte	0x3
	.2byte	0x1eb
	.4byte	0x90
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0x10
	.4byte	.LASF32
	.byte	0x3
	.2byte	0x1ec
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x1c
	.uleb128 0x10
	.4byte	.LASF33
	.byte	0x3
	.2byte	0x1ed
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x20
	.uleb128 0x10
	.4byte	.LASF34
	.byte	0x3
	.2byte	0x1ee
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x24
	.uleb128 0x10
	.4byte	.LASF35
	.byte	0x3
	.2byte	0x1ef
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x28
	.uleb128 0x10
	.4byte	.LASF36
	.byte	0x3
	.2byte	0x1f0
	.4byte	0x90
	.byte	0x2
	.byte	0x23
	.uleb128 0x2c
	.uleb128 0x10
	.4byte	.LASF37
	.byte	0x3
	.2byte	0x1f2
	.4byte	0x90
	.byte	0x2
	.byte	0x23
	.uleb128 0x30
	.uleb128 0x10
	.4byte	.LASF38
	.byte	0x3
	.2byte	0x1f3
	.4byte	0x90
	.byte	0x2
	.byte	0x23
	.uleb128 0x34
	.uleb128 0x10
	.4byte	.LASF39
	.byte	0x3
	.2byte	0x1f4
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x38
	.uleb128 0x10
	.4byte	.LASF40
	.byte	0x3
	.2byte	0x1f5
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x3c
	.uleb128 0x10
	.4byte	.LASF41
	.byte	0x3
	.2byte	0x1f6
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x40
	.uleb128 0x10
	.4byte	.LASF42
	.byte	0x3
	.2byte	0x1f7
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x44
	.uleb128 0x10
	.4byte	.LASF43
	.byte	0x3
	.2byte	0x1f8
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x48
	.uleb128 0x10
	.4byte	.LASF44
	.byte	0x3
	.2byte	0x1f9
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x4c
	.uleb128 0x10
	.4byte	.LASF45
	.byte	0x3
	.2byte	0x1fa
	.4byte	0x90
	.byte	0x2
	.byte	0x23
	.uleb128 0x50
	.uleb128 0x10
	.4byte	.LASF46
	.byte	0x3
	.2byte	0x1fb
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x54
	.byte	0x0
	.uleb128 0xf
	.byte	0x28
	.byte	0x3
	.2byte	0x35d
	.4byte	0x40e
	.uleb128 0x10
	.4byte	.LASF47
	.byte	0x3
	.2byte	0x35e
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x10
	.4byte	.LASF48
	.byte	0x3
	.2byte	0x35f
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x10
	.4byte	.LASF49
	.byte	0x3
	.2byte	0x360
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x10
	.4byte	.LASF12
	.byte	0x3
	.2byte	0x361
	.4byte	0x5a
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0x10
	.4byte	.LASF50
	.byte	0x3
	.2byte	0x362
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0x10
	.4byte	.LASF51
	.byte	0x3
	.2byte	0x363
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.uleb128 0x10
	.4byte	.LASF52
	.byte	0x3
	.2byte	0x364
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0x10
	.4byte	.LASF53
	.byte	0x3
	.2byte	0x365
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x1c
	.uleb128 0x10
	.4byte	.LASF54
	.byte	0x3
	.2byte	0x366
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x20
	.uleb128 0x10
	.4byte	.LASF55
	.byte	0x3
	.2byte	0x367
	.4byte	0x8b
	.byte	0x2
	.byte	0x23
	.uleb128 0x24
	.byte	0x0
	.uleb128 0x11
	.4byte	.LASF70
	.byte	0x1
	.byte	0x27
	.byte	0x1
	.4byte	.LFB31
	.4byte	.LFE31
	.4byte	.LLST0
	.4byte	0x441
	.uleb128 0x12
	.ascii	"c\000"
	.byte	0x1
	.byte	0x26
	.4byte	0x441
	.4byte	.LLST1
	.uleb128 0x13
	.4byte	.LASF58
	.byte	0x1
	.byte	0x28
	.4byte	0x7a
	.byte	0x1
	.byte	0x51
	.byte	0x0
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.4byte	.LASF56
	.uleb128 0x14
	.4byte	.LASF71
	.byte	0x1
	.byte	0x38
	.byte	0x1
	.4byte	0x7a
	.4byte	.LFB32
	.4byte	.LFE32
	.4byte	.LLST2
	.4byte	0x4a3
	.uleb128 0x15
	.4byte	.LASF57
	.byte	0x1
	.byte	0x37
	.4byte	0x81
	.4byte	.LLST3
	.uleb128 0x12
	.ascii	"cmd\000"
	.byte	0x1
	.byte	0x37
	.4byte	0x81
	.4byte	.LLST4
	.uleb128 0x16
	.4byte	.LASF58
	.byte	0x1
	.byte	0x3a
	.4byte	0x7a
	.4byte	.LLST5
	.uleb128 0x13
	.4byte	.LASF59
	.byte	0x1
	.byte	0x39
	.4byte	0x81
	.byte	0x5
	.byte	0x3
	.4byte	status.2439
	.byte	0x0
	.uleb128 0x17
	.byte	0x1
	.4byte	.LASF63
	.byte	0x1
	.byte	0x79
	.byte	0x1
	.4byte	0x4c
	.4byte	.LFB33
	.4byte	.LFE33
	.4byte	.LLST6
	.4byte	0x52f
	.uleb128 0x15
	.4byte	.LASF60
	.byte	0x1
	.byte	0x78
	.4byte	0x81
	.4byte	.LLST7
	.uleb128 0x12
	.ascii	"buf\000"
	.byte	0x1
	.byte	0x78
	.4byte	0x52f
	.4byte	.LLST8
	.uleb128 0x15
	.4byte	.LASF61
	.byte	0x1
	.byte	0x78
	.4byte	0x4c
	.4byte	.LLST9
	.uleb128 0x16
	.4byte	.LASF57
	.byte	0x1
	.byte	0x7a
	.4byte	0x81
	.4byte	.LLST10
	.uleb128 0x18
	.ascii	"src\000"
	.byte	0x1
	.byte	0x7b
	.4byte	0x531
	.uleb128 0x19
	.ascii	"i\000"
	.byte	0x1
	.byte	0x7c
	.4byte	0x4c
	.4byte	.LLST11
	.uleb128 0x16
	.4byte	.LASF62
	.byte	0x1
	.byte	0x7c
	.4byte	0x4c
	.4byte	.LLST12
	.uleb128 0x18
	.ascii	"ret\000"
	.byte	0x1
	.byte	0x7d
	.4byte	0x7a
	.byte	0x0
	.uleb128 0x1a
	.byte	0x4
	.uleb128 0x1b
	.byte	0x4
	.4byte	0x33
	.uleb128 0x1c
	.byte	0x1
	.4byte	.LASF72
	.byte	0x1
	.byte	0xac
	.byte	0x1
	.4byte	.LFB35
	.4byte	.LFE35
	.byte	0x1
	.byte	0x5d
	.uleb128 0x17
	.byte	0x1
	.4byte	.LASF64
	.byte	0x1
	.byte	0xa5
	.byte	0x1
	.4byte	0x4c
	.4byte	.LFB34
	.4byte	.LFE34
	.4byte	.LLST14
	.4byte	0x595
	.uleb128 0x15
	.4byte	.LASF60
	.byte	0x1
	.byte	0xa3
	.4byte	0x81
	.4byte	.LLST15
	.uleb128 0x12
	.ascii	"buf\000"
	.byte	0x1
	.byte	0xa3
	.4byte	0x52f
	.4byte	.LLST16
	.uleb128 0x15
	.4byte	.LASF61
	.byte	0x1
	.byte	0xa4
	.4byte	0x4c
	.4byte	.LLST17
	.byte	0x0
	.uleb128 0x1d
	.4byte	.LASF65
	.byte	0x4
	.2byte	0x6d0
	.4byte	0x5a3
	.byte	0x1
	.byte	0x1
	.uleb128 0x6
	.4byte	0x7a
	.uleb128 0x1e
	.4byte	.LASF66
	.byte	0x5
	.byte	0x16
	.4byte	0x5a
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
	.uleb128 0xe
	.byte	0x0
	.byte	0x0
	.uleb128 0x3
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
	.uleb128 0x4
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
	.uleb128 0x26
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x8
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
	.uleb128 0x9
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
	.uleb128 0xa
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
	.uleb128 0x10
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
	.uleb128 0x11
	.uleb128 0x2e
	.byte	0x1
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
	.uleb128 0x2e
	.byte	0x1
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
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x15
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
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x16
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
	.uleb128 0x17
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
	.uleb128 0x49
	.uleb128 0x13
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
	.uleb128 0x18
	.uleb128 0x34
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
	.uleb128 0x19
	.uleb128 0x34
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
	.uleb128 0x1a
	.uleb128 0xf
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x1b
	.uleb128 0xf
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x1c
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
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x1d
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
	.uleb128 0x1e
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
	.4byte	0x36
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x5b6
	.4byte	0x4a3
	.ascii	"nvm_write\000"
	.4byte	0x537
	.ascii	"nvm_init\000"
	.4byte	0x54a
	.ascii	"nvm_read\000"
	.4byte	0x0
	.section	.debug_aranges,"",%progbits
	.4byte	0x34
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0x0
	.2byte	0x0
	.2byte	0x0
	.4byte	.Ltext0
	.4byte	.Letext0-.Ltext0
	.4byte	.LFB31
	.4byte	.LFE31-.LFB31
	.4byte	.LFB32
	.4byte	.LFE32-.LFB32
	.4byte	.LFB33
	.4byte	.LFE33-.LFB33
	.4byte	0x0
	.4byte	0x0
	.section	.debug_str,"MS",%progbits,1
.LASF71:
	.ascii	"mss_nvm_exec_cmd\000"
.LASF41:
	.ascii	"TIM64_BGLOADVAL_U\000"
.LASF46:
	.ascii	"TIM64_MODE\000"
.LASF2:
	.ascii	"short int\000"
.LASF38:
	.ascii	"TIM64_VAL_L\000"
.LASF50:
	.ascii	"CONFIG_0\000"
.LASF67:
	.ascii	"GNU C 4.2.3\000"
.LASF62:
	.ascii	"written\000"
.LASF36:
	.ascii	"TIM2_MIS\000"
.LASF3:
	.ascii	"uint8_t\000"
.LASF48:
	.ascii	"CONTROL\000"
.LASF68:
	.ascii	"nvm.c\000"
.LASF26:
	.ascii	"TIM1_LOADVAL\000"
.LASF58:
	.ascii	"wait\000"
.LASF69:
	.ascii	"/home/vlad/uboot/u-boot/cpu/arm_cortexm3\000"
.LASF32:
	.ascii	"TIM2_LOADVAL\000"
.LASF44:
	.ascii	"TIM64_RIS\000"
.LASF9:
	.ascii	"long long int\000"
.LASF51:
	.ascii	"CONFIG_1\000"
.LASF6:
	.ascii	"long int\000"
.LASF57:
	.ascii	"addr\000"
.LASF28:
	.ascii	"TIM1_CTRL\000"
.LASF24:
	.ascii	"RESERVED12\000"
.LASF1:
	.ascii	"unsigned char\000"
.LASF22:
	.ascii	"RESERVED10\000"
.LASF23:
	.ascii	"RESERVED11\000"
.LASF66:
	.ascii	"g_FrequencyPCLK0\000"
.LASF0:
	.ascii	"signed char\000"
.LASF10:
	.ascii	"long long unsigned int\000"
.LASF7:
	.ascii	"uint32_t\000"
.LASF72:
	.ascii	"nvm_init\000"
.LASF11:
	.ascii	"unsigned int\000"
.LASF47:
	.ascii	"STATUS\000"
.LASF4:
	.ascii	"uint16_t\000"
.LASF29:
	.ascii	"TIM1_RIS\000"
.LASF8:
	.ascii	"long unsigned int\000"
.LASF52:
	.ascii	"PAGE_STATUS_0\000"
.LASF53:
	.ascii	"PAGE_STATUS_1\000"
.LASF63:
	.ascii	"nvm_write\000"
.LASF54:
	.ascii	"SEGMENT\000"
.LASF35:
	.ascii	"TIM2_RIS\000"
.LASF5:
	.ascii	"short unsigned int\000"
.LASF60:
	.ascii	"offset\000"
.LASF59:
	.ascii	"status\000"
.LASF25:
	.ascii	"TIM1_VAL\000"
.LASF56:
	.ascii	"char\000"
.LASF43:
	.ascii	"TIM64_CTRL\000"
.LASF40:
	.ascii	"TIM64_LOADVAL_L\000"
.LASF70:
	.ascii	"my_putc\000"
.LASF39:
	.ascii	"TIM64_LOADVAL_U\000"
.LASF64:
	.ascii	"nvm_read\000"
.LASF12:
	.ascii	"RESERVED0\000"
.LASF13:
	.ascii	"RESERVED1\000"
.LASF14:
	.ascii	"RESERVED2\000"
.LASF15:
	.ascii	"RESERVED3\000"
.LASF16:
	.ascii	"RESERVED4\000"
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
.LASF61:
	.ascii	"size\000"
.LASF34:
	.ascii	"TIM2_CTRL\000"
.LASF30:
	.ascii	"TIM1_MIS\000"
.LASF37:
	.ascii	"TIM64_VAL_U\000"
.LASF45:
	.ascii	"TIM64_MIS\000"
.LASF33:
	.ascii	"TIM2_BGLOADVAL\000"
.LASF65:
	.ascii	"ITM_RxBuffer\000"
.LASF55:
	.ascii	"ENVM_SELECT\000"
.LASF49:
	.ascii	"ENABLE\000"
.LASF31:
	.ascii	"TIM2_VAL\000"
.LASF42:
	.ascii	"TIM64_BGLOADVAL_L\000"
.LASF27:
	.ascii	"TIM1_BGLOADVAL\000"
	.ident	"GCC: (Sourcery G++ Lite 2008q1-126) 4.2.3"
