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
	.file	"cmd_cptf.c"
	.section	.debug_abbrev,"",%progbits
.Ldebug_abbrev0:
	.section	.debug_info,"",%progbits
.Ldebug_info0:
	.section	.debug_line,"",%progbits
.Ldebug_line0:
	.text
.Ltext0:
	.align	2
	.global	do_cptf
	.thumb
	.thumb_func
	.type	do_cptf, %function
do_cptf:
.LFB102:
	.file 1 "cmd_cptf.c"
	.loc 1 89 0
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
.LVL0:
	@ lr needed for prologue
.L2:
	b	.L2
.LFE102:
	.size	do_cptf, .-do_cptf
	.global	__u_boot_cmd_cptf
	.section	.u_boot_cmd,"aw",%progbits
	.align	2
	.type	__u_boot_cmd_cptf, %object
	.size	__u_boot_cmd_cptf, 20
__u_boot_cmd_cptf:
	.word	.LC0
	.word	5
	.word	0
	.word	do_cptf
	.word	.LC1
	.section	.rodata.str1.4,"aMS",%progbits,1
	.align	2
.LC0:
	.ascii	"cptf\000"
	.space	3
.LC1:
	.ascii	"copy memory buffer to internal Flash of the A2F\000"
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
	.4byte	.LFB102
	.4byte	.LFE102-.LFB102
	.align	2
.LEFDE0:
	.text
.Letext0:
	.file 2 "/home/vlad/uboot/u-boot/include/command.h"
	.file 3 "/home/vlad/uboot/u-boot/include/linux/types.h"
	.file 4 "CMSIS/core_cm3.h"
	.section	.debug_info
	.4byte	0x1cb
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.LASF24
	.byte	0x1
	.4byte	.LASF25
	.4byte	.LASF26
	.4byte	.Ltext0
	.4byte	.Letext0
	.4byte	.Ldebug_line0
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.4byte	.LASF0
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF1
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.4byte	.LASF2
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.4byte	.LASF3
	.uleb128 0x2
	.byte	0x2
	.byte	0x5
	.4byte	.LASF4
	.uleb128 0x3
	.byte	0x4
	.byte	0x5
	.ascii	"int\000"
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF5
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.4byte	.LASF6
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.4byte	.LASF7
	.uleb128 0x4
	.byte	0x4
	.byte	0x7
	.uleb128 0x2
	.byte	0x4
	.byte	0x5
	.4byte	.LASF8
	.uleb128 0x5
	.byte	0x4
	.4byte	0x74
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.4byte	.LASF9
	.uleb128 0x6
	.4byte	.LASF14
	.byte	0x3
	.byte	0x5c
	.4byte	0x2c
	.uleb128 0x7
	.4byte	.LASF27
	.byte	0x14
	.byte	0x2
	.byte	0x2e
	.4byte	0xd9
	.uleb128 0x8
	.4byte	.LASF10
	.byte	0x2
	.byte	0x2f
	.4byte	0x6e
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x8
	.4byte	.LASF11
	.byte	0x2
	.byte	0x30
	.4byte	0x48
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x8
	.4byte	.LASF12
	.byte	0x2
	.byte	0x31
	.4byte	0x48
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x9
	.ascii	"cmd\000"
	.byte	0x2
	.byte	0x33
	.4byte	0x104
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0x8
	.4byte	.LASF13
	.byte	0x2
	.byte	0x34
	.4byte	0x6e
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.byte	0x0
	.uleb128 0xa
	.byte	0x1
	.4byte	0x48
	.4byte	0xf8
	.uleb128 0xb
	.4byte	0xf8
	.uleb128 0xb
	.4byte	0x48
	.uleb128 0xb
	.4byte	0x48
	.uleb128 0xb
	.4byte	0xfe
	.byte	0x0
	.uleb128 0x5
	.byte	0x4
	.4byte	0x86
	.uleb128 0x5
	.byte	0x4
	.4byte	0x6e
	.uleb128 0x5
	.byte	0x4
	.4byte	0xd9
	.uleb128 0x6
	.4byte	.LASF15
	.byte	0x2
	.byte	0x3e
	.4byte	0x86
	.uleb128 0x5
	.byte	0x4
	.4byte	0x10a
	.uleb128 0xc
	.byte	0x1
	.4byte	.LASF28
	.byte	0x1
	.byte	0x59
	.byte	0x1
	.4byte	0x48
	.4byte	.LFB102
	.4byte	.LFE102
	.byte	0x1
	.byte	0x5d
	.4byte	0x1a9
	.uleb128 0xd
	.4byte	.LASF16
	.byte	0x1
	.byte	0x58
	.4byte	0x115
	.byte	0x1
	.byte	0x50
	.uleb128 0xd
	.4byte	.LASF17
	.byte	0x1
	.byte	0x58
	.4byte	0x48
	.byte	0x1
	.byte	0x51
	.uleb128 0xd
	.4byte	.LASF18
	.byte	0x1
	.byte	0x58
	.4byte	0x48
	.byte	0x1
	.byte	0x52
	.uleb128 0xd
	.4byte	.LASF19
	.byte	0x1
	.byte	0x58
	.4byte	0xfe
	.byte	0x1
	.byte	0x53
	.uleb128 0xe
	.ascii	"dst\000"
	.byte	0x1
	.byte	0x5a
	.4byte	0x7b
	.uleb128 0xe
	.ascii	"src\000"
	.byte	0x1
	.byte	0x5b
	.4byte	0x7b
	.uleb128 0xf
	.4byte	.LASF20
	.byte	0x1
	.byte	0x5c
	.4byte	0x7b
	.uleb128 0xf
	.4byte	.LASF21
	.byte	0x1
	.byte	0x5d
	.4byte	0x48
	.uleb128 0xe
	.ascii	"ret\000"
	.byte	0x1
	.byte	0x5e
	.4byte	0x48
	.uleb128 0x10
	.4byte	.LASF29
	.byte	0x1
	.byte	0x84
	.byte	0x0
	.uleb128 0x11
	.4byte	.LASF22
	.byte	0x4
	.2byte	0x6d0
	.4byte	0x1b7
	.byte	0x1
	.byte	0x1
	.uleb128 0x12
	.4byte	0x48
	.uleb128 0x13
	.4byte	.LASF23
	.byte	0x1
	.byte	0x88
	.4byte	0x10a
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	__u_boot_cmd_cptf
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
	.uleb128 0xe
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
	.uleb128 0x8
	.byte	0x0
	.byte	0x0
	.uleb128 0x4
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x5
	.uleb128 0xf
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x6
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
	.uleb128 0x7
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
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
	.uleb128 0x38
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0xa
	.uleb128 0x15
	.byte	0x1
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xb
	.uleb128 0x5
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xc
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
	.uleb128 0xa
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xd
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
	.uleb128 0xe
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
	.uleb128 0xf
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
	.byte	0x0
	.byte	0x0
	.uleb128 0x10
	.uleb128 0xa
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
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
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0x0
	.byte	0x0
	.uleb128 0x12
	.uleb128 0x35
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
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
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.section	.debug_pubnames,"",%progbits
	.4byte	0x30
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x1cf
	.4byte	0x11b
	.ascii	"do_cptf\000"
	.4byte	0x1bc
	.ascii	"__u_boot_cmd_cptf\000"
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
.LASF25:
	.ascii	"cmd_cptf.c\000"
.LASF24:
	.ascii	"GNU C 4.2.3\000"
.LASF28:
	.ascii	"do_cptf\000"
.LASF11:
	.ascii	"maxargs\000"
.LASF29:
	.ascii	"Done\000"
.LASF7:
	.ascii	"long long unsigned int\000"
.LASF0:
	.ascii	"unsigned char\000"
.LASF26:
	.ascii	"/home/vlad/uboot/u-boot/cpu/arm_cortexm3\000"
.LASF1:
	.ascii	"long unsigned int\000"
.LASF2:
	.ascii	"short unsigned int\000"
.LASF23:
	.ascii	"__u_boot_cmd_cptf\000"
.LASF22:
	.ascii	"ITM_RxBuffer\000"
.LASF13:
	.ascii	"usage\000"
.LASF17:
	.ascii	"flag\000"
.LASF14:
	.ascii	"ulong\000"
.LASF5:
	.ascii	"unsigned int\000"
.LASF9:
	.ascii	"char\000"
.LASF18:
	.ascii	"argc\000"
.LASF6:
	.ascii	"long long int\000"
.LASF10:
	.ascii	"name\000"
.LASF12:
	.ascii	"repeatable\000"
.LASF21:
	.ascii	"do_reset\000"
.LASF4:
	.ascii	"short int\000"
.LASF19:
	.ascii	"argv\000"
.LASF8:
	.ascii	"long int\000"
.LASF27:
	.ascii	"cmd_tbl_s\000"
.LASF15:
	.ascii	"cmd_tbl_t\000"
.LASF3:
	.ascii	"signed char\000"
.LASF16:
	.ascii	"cmdtp\000"
.LASF20:
	.ascii	"size\000"
	.ident	"GCC: (Sourcery G++ Lite 2008q1-126) 4.2.3"
