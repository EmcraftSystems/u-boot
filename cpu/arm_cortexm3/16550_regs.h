#ifndef __16550_REGS_H__
#define __16550_REGS_H__

/* Register offsets */
#define	RBR_OFFSET		0
#define	THR_OFFSET		0
#define	DLR_OFFSET		0
#define	DMR_OFFSET		1
#define	IER_OFFSET		1
#define	IIR_OFFSET		2
#define	FCR_OFFSET		2
#define	LCR_OFFSET		3
#define	MCR_OFFSET		4
#define	LSR_OFFSET		5
#define	MSR_OFFSET		6
#define	SR_OFFSET		7

/* FCR register bits */
#define FCR_EN_RX_TX_FIFO	(1<<0)
#define	FCR_CLR_RX_FIFO		(1<<1)
#define	FCR_CLR_TX_FIFO		(1<<2)
#define	FCR_RX_TX_RDY_EN	(1<<3)
#define	FCR_RX_TRIG_LEVEL_MASK	0x3
#define	FCR_RX_TRIG_LEVEL_SHIFT	6

/* IIR register bits */
#define IIR_IIR_MASK		0xF
#define	IIR_IIR_SHIFT		0
#define IIR_TYPE_RLS		0x6
#define IIR_TYPE_RDA		0x4
#define IIR_TYPE_CTI		0xC
#define IIR_TYPE_THRE		0x2
#define IIR_TYPE_MS		0x0
#define IIR_MODE_MASK		0x3
#define IIR_MODE_SHIFT		6

/* LCR register bits */
#define LCR_WLS_MASK		0x3
#define LCR_WLS_SHIFT		0
#define LCR_STB			(1<<2)
#define LCR_PEN			(1<<3)
#define LCR_EPS			(1<<4)
#define LCR_SP			(1<<5)
#define LCR_SB			(1<<6)
#define LCR_DLAB		(1<<7)

/* MCR register bits */
#define MCR_DTR			(1<<0)
#define MCR_RTS			(1<<1)
#define	MCR_OUT1		(1<<2)
#define	MCR_OUT2		(1<<3)
#define	MCR_LOOP		(1<<4)

/* LSR register bits */
#define LSR_DR			(1<<0)
#define LSR_OE			(1<<1)
#define LSR_PE			(1<<2)
#define LSR_FE			(1<<3)
#define LSR_BI			(1<<4)
#define LSR_THRE		(1<<5)
#define LSR_TEMT		(1<<6)
#define LSR_FIER		(1<<7)

/* MSR register bits */
#define MSR_DCTS		(1<<0)
#define MSR_DDSR		(1<<1)
#define MSR_TERI		(1<<2)
#define MSR_DDCD		(1<<3)
#define MSR_CTS			(1<<4)
#define MSR_DSR			(1<<5)
#define MSR_RI			(1<<6)
#define MSR_DCD			(1<<7)

#endif /* __16550_REGS_H__ */
