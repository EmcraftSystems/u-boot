/*
 * (C) Copyright 2012
 *
 * Alexander Potashev, Emcraft Systems, aspotashev@emcraft.com
 * Yuri Tikhonov, Emcraft Systems, yur@emcraft.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Actel M2S (SmartFusion2) Ethernet driver
 */

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <config.h>

/*
 * Define DEBUG to enable debug() messages in this module
 */
#undef DEBUG

#include <common.h>
#include <net.h>
#include <miiphy.h>
#include <asm/errno.h>

#include <asm/arch/m2s.h>

/******************************************************************************
 * Local constants and macros
 ******************************************************************************/

/*
 * Configs.
 * It makes sense to use for tuning only RX_ETH_BUFFER config.
 * As for the appropriate TX_ETH_BUFFER config - it changes nothing, we always
 * do 'busy-send' of one frame per 'send' call: it's more simple, and in case
 * of u-boot is completely OK
 */
#if !defined(CONFIG_SYS_RX_ETH_BUFFER)
# error CONFIG_SYS_RX_ETH_BUFFER should be set
#endif

/*
 * Maximum frame sizes we allow
 */
#define M2S_FRM_MAX_LEN		0x600	/* 1536, same as PKTSIZE_ALIGN	      */

/*
 * Just more compact names
 */
#define M2S_TX_BD_NUM		1
#define M2S_RX_BD_NUM		CONFIG_SYS_RX_ETH_BUFFER

/*
 * Different timeouts, in msec
 */
#define M2S_FIFO_TOUT		100	/* FIFO initialization timeout	      */
#define M2S_MII_TOUT		250	/* MII read/write cycle timeout	      */
#define M2S_SEND_TOUT		1000	/* Waiting for busy tx timeout	      */
#define M2S_AUTONEG_TOUT	10000	/* Auto-negotiation timeout	      */

/*
 * Device name
 */
#define M2S_MAC_NAME		"M2S_MAC" /* u-boot ethernet driver name      */
#define M2S_MII_NAME		"M2S_MII" /* u-boot miiphy driver name	      */

/*
 * Base reg addresses
 */
#define M2S_MAC_CFG_ADR		0x40041000 /* Ethernet MAC PE-MCXMAC reg base */
#define M2S_MAC_DMA_ADR		0x40041180 /* Ethernet MAC M-AHB reg base     */

/*
 * CFG1 register fields
 */
#define M2S_MAC_CFG1_RST	(1 << 31)	/* PE-MCXMAC full reset	      */
#define M2S_MAC_CFG1_RXCTL_RST	(1 << 19)
#define M2S_MAC_CFG1_TXCTL_RST	(1 << 18)
#define M2S_MAC_CFG1_RX_RST	(1 << 17)
#define M2S_MAC_CFG1_TX_RST	(1 << 16)
#define M2S_MAC_CFG1_RX_ENA	(1 << 2)	/* MAC receive enable	      */
#define M2S_MAC_CFG1_TX_ENA	(1 << 0)	/* MAC transmit enable	      */

/*
 * CFG2 register fields
 */
#define M2S_MAC_CFG2_PREAM_LEN_BIT	12
#define M2S_MAC_CFG2_PREAM_LEN_MSK	0xf
#define M2S_MAC_CFG2_MODE_BIT		8	/* MAC interface mode	      */
#define M2S_MAC_CFG2_MODE_MSK		0x3
#define M2S_MAC_CFG2_MODE_BYTE		0x2	/* Byte mode		      */
#define M2S_MAC_CFG2_MODE_MII		0x1	/* Nibble mode		      */
#define M2S_MAC_CFG2_HUGE_FRAME_EN	(1 << 5)
#define M2S_MAC_CFG2_LEN_CHECK		(1 << 4)
#define M2S_MAC_CFG2_PAD_CRC		(1 << 2) /* PAD&CRC appending enable   */
#define M2S_MAC_CFG2_CRC_EN		(1 << 1)
#define M2S_MAC_CFG2_FULL_DUP		(1 << 0) /* PE-MCXMAC Full duplex      */

/*
 * IPG/IFG register
 */
#define M2S_MAC_IFG_BTBIPG_BIT		0
#define M2S_MAC_IFG_MINIFGENF_BIT	8
#define M2S_MAC_IFG_NONBTBIPG_BIT	16

/*
 * half-duplex register
 */
#define M2S_MAC_HALF_DUPLEX_ABEB_TUNC_BIT		20
#define M2S_MAC_HALF_DUPLEX_ABEB_ENABLE			(1 << 19)
#define M2S_MAC_HALF_DUPLEX_BACKPRES_NOBACKOFF		(1 << 18)
#define M2S_MAC_HALF_DUPLEX_NO_BACKOFF			(1 << 17)
#define M2S_MAC_HALF_DUPLEX_EXCS_DEFER			(1 << 16)
#define M2S_MAC_HALF_DUPLEX_RETX_MAX_BIT		12
#define M2S_MAC_HALF_DUPLEX_SLOTTIME_BIT		0

/*
 * MII_COMMAND register fields
 */
#define M2S_MAC_MII_CMD_READ	(1 << 0)	/* Do single Read cycle	      */

/*
 * MII_ADDRESS register fields
 */
#define M2S_MAC_MII_ADR_PHY_BIT	8		/* 5-bit PHY address	      */
#define M2S_MAC_MII_ADR_REG_BIT	0		/* 5-bit register address     */

/*
 * MII_INDICATORS register fields
 */
#define M2S_MAC_MII_IND_NVAL	(1 << 2)	/* Read Data not yet validated*/
#define M2S_MAC_MII_IND_BUSY	(1 << 0)	/* MII is performing cycle    */

/*
 * DMA_RX_CTRL/DMA_TX_CTRL register fields
 */
#define M2S_MAC_DMA_CTRL_ENA	(1 << 0)	/* Enable Tx/Rx DMA xfers     */

/*
 * Interface Control register fields
 */
#define M2S_MAC_INTF_RESET		(1 << 31) /* Reset interface module */
#define M2S_MAC_INTF_SPEED_100		(1 << 4)  /* MII PHY speed 100Mbit */

/*
 * FIFO_CFG0 register fields
 */
#define M2S_MAC_FIFO_CFG0_FTFENRPLY	(1 << 20) /* Fabric tx iface ena ack  */
#define M2S_MAC_FIFO_CFG0_STFENRPLY	(1 << 19) /* PE-MCXMAC tx iface	      */
#define M2S_MAC_FIFO_CFG0_FRFENRPLY	(1 << 18) /* Fabric rx iface	      */
#define M2S_MAC_FIFO_CFG0_SRFENRPLY	(1 << 17) /* PE-MCXMAC rx iface	      */
#define M2S_MAC_FIFO_CFG0_WTMENRPLY	(1 << 16) /* PE-MCXMAC watermark modu */

/*
 * Note, PE-MCXMAC rx becomes 'enabled' only after smth received from line,
 * so don't care about this at initialization: miss M2S_MAC_FIFO_CFG0_SRFENRPLY
 * in the following mask
 */
#define M2S_MAC_FIFO_CFG0_ALL_RPLY	(M2S_MAC_FIFO_CFG0_FTFENRPLY      | \
		M2S_MAC_FIFO_CFG0_STFENRPLY | M2S_MAC_FIFO_CFG0_FRFENRPLY | \
		M2S_MAC_FIFO_CFG0_WTMENRPLY)

#define M2S_MAC_FIFO_CFG0_FTFENREQ	(1 << 12) /* Fabric tx iface ena req  */
#define M2S_MAC_FIFO_CFG0_STFENREQ	(1 << 11) /* PE-MCXMAC tx iface	      */
#define M2S_MAC_FIFO_CFG0_FRFENREQ	(1 << 10) /* Fabric rx iface	      */
#define M2S_MAC_FIFO_CFG0_SRFENREQ	(1 << 9)  /* PE-MCXMAC rx iface	      */
#define M2S_MAC_FIFO_CFG0_WTMENREQ	(1 << 8)  /* PE-MCXMAC watermark modu */
#define M2S_MAC_FIFO_CFG0_ALL_REQ	(M2S_MAC_FIFO_CFG0_FTFENREQ     | \
		M2S_MAC_FIFO_CFG0_STFENREQ | M2S_MAC_FIFO_CFG0_FRFENREQ | \
		M2S_MAC_FIFO_CFG0_SRFENREQ | M2S_MAC_FIFO_CFG0_WTMENREQ)

#define M2S_MAC_FIFO_CFG0_HSTRSTFT	(1 << 4)  /* Fabric tx iface reset    */
#define M2S_MAC_FIFO_CFG0_HSTRSTST	(1 << 3)  /* PE-MCXMAC tx iface	      */
#define M2S_MAC_FIFO_CFG0_HSTRSTFR	(1 << 2)  /* Fabric rx iface	      */
#define M2S_MAC_FIFO_CFG0_HSTRSTSR	(1 << 1)  /* PE-MCXMAC rx iface	      */
#define M2S_MAC_FIFO_CFG0_HSTRSTWT	(1 << 0)  /* PE-MCXMAC watermark modu */
#define M2S_MAC_FIFO_CFG0_ALL_RST	(M2S_MAC_FIFO_CFG0_HSTRSTFT     | \
		M2S_MAC_FIFO_CFG0_HSTRSTST | M2S_MAC_FIFO_CFG0_HSTRSTFR | \
		M2S_MAC_FIFO_CFG0_HSTRSTSR | M2S_MAC_FIFO_CFG0_HSTRSTWT)

/*
 * MAC Configuration Register in Sysreg block fields
 */
#define M2S_SYS_MAC_CR_PM_BIT	2		/* PHY mode		      */
#define M2S_SYS_MAC_CR_PM_MSK	0x7
#define M2S_SYS_MAC_CR_PM_TBI	0x2		/* Use TBI mode		      */
#define M2S_SYS_MAC_CR_PM_MII	0x3		/* Use MII mode		      */
#define M2S_SYS_MAC_CR_LS_BIT	0		/* Line speed		      */
#define M2S_SYS_MAC_CR_LS_MSK	0x3
#define M2S_SYS_MAC_CR_LS_10	0x0		/* 10 Mbps		      */
#define M2S_SYS_MAC_CR_LS_100	0x1		/* 100 Mbps		      */
#define M2S_SYS_MAC_CR_LS_1000	0x2		/* 1000 Mbps		      */

/*
 * Software Reset Control Register fields
 */
#define M2S_SYS_SOFT_RST_CR_MAC	(1 << 4)	/* MAC_SOFTRESET	      */

/*
 * BD constants
 */
#define M2S_BD_EMPTY		(1 << 31)	/* Empty flag		      */
#define M2S_BD_SIZE_MSK		0xFFF		/* Frame size mask	      */

#if (M2S_FRM_MAX_LEN > M2S_BD_SIZE_MSK)
# error M2S_FRM_MAX_LEN too big
#endif

/*
 * MAC register access macros
 */
#define M2S_MAC_CFG	((volatile struct m2s_mac_cfg_regs *)M2S_MAC_CFG_ADR)
#define M2S_MAC_DMA	((volatile struct m2s_mac_dma_regs *)M2S_MAC_DMA_ADR)

/******************************************************************************
 * Local C-types
 ******************************************************************************/

/*
 * Ethernet MAC PE-MCXMAC Register Map, accessed via M2S_MAC_CFG macro
 */
struct m2s_mac_cfg_regs {
	u32	cfg1;			/* MAC Configuration register 1	      */
	u32	cfg2;			/* MAC Configuration register 2	      */
	u32	ifg;			/* Inter Packet/Frame gaps	      */
	u32	half_duplex;		/* Definition of half duplex	      */
	u32	max_frame_length;	/* Maximum frame size		      */
	u32	rsv[2];
	u32	test;			/* For testing purposes		      */
	u32	mii_config;		/* MII configuration		      */
	u32	mii_command;		/* MII command			      */
	u32	mii_address;		/* 5-bit PHY addr / 5 bit reg addr    */
	u32	mii_ctrl;		/* MII Mgmt write cycle control	      */
	u32	mii_status;		/* MII Mgmt read cycle status	      */
	u32	mii_ind;		/* MII Mgmt indication		      */
	u32	if_ctrl;		/* Interface controls		      */
	u32	if_stat;		/* Interface status		      */
	u32	station_addr[2];	/* Station MAC address		      */
	u32	fifo_cfg[6];		/* A-MCXFIFO configuration registers  */
	u32	fifo_ram_access[8];	/* FIFO RAM access registers	      */
};

/*
 * Ethernet MAC M-AHB Register Map, accessed via M2S_MAC_DMA macro
 */
struct m2s_mac_dma_regs {
	u32	tx_ctrl;		/* Transmit control register	      */
	u32	tx_desc;		/* Pointer to Transmit Descriptor     */
	u32	tx_stat;		/* Transmit Status register	      */
	u32	rx_ctrl;		/* Receive Control register	      */
	u32	rx_desc;		/* Pointer to Receive Descriptor      */
	u32	rx_stat;		/* Receive Status register	      */
	u32	irq_msk;		/* Interrupt Mask register	      */
	u32	irq;			/* Interrupts register		      */
};

/*
 * M2S ETH DMA Receive/Transmit descriptor
 */
struct m2s_eth_dma_bd {
	volatile u8		*frame;		/* Pointer to frame buffer    */
	u32			cfg_size;	/* Cfg flags and frame size   */
	struct m2s_eth_dma_bd	*next;		/* Pointer to next BD in chain*/
};

/******************************************************************************
 * Local prototypes
 ******************************************************************************/

static  int m2s_eth_init(struct eth_device *dev, bd_t *bd);
static  int m2s_eth_send(struct eth_device *dev, volatile void *pkt, int len);
static  int m2s_eth_recv(struct eth_device *dev);
static void m2s_eth_halt(struct eth_device *dev);

static  int m2s_mii_read(char *devname, u8 addr, u8 reg, u16 *val);
static  int m2s_mii_write(char *devname, u8 addr, u8 reg, u16 val);

#ifndef CONFIG_M2S_ETH_MODE_SGMII
static  int m2s_phy_probe(void);
#endif

static void m2s_mac_dump_regs(char *who);

/******************************************************************************
 * Variables local to this module
 ******************************************************************************/

/*
 * U-boot ethernet device standard descriptor for M2S MAC
 */
static struct eth_device	m2s_eth_dev = {
	.init	= m2s_eth_init,
	.halt	= m2s_eth_halt,
	.send	= m2s_eth_send,
	.recv	= m2s_eth_recv,
};

/*
 * PHY address
 */
#ifndef CONFIG_M2S_ETH_MODE_SGMII
static u8			m2s_phy_addr = 0xFF;
#endif
static u8			m2s_mii_speed = M2S_SYS_MAC_CR_LS_100;

/*
 * Current indexes within m2s_bd_Xx[] (idx of BT to process next)
 */
static int			m2s_bd_cur_tx, m2s_bd_cur_rx;

/*
 * Buffer descriptors (updated by DMA too, so specify them as volatile)
 */
static volatile struct m2s_eth_dma_bd	m2s_bd_tx[M2S_TX_BD_NUM];
static volatile struct m2s_eth_dma_bd	m2s_bd_rx[M2S_RX_BD_NUM];

/*
 * Rx buffers
 */
static u8			m2s_buf_rx[M2S_RX_BD_NUM][M2S_FRM_MAX_LEN];

#define M88E1340_PHY_ADDR 0
#define SF2_MSGMII_PHY_ADDR 0x1e

/* M88E1340 PHY registers */

/* Advertisement control register. */
#define MII_ADVERTISE           0x04        /* Advertisement control reg   */

#define ADVERTISE_SLCT          0x001f      /* Selector bits               */
#define ADVERTISE_CSMA          0x0001      /* Only selector supported     */
#define ADVERTISE_10HALF        0x0020      /* Try for 10mbps half-duplex  */
#define ADVERTISE_1000XFULL     0x0020      /* Try for 1000BASE-X full-duplex */
#define ADVERTISE_10FULL        0x0040      /* Try for 10mbps full-duplex  */
#define ADVERTISE_1000XHALF     0x0040      /* Try for 1000BASE-X half-duplex */
#define ADVERTISE_100HALF       0x0080      /* Try for 100mbps half-duplex */
#define ADVERTISE_1000XPAUSE    0x0080      /* Try for 1000BASE-X pause    */
#define ADVERTISE_100FULL       0x0100      /* Try for 100mbps full-duplex */
#define ADVERTISE_1000XPSE_ASYM 0x0100      /* Try for 1000BASE-X asym pause */
#define ADVERTISE_100BASE4      0x0200      /* Try for 100mbps 4k packets  */
#define ADVERTISE_PAUSE_CAP     0x0400      /* Try for pause               */
#define ADVERTISE_PAUSE_ASYM    0x0800      /* Try for asymetric pause     */
#define ADVERTISE_RESV          0x1000      /* Unused...                   */
#define ADVERTISE_RFAULT        0x2000      /* Say we can detect faults    */
#define ADVERTISE_LPACK         0x4000      /* Ack link partners response  */
#define ADVERTISE_NPAGE         0x8000      /* Next page bit               */

/* 1000BASE-T Control register */
#define MII_CTRL1000            0x09        /* 1000BASE-T control          */

#define ADVERTISE_1000FULL      0x0200      /* Advertise 1000BASE-T full duplex */
#define ADVERTISE_1000HALF      0x0100      /* Advertise 1000BASE-T half duplex */

#define M88E1340_EXT_ADDR_PAGE_CR       0x16
#define PAGE_0                          0x00

#define M88E1340_PHY_STATUS		0x11
#define M88E1340_PHY_STATUS_1000	0x8000
#define M88E1340_PHY_STATUS_100	        0x4000
#define M88E1340_PHY_STATUS_SPD_MASK	0xc000
#define M88E1340_PHY_STATUS_FULLDUPLEX	0x2000
#define M88E1340_PHY_STATUS_RESOLVED	0x0800
#define M88E1340_PHY_STATUS_LINK	0x0400

#ifdef CONFIG_M2S_ETH_MODE_SGMII

static int msgmii_phy_init(void)
{
	u16 val;
	int rv;

	/* Reset M-SGMII. */
	rv = miiphy_write(M2S_MII_NAME, SF2_MSGMII_PHY_ADDR, 0x00, 0x9000u);
	if (rv != 0) {
		return -1;
	}
	/* Register 0x04 of M-SGMII must be always be set to 0x0001. */
	rv = miiphy_write(M2S_MII_NAME, SF2_MSGMII_PHY_ADDR, 0x04, 0x0001);
	if (rv != 0) {
		return -1;
	}
	/*
	 * Enable auto-negotiation inside SmartFusion2 SGMII block.
	 */
	rv = miiphy_read(M2S_MII_NAME, SF2_MSGMII_PHY_ADDR, 0, &val);
	if (rv != 0) {
		return -1;
	}
	val |= 0x1000;
	rv = miiphy_write(M2S_MII_NAME, SF2_MSGMII_PHY_ADDR, 0x0, val);
	if (rv != 0) {
		return -1;
	}

	return 0;
}

static int m881340_phy_set_link_speed(void)
{
	u16 val;
	int rv;
        /* Set auto-negotiation advertisement. */
	/* Set 10Mbps and 100Mbps advertisement. */
	rv = miiphy_read(M2S_MII_NAME, M88E1340_PHY_ADDR, MII_ADVERTISE, &val);
	if (rv != 0) {
		goto out;
	}
	val |= (ADVERTISE_10HALF | ADVERTISE_10FULL
		| ADVERTISE_100HALF | ADVERTISE_100FULL);
	rv = miiphy_write(M2S_MII_NAME, M88E1340_PHY_ADDR, MII_ADVERTISE, val);
	if (rv != 0) {
		goto out;
	}

	/* Set 1000Mbps advertisement. */
	rv = miiphy_read(M2S_MII_NAME, M88E1340_PHY_ADDR, MII_CTRL1000, &val);
	if (rv != 0) {
		goto out;
	}

	val |= (ADVERTISE_1000FULL | ADVERTISE_1000HALF);
	rv = miiphy_write(M2S_MII_NAME, M88E1340_PHY_ADDR, MII_CTRL1000, val);
	if (rv != 0) {
		goto out;
	}
 out:
	if (rv) {
		printf("%s %d %s return %d\n", __FILE__, __LINE__,
				__func__, rv);
	}
	return rv;
}

static int msgmii_phy_autonegotiate(void)
{
	int rv;
	u16 val;
	int timeout;

	/* Enable auto-negotiation. */
	rv = miiphy_write(M2S_MII_NAME, M88E1340_PHY_ADDR,
			M88E1340_EXT_ADDR_PAGE_CR, PAGE_0);
	if (rv != 0) {
		goto out;
	}
	val = 0x9340 | PHY_BMCR_100_MBPS;
	rv = miiphy_write(M2S_MII_NAME, M88E1340_PHY_ADDR, PHY_BMCR, val);
	if (rv != 0) {
		goto out;
	}

	/* Wait for copper auto-negotiation to complete. */
	timeout = M2S_AUTONEG_TOUT/1000;
	while (timeout--) {
		rv = miiphy_read(M2S_MII_NAME, M88E1340_PHY_ADDR,
				PHY_BMSR, &val);
		if (rv != 0) {
			printf("mii err %d.\n", rv);
			goto out;
		}
		if (!(val & PHY_BMSR_AUTN_COMP)) {
			int i;
			for (i = 0; i < 1000; i++)
				udelay(1000);
			continue;
		}
		rv = miiphy_read(M2S_MII_NAME, M88E1340_PHY_ADDR,
				M88E1340_PHY_STATUS, &val);
		if (rv != 0) {
			printf("mii err %d.\n", rv);
			goto out;
		}
		if ((val & M88E1340_PHY_STATUS_SPD_MASK) ==
				M88E1340_PHY_STATUS_100) {
			m2s_mii_speed = M2S_SYS_MAC_CR_LS_100;
		} else if ((val & M88E1340_PHY_STATUS_SPD_MASK) ==
				M88E1340_PHY_STATUS_1000) {
			m2s_mii_speed = M2S_SYS_MAC_CR_LS_1000;
		} else {
			m2s_mii_speed = M2S_SYS_MAC_CR_LS_10;
		}
		break;
	}

	if (timeout <= 0) {
		printf("%s %d %s marvel autoneg failed by timeout %02x\n",
				__FILE__, __LINE__, __func__, val);
		return -1;
	}

	rv = miiphy_read(M2S_MII_NAME, SF2_MSGMII_PHY_ADDR, PHY_BMSR, &val);
	if (rv != 0) {
		goto out;
	}

	if (val & PHY_BMSR_AUTN_COMP) {
		/* no need to start auto-negotiation if it is already done */
		goto out;
	}

	rv = miiphy_read(M2S_MII_NAME, SF2_MSGMII_PHY_ADDR, PHY_BMCR, &val);
	if (rv != 0) {
		goto out;
	}

	rv = miiphy_write(M2S_MII_NAME, SF2_MSGMII_PHY_ADDR, PHY_BMCR,
			val | PHY_BMCR_AUTON | PHY_BMCR_RST_NEG);
	if (rv != 0) {
		goto out;
	}

	/*
	 * Wait until auto-negotiation completes
	 */
	timeout = M2S_AUTONEG_TOUT/10;
	while (timeout--) {
		rv = miiphy_read(M2S_MII_NAME, SF2_MSGMII_PHY_ADDR,
				PHY_BMSR, &val);

		if (rv != 0) {
			printf("mii err %d.\n", rv);
			goto out;
		}
		if (!(val & PHY_BMSR_AUTN_COMP)) {
			if (timeout % 100 != 0) {
				udelay(10000);
				continue;
			}

			/* restart auto-negotiation if it is not complete
			   in a second */
			rv = miiphy_write(M2S_MII_NAME, SF2_MSGMII_PHY_ADDR,
					PHY_BMCR,
					PHY_BMCR_AUTON | PHY_BMCR_RST_NEG);
			if (rv != 0) {
				goto out;
			}

			continue;
		}
		break;
	}
	if (timeout <= 0)
		printf("MSGMII PHY auto-negotiaiton timed out!\n");
    
 out:
	return rv;


}

#endif

/******************************************************************************
 * Functions exported from the module
 ******************************************************************************/

/*
 * Initialize driver
 */
int m2s_eth_driver_init(bd_t *bd)
{
	int	rv;

	/*
	 * Register PHY driver
	 */
	miiphy_register(M2S_MII_NAME, m2s_mii_read, m2s_mii_write);

	/*
	 * Complete eth device initialization, and register device
	 */
	strncpy(m2s_eth_dev.name, M2S_MAC_NAME, NAMESIZE - 1);
	rv = eth_register(&m2s_eth_dev);
	if (rv != 0) {
		printf("%s: eth register err %d\n", __func__, rv);
		goto out;
	}
out:
	return rv;
}

/******************************************************************************
 * Standard U-Boot eth_device "API"
 ******************************************************************************/

/*
 * Init device
 */
static int m2s_eth_init(struct eth_device *dev, bd_t *bd_unused)
{
	volatile struct m2s_eth_dma_bd	*bd;
	int				i, rv = 0, timeout;

	/*
	 * Release the Ethernet MAC from reset
	 */
	M2S_SYSREG->soft_reset_cr &= ~M2S_SYS_SOFT_RST_CR_MAC;

	/*
	 * Set-up CR
	 */
	M2S_SYSREG->mac_cr &= ~(M2S_SYS_MAC_CR_PM_MSK << M2S_SYS_MAC_CR_PM_BIT);
	M2S_SYSREG->mac_cr &= ~(M2S_SYS_MAC_CR_LS_MSK << M2S_SYS_MAC_CR_LS_BIT);
#ifndef CONFIG_M2S_ETH_MODE_SGMII
	M2S_SYSREG->mac_cr |= M2S_SYS_MAC_CR_PM_MII << M2S_SYS_MAC_CR_PM_BIT;
#else
	/* Interface type: TBI */
	M2S_SYSREG->mac_cr = M2S_SYS_MAC_CR_PM_TBI << M2S_SYS_MAC_CR_PM_BIT;
#endif

	M2S_MAC_CFG->mii_config = 7;

	/*
	 * Reset all PE-MCXMAC modules, and configure
	 */
	M2S_MAC_CFG->cfg1 |= M2S_MAC_CFG1_RST;
	M2S_MAC_CFG->cfg1 &= ~M2S_MAC_CFG1_RST;
	/* Clear all reset bits */
	/* Clear MCXMAC TX reset */
	M2S_MAC_CFG->cfg1 &= ~M2S_MAC_CFG1_TX_RST;
	/* Clear MCXMAC RX reset */
	M2S_MAC_CFG->cfg1 &= ~M2S_MAC_CFG1_RX_RST;
	/* Clear MCXMAC TX reset */
	M2S_MAC_CFG->cfg1 &= ~M2S_MAC_CFG1_TXCTL_RST;
	/* Clear MCXMAC RX reset */
	M2S_MAC_CFG->cfg1 &= ~M2S_MAC_CFG1_RXCTL_RST;
	/* Clear MCXMAC interface reset */
	M2S_MAC_CFG->if_ctrl &= ~M2S_MAC_INTF_RESET;
	/* Clear FIFO watermark reset */
	/* Clear FIFO Rx system reset */
	/* Clear FIFO Rx fab reset */
	/* Clear FIFO Tx system reset */
	/* Clear FIFO Tx system reset */
	M2S_MAC_CFG->fifo_cfg[0] &= ~M2S_MAC_FIFO_CFG0_ALL_RST;

	M2S_MAC_CFG->cfg1 = 0;

	M2S_MAC_CFG->cfg2 &= ~(M2S_MAC_CFG2_MODE_MSK << M2S_MAC_CFG2_MODE_BIT);
#ifndef CONFIG_M2S_ETH_MODE_SGMII
	M2S_MAC_CFG->cfg2 |= (M2S_MAC_CFG2_MODE_MII << M2S_MAC_CFG2_MODE_BIT) |
			     M2S_MAC_CFG2_FULL_DUP | M2S_MAC_CFG2_PAD_CRC;
#else
	M2S_MAC_CFG->cfg2 =
		M2S_MAC_CFG2_FULL_DUP | M2S_MAC_CFG2_CRC_EN
		| M2S_MAC_CFG2_PAD_CRC
		| M2S_MAC_CFG2_LEN_CHECK
		| (M2S_MAC_CFG2_MODE_BYTE << M2S_MAC_CFG2_MODE_BIT)
		| (0x7 << M2S_MAC_CFG2_PREAM_LEN_BIT);
#endif
	M2S_MAC_CFG->max_frame_length = M2S_FRM_MAX_LEN;

	M2S_MAC_CFG->station_addr[0] = (dev->enetaddr[0] << 24) |
				       (dev->enetaddr[1] << 16) |
				       (dev->enetaddr[2] <<  8) |
				       (dev->enetaddr[3] <<  0);
	M2S_MAC_CFG->station_addr[1] = (dev->enetaddr[4] << 24) |
				       (dev->enetaddr[5] << 16);

	/*
	 * Init RX BDs: allocate bufs for incoming frames, link BDs to
	 * list, and mark all as empty. We don't specify buf sizes in cfg_size,
	 * 'cause these aren't used according to doc; guess MAC assumes size
	 * of buffer basing on max_frame_length register.
	 */
	for (i = 0; i < M2S_RX_BD_NUM; i++) {
		bd = &m2s_bd_rx[i];

		bd->frame = m2s_buf_rx[i];
		bd->cfg_size  = M2S_BD_EMPTY;
		bd->next = (void *)&m2s_bd_rx[(i + 1) % M2S_RX_BD_NUM];
	}

	/*
	 * Init TX BDs: link BDs to list, and mark all as empty
	 */
	for (i = 0; i < M2S_TX_BD_NUM; i++) {
		bd = &m2s_bd_tx[i];

		bd->frame = NULL;
		bd->cfg_size = M2S_BD_EMPTY;
		bd->next = (void *)&m2s_bd_tx[(i + 1) % M2S_TX_BD_NUM];
	}

	/*
	 * Init indexes, and program rx DMA
	 */
	m2s_bd_cur_rx = m2s_bd_cur_tx = 0;

	M2S_MAC_DMA->rx_desc = (u32)m2s_bd_rx;
	M2S_MAC_DMA->rx_ctrl = M2S_MAC_DMA_CTRL_ENA;

	/*
	 * Reset and enable FIFOs
	 */
	M2S_MAC_CFG->fifo_cfg[0] |= M2S_MAC_FIFO_CFG0_ALL_RST;
	M2S_MAC_CFG->fifo_cfg[0] &= ~M2S_MAC_FIFO_CFG0_ALL_RST;

	M2S_MAC_CFG->fifo_cfg[0] = M2S_MAC_FIFO_CFG0_ALL_REQ | 0xFF00;
	timeout = M2S_FIFO_TOUT;
	while (((M2S_MAC_CFG->fifo_cfg[0] & M2S_MAC_FIFO_CFG0_ALL_RPLY) !=
		M2S_MAC_FIFO_CFG0_ALL_RPLY) && timeout) {
		timeout--;
		udelay(1000);
	}
	if (!timeout) {
		printf("%s: FIFO initialization timeout\n", __func__);
		m2s_mac_dump_regs("FIFO init");
		rv = -ETIMEDOUT;
		goto out;
	}

#ifdef CONFIG_M2S_ETH_MODE_SGMII
	if (msgmii_phy_init() < 0)
		goto out;
	if (m881340_phy_set_link_speed() < 0)
		goto out;
	if (msgmii_phy_autonegotiate() < 0)
		goto out;
#else
	/*
	 * Probe for PHY, and get LINK status
	 */
	rv = m2s_phy_probe();
	if (rv != 0)
		goto out;
	if (rv != 0)
		goto out;
#endif

	if (m2s_mii_speed == M2S_SYS_MAC_CR_LS_100) {
		M2S_MAC_CFG->if_ctrl |= M2S_MAC_INTF_SPEED_100;
	} else {
		M2S_MAC_CFG->if_ctrl &= ~M2S_MAC_INTF_SPEED_100;
	}
	M2S_SYSREG->mac_cr = (M2S_SYSREG->mac_cr & ~M2S_SYS_MAC_CR_LS_MSK) |
		m2s_mii_speed;
	M2S_MAC_CFG->cfg2 |= M2S_MAC_CFG2_FULL_DUP;
	M2S_MAC_CFG->cfg2 &= ~(M2S_MAC_CFG2_MODE_MSK << M2S_MAC_CFG2_MODE_BIT);
	if (m2s_mii_speed == M2S_SYS_MAC_CR_LS_1000)
		M2S_MAC_CFG->cfg2 |= (M2S_MAC_CFG2_MODE_BYTE << M2S_MAC_CFG2_MODE_BIT);
	else
		M2S_MAC_CFG->cfg2 |= (M2S_MAC_CFG2_MODE_MII << M2S_MAC_CFG2_MODE_BIT);

	/*
	 * Enable MAC Rx and Tx
	 */
	M2S_MAC_CFG->cfg1 = M2S_MAC_CFG1_RX_ENA | M2S_MAC_CFG1_TX_ENA;

	rv = 0;
out:
	/*
	 * In case of error free allocated resources
	 */
	if (rv < 0)
		m2s_eth_halt(dev);

#if defined(DEBUG)
	m2s_mac_dump_regs("init done");
#endif

	return rv;
}

/*
 * Send frame
 */
static int m2s_eth_send(struct eth_device *dev, volatile void *pkt, int len)
{
	int	timeout, rv;

	if (len > M2S_FRM_MAX_LEN || len <= 0) {
		printf("%s: bad len %d\n", __func__, len);
		rv = -EINVAL;
		goto out;
	}

	/*
	 * Make sure that nothing is sending right now
	 */
	if (M2S_MAC_DMA->tx_ctrl & M2S_MAC_DMA_CTRL_ENA) {
		m2s_mac_dump_regs("tx busy");
		rv = -EBUSY;
		goto out;
	}

	m2s_bd_tx[m2s_bd_cur_tx].frame = pkt;
	m2s_bd_tx[m2s_bd_cur_tx].cfg_size = len;

	M2S_MAC_DMA->tx_desc = (u32)&m2s_bd_tx[m2s_bd_cur_tx];
	M2S_MAC_DMA->tx_ctrl = M2S_MAC_DMA_CTRL_ENA;

	/*
	 * Wait until xfer completes
	 */
	timeout = M2S_SEND_TOUT;
	while (!(m2s_bd_tx[m2s_bd_cur_tx].cfg_size & M2S_BD_EMPTY) && timeout) {
		timeout--;
		udelay(1000);
	}
	rv = timeout ? 0 : -ETIMEDOUT;
	if (rv < 0)
		m2s_mac_dump_regs("tx timeout");

	m2s_bd_cur_tx = (m2s_bd_cur_tx + 1) % M2S_TX_BD_NUM;
out:
	debug("%s: tx[%d] %s/%d\n", __func__, m2s_bd_cur_tx, rv ? "ERR" : "OK",
	      rv);

	return rv;
}

/*
 * Process received frames (if any)
 */
static int m2s_eth_recv(struct eth_device *dev)
{
	volatile struct m2s_eth_dma_bd	*bd;

	/*
	 * Walk through the list of rx bds, and process rxed frames until
	 * detect BD owned by DMA
	 */
	while (!(m2s_bd_rx[m2s_bd_cur_rx].cfg_size & M2S_BD_EMPTY)) {
		bd = &m2s_bd_rx[m2s_bd_cur_rx];

		/*
		 * Pass frame to the upper level
		 */
		debug("%s: rx[%d] %x\n", __func__, m2s_bd_cur_rx,
		      m2s_bd_rx[m2s_bd_cur_rx].cfg_size);
		NetReceive(bd->frame, bd->cfg_size & M2S_BD_SIZE_MSK);

		/*
		 * Update BD, and re-enable RX (for the case of overflow)
		 */
		bd->cfg_size = M2S_BD_EMPTY | PKTSIZE_ALIGN;

		/*
		 * Update our position
		 */
		m2s_bd_cur_rx = (m2s_bd_cur_rx + 1) % M2S_RX_BD_NUM;
	}

	/*
	 * In case of RX stopped (overrun, etc), and all rx packets
	 * processed - restart DMA
	 */
	if (!(M2S_MAC_DMA->rx_ctrl & M2S_MAC_DMA_CTRL_ENA) &&
	     (m2s_bd_rx[m2s_bd_cur_rx].cfg_size & M2S_BD_EMPTY)) {
		debug("%s: restart rx[%d] %x/%x/%x/%x\n", __func__,
		      m2s_bd_cur_rx,
		      M2S_MAC_DMA->rx_stat, M2S_MAC_DMA->rx_ctrl,
		      M2S_MAC_CFG->cfg1, M2S_MAC_CFG->cfg2);

		M2S_MAC_DMA->rx_desc = (u32)&m2s_bd_rx[m2s_bd_cur_rx];
		M2S_MAC_DMA->rx_ctrl = M2S_MAC_DMA_CTRL_ENA;
	}

	return 0;
}

/*
 * Halt MAC
 */
static void m2s_eth_halt(struct eth_device *dev)
{
	int	i;

	/*
	 * Put MAC to the reset state
	 */
	M2S_SYSREG->soft_reset_cr |= M2S_SYS_SOFT_RST_CR_MAC;

	/*
	 * Free resources
	 */
	for (i = 0; i < M2S_RX_BD_NUM; i++) {
		if (!m2s_bd_rx[i].frame)
			continue;
		m2s_bd_rx[i].frame = NULL;
	}
}

/******************************************************************************
 * Standard U-Boot miiphy "API"
 ******************************************************************************/

/*
 * Read register value via MII
 */
static int m2s_mii_read(char *devname, u8 adr, u8 reg, u16 *val)
{
	int	timeout, rv;

	if (adr > 31 || reg > 31 || !val) {
		printf("%s: bad params %x/%x/%p\n", __func__, adr, reg, val);
		rv = -EINVAL;
		goto out;
	}

	/*
	 * Set PHY & REG addresses, and issue read command.
	 * Note, we MUST reset 'READ' bit in cmd, otherwise, on each
	 * next read - the same data will be returned in mii_status
	 */
	M2S_MAC_CFG->mii_address = (adr << M2S_MAC_MII_ADR_PHY_BIT) |
				   (reg << M2S_MAC_MII_ADR_REG_BIT);
	M2S_MAC_CFG->mii_command = M2S_MAC_MII_CMD_READ;
	M2S_MAC_CFG->mii_command = 0;

	timeout = M2S_MII_TOUT;
	while ((M2S_MAC_CFG->mii_ind & (M2S_MAC_MII_IND_NVAL |
					M2S_MAC_MII_IND_BUSY)) && timeout) {
		timeout--;
		udelay(1000);
	}

	*val = M2S_MAC_CFG->mii_status;
	rv = timeout ? 0 : -ETIMEDOUT;
out:
	return rv;
}

/*
 * Write register value to MII
 */
static int m2s_mii_write(char *devname, u8 adr, u8 reg, u16 val)
{
	int	timeout, rv;

	if (adr > 31 || reg > 31) {
		printf("%s: bad params %x/%x\n", __func__, adr, reg);
		rv = -EINVAL;
		goto out;
	}

	/*
	 * Set PHY & REG addresses, then write data
	 */
	M2S_MAC_CFG->mii_address = (adr << M2S_MAC_MII_ADR_PHY_BIT) |
				   (reg << M2S_MAC_MII_ADR_REG_BIT);
	M2S_MAC_CFG->mii_ctrl = val;

	timeout = M2S_MII_TOUT;
	while ((M2S_MAC_CFG->mii_ind & M2S_MAC_MII_IND_BUSY) && timeout) {
		timeout--;
		udelay(1000);
	}

	rv = timeout ? 0 : -ETIMEDOUT;
out:
	return rv;
}

#ifndef CONFIG_M2S_ETH_MODE_SGMII

/******************************************************************************
 * PHY routines
 ******************************************************************************/

/*
 * Probe for PHY
 */
static int m2s_phy_probe(void)
{
	int	i, rv;

	/*
	 * Probe for PHY on different addresses
	 */
	for (i = 1; i < 32; i++) {
		u32	oui;
		u8	model, rev;

		rv = miiphy_info(M2S_MII_NAME, i, &oui, &model, &rev);
		if (rv != 0)
			continue;

		m2s_phy_addr = i;
		debug("%s: PHY 0x%x:0x%x:0x%x found at 0x%x\n", __func__,
			oui, model, rev, m2s_phy_addr);
		break;
	}
	if (m2s_phy_addr == 0xFF) {
		printf("%s: no PHY found\n", __func__);
		rv = -ENODEV;
		goto out;
	}

	rv = 0;
out:
	return rv;
}

#endif

/******************************************************************************
 * Debug stuff
 ******************************************************************************/

/*
 * Dump MAC registers
 */
static void m2s_mac_dump_regs(char *who)
{
	int	i;

	if (who)
		printf("*** %s %s:\n", __func__, who);

	printf(" DMA TX CTRL=%08x;DESC=%08x;STAT=%08x\n",
		M2S_MAC_DMA->tx_ctrl, M2S_MAC_DMA->tx_desc,
		M2S_MAC_DMA->tx_stat);
	printf(" DMA RX CTRL=%08x;DESC=%08x;STAT=%08x\n",
		M2S_MAC_DMA->rx_ctrl, M2S_MAC_DMA->rx_desc,
		M2S_MAC_DMA->rx_stat);
	printf(" DMA IRQ %08x/%08x\n", M2S_MAC_DMA->irq, M2S_MAC_DMA->irq_msk);
	printf(" CFG1=%08x;CFG2=%08x;IFG=%08x;HD=%08x;MFL=%08x\n",
		M2S_MAC_CFG->cfg1, M2S_MAC_CFG->cfg2,
		M2S_MAC_CFG->ifg, M2S_MAC_CFG->half_duplex,
		M2S_MAC_CFG->max_frame_length);
	printf(" IFCTRL=%08x;IFSTAT=%08x;ADR1=%08x;ADR2=%08x\n",
		M2S_MAC_CFG->if_ctrl, M2S_MAC_CFG->if_stat,
		M2S_MAC_CFG->station_addr[0], M2S_MAC_CFG->station_addr[1]);
	printf(" FIFO CFG ");
	for (i = 0; i < 6; i++)
		printf("%08x/", M2S_MAC_CFG->fifo_cfg[i]);
	printf("\n");

	printf(" FIFO ACC ");
	for (i = 0; i < 8; i++)
		printf("%08x/", M2S_MAC_CFG->fifo_ram_access[i]);
	printf("\n");
}

