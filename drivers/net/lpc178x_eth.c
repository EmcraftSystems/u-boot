/*
 * (C) Copyright 2011
 *
 * Alexander Potashev, Emcraft Systems, aspotashev@emcraft.com
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
 * LPC178x/7x Ethernet driver
 */

#include <config.h>

/*
 * Define DEBUG to enable debug() messages in this module
 */
#undef DEBUG

#include <common.h>
#include <malloc.h>
#include <net.h>
#include <miiphy.h>
#include <asm/errno.h>

#if !defined(CONFIG_SYS_RX_ETH_BUFFER)
#error CONFIG_SYS_RX_ETH_BUFFER should be set
#elif CONFIG_SYS_RX_ETH_BUFFER < 2
#error The LPC178x/7x DMA controller cannot work with less than 2 buffers
#endif

#if !defined(CONFIG_SYS_TX_ETH_BUFFER)
#error CONFIG_SYS_TX_ETH_BUFFER should be set
#elif CONFIG_SYS_TX_ETH_BUFFER < 2
#error The LPC178x/7x DMA controller cannot work with less than 2 buffers
#endif

/*
 * Device name
 */
#define LPC178X_MAC_NAME		"LPC178X_MAC"

#define LPC178X_ENET_RETR_MAX		0xF
#define LPC178X_ENET_COLL_WND		0x37

#define LPC178X_ENET_BB_GAP_FULL	0x15
#define LPC178X_ENET_BB_GAP_HALF	0x12

#define LPC178X_ENET_GAP2		0x12
/* FIXME: lpcware port suggests 0, but the User Manual recommends 0xC */
#define LPC178X_ENET_GAP1		0

#define LPC178X_PHY_READ_TIMEOUT	2500	/* x 100 usec = 250 ms */
#define LPC178X_PHY_WRITE_TIMEOUT	2500	/* x 100 usec = 250 ms */
#define LPC178X_PHY_AUTONEG_TIMEOUT	100000	/* x 100 usec = 10 s */
#define LPC178X_PHY_RESET_TIMEOUT	4000	/* x 100 usec = 400 ms */

#define LPC178X_MAC_TX_TIMEOUT		1000000	/* x 1 usec = 1000 ms */

/*
 * MII Mgmt Command Register
 */
#define LPC178X_ETH_MCMD_READ_MSK	(1 << 0)

/*
 * MII Mgmt Indicators Register
 */
/*
 * When "1" is returned - indicates MII Mgmt is currently performing
 * an MII Mgmt Read or Write cycle.
 */
#define LPC178X_ETH_MIND_BUSY_MSK	(1 << 0)

/*
 * MII Mgmt Address Register
 */
/* This field represents the 5-bit Register Address field of Mgmt cycles */
#define LPC178X_ETH_MADR_RA_BITS	0
/* This field represents the 5-bit PHY Address field of Mgmt cycles */
#define LPC178X_ETH_MADR_PA_BITS	8

/*
 * MII Mgmt Configuration Register
 */
/*
 * This field is used by the clock divide logic in creating
 * the MII Management Clock
 */
#define LPC178X_ETH_MCFG_CS_BITS	2

/*
 * MAC Configuration Register 1
 */
/* RECEIVE ENABLE */
#define LPC178X_ETH_MAC1_RX_EN_MSK		(1 << 0)
/* PASS ALL RECEIVE FRAMES */
#define LPC178X_ETH_MAC1_PASS_ALL_RX_MSK	(1 << 1)
/* RESET TX */
#define LPC178X_ETH_MAC1_RST_TX_MSK		(1 << 8)
/* RESET MCS / TX */
#define LPC178X_ETH_MAC1_RST_MCSTX_MSK		(1 << 9)
/* RESET RX */
#define LPC178X_ETH_MAC1_RST_RX_MSK		(1 << 10)
/* RESET MCS / RX */
#define LPC178X_ETH_MAC1_RST_MCSRX_MSK		(1 << 11)
/* SIMULATION RESET */
#define LPC178X_ETH_MAC1_RST_SIM_MSK		(1 << 14)
/* SOFT RESET */
#define LPC178X_ETH_MAC1_RST_SOFT_MSK		(1 << 15)

/*
 * MAC Configuration Register 2
 */
/* FULL-DUPLEX */
#define LPC178X_ETH_MAC2_FULL_DUPLEX_MSK	(1 << 0)
/* CRC ENABLE */
#define LPC178X_ETH_MAC2_CRC_EN_MSK		(1 << 4)
/* PAD / CRC ENABLE */
#define LPC178X_ETH_MAC2_PAD_CRC_EN_MSK		(1 << 5)

/*
 * Command Register
 */
/* Enable receive */
#define LPC178X_ETH_COMMAND_RX_EN_MSK		(1 << 0)
/* Enable transmit */
#define LPC178X_ETH_COMMAND_TX_EN_MSK		(1 << 1)
/* RegReset */
#define LPC178X_ETH_COMMAND_REG_RST_MSK		(1 << 3)
/* TxReset */
#define LPC178X_ETH_COMMAND_TX_RST_MSK		(1 << 4)
/* RxReset */
#define LPC178X_ETH_COMMAND_RX_RST_MSK		(1 << 5)
/* PassRuntFrame */
#define LPC178X_ETH_COMMAND_PASS_RUNT_MSK	(1 << 6)
/* RMII */
#define LPC178X_ETH_COMMAND_RMII		(1 << 9)
/* FullDuplex */
#define LPC178X_ETH_COMMAND_FULL_DUPLEX_MSK	(1 << 10)

/*
 * Receive Filter Control register
 */
/* All broadcast frames are accepted */
#define LPC178X_ETH_RXFILTCTRL_ACC_BROADCAST_MSK	(1 << 1)
/*
 * The frames with a destination address identical to the station address
 * are accepted
 */
#define LPC178X_ETH_RXFILTCTRL_ACC_PERFECT_MSK		(1 << 5)

/*
 * PHY Support Register
 */
/* 1 = 100 Mbps; 0 = 10 Mbps */
#define LPC178X_ETH_SUPP_SPEED_MSK		(1 << 8)

/*
 * Collision Window / Retry Register
 */
/* RETRANSMISSION MAXIMUM */
#define LPC178X_ETH_CLRT_RETR_MAX_BITS	0
/* COLLISION WINDOW */
#define LPC178X_ETH_CLRT_COLL_WND_BITS	8

/*
 * Back-to-Back Inter-Packet-Gap Register
 */
/* BACK-TO-BACK INTER-PACKET-GAP */
#define LPC178X_ETH_IPGT_GAP_BITS	0

/*
 * Non Back-to-Back Inter-Packet-Gap Register
 */
/* NON-BACK-TO-BACK INTER-PACKET-GAP PART2 */
#define LPC178X_ETH_IPGR_GAP2_BITS	0
/* NON-BACK-TO-BACK INTER-PACKET-GAP PART1 */
#define LPC178X_ETH_IPGR_GAP1_BITS	8

/*
 * Receive descriptor control word
 */
/*
 * If true generate an RxDone interrupt when the data in this frame or frame
 * fragment and the associated status information has been committed to
 * memory.
 */
#define LPC178X_DMA_RBD_INT_MSK		(1 << 31)
/* Size in bytes of the data buffer */
#define LPC178X_DMA_RBD_SIZE_BITS	0

/*
 * Receive status information word
 */
/* RxSize mask */
#define LPC178X_DMA_RBS_SIZE_MSK	((1 << 11) - 1)
/* RxSize bit offset */
#define LPC178X_DMA_RBS_SIZE_BITS	0

/*
 * Transmit descriptor control word
 */
/*
 * If true, bits 30:27 will override the defaults from the MAC internal
 * registers
 */
#define LPC178X_DMA_TBD_OVERRIDE_MSK	(1 << 26)
/* If true, pad short frames to 64 bytes. */
#define LPC178X_DMA_TBD_PAD_MSK		(1 << 28)
/* If true, append a hardware CRC to the frame. */
#define LPC178X_DMA_TBD_CRC_MSK		(1 << 29)
/*
 * If true, indicates that this is the descriptor for the last fragment in the
 * transmit frame. If false, the fragment from the next descriptor should be
 * appended.
 */
#define LPC178X_DMA_TBD_LAST_MSK	(1 << 30)
/* TxSize bit offset */
#define LPC178X_DMA_TBD_SIZE_BITS	0

/*
 * Ethernet register map
 */
struct lpc178x_eth_regs {
	/* 0x20084000: MAC registers */
	u32 mac1;	/* MAC configuration register 1 */
	u32 mac2;	/* MAC configuration register 2 */
	u32 ipgt;	/* Back-to-Back Inter-Packet-Gap register */
	u32 ipgr;	/* Non Back-to-Back Inter-Packet-Gap register */
	u32 clrt;	/* Collision window / Retry register */
	u32 maxf;	/* Maximum Frame register */
	u32 supp;	/* PHY Support register */
	u32 test;	/* Test register */
	u32 mcfg;	/* MII Mgmt Configuration register */
	u32 mcmd;	/* MII Mgmt Command register */
	u32 madr;	/* MII Mgmt Address register */
	u32 mwtd;	/* MII Mgmt Write Data register */
	u32 mrdd;	/* MII Mgmt Read Data register */
	u32 mind;	/* MII Mgmt Indicators register */
	u32 rsv0[2];
	u32 sa0;	/* Station Address 0 register */
	u32 sa1;	/* Station Address 1 register */
	u32 sa2;	/* Station Address 2 register */

	/* 0x2008404C */
	u32 rsv1[45];

	/* 0x20084100: Control registers */
	u32 command;	/* Command Register */
	u32 status;	/* Status Register */

	/* Receive Descriptor Base Address Register */
	volatile struct lpc178x_eth_dma_bd *rxdesc;
	/* Receive Status Base Address Register */
	volatile struct lpc178x_eth_dma_rx_status *rxstatus;
	u32 rxdescnum;	/* Receive Number of Descriptors Register */
	u32 rxprodidx;	/* Receive Produce Index Register */
	u32 rxconsidx;	/* Receive Consume Index Register */

	/* Transmit Descriptor Base Address Register */
	volatile struct lpc178x_eth_dma_bd *txdesc;
	/* Transmit Status Base Address Register */
	volatile struct lpc178x_eth_dma_tx_status *txstatus;
	u32 txdescnum;	/* Transmit Number of Descriptors Register */
	u32 txprodidx;	/* Transmit Produce Index Register */
	u32 txconsidx;	/* Transmit Consume Index Register */

	/* 0x20084130 */
	u32 rsv2[52];

	/* 0x20084200: Receive filter registers */
	u32 rxfiltctrl;	/* Receive Filter Control register */
};

#define LPC178X_ETH_BASE		(LPC178X_AHB_PERIPH_BASE + 0x00004000)
#define LPC178X_ETH			((volatile struct lpc178x_eth_regs *) \
					LPC178X_ETH_BASE)

/*
 * LPC178x/7x ETH DMA Receive/Transmit descriptor
 */
struct lpc178x_eth_dma_bd {
	volatile u8 *packet;	/* Pointer to buffer */
	u32 control;		/* Control, and buffer length */
};

/*
 * Transmit status fields
 */
struct lpc178x_eth_dma_tx_status {
	u32 info;	/* Status bits */
};

/*
 * Receive Status Fields
 */
struct lpc178x_eth_dma_rx_status {
	u32 info;	/* Status bits */
	u32 hash_crc;	/* Source address and destination address CRCs */
};

struct lpc178x_eth_dma_data {
	/*
	 * DMA buffer descriptors, and index of last processed buf:
	 * - have CONFIG_SYS_TX_ETH_BUFFER Tx buffer descriptors;
	 * - have CONFIG_SYS_RX_ETH_BUFFER Rx buffer descriptors.
	 */
	struct lpc178x_eth_dma_bd tx_bd[CONFIG_SYS_TX_ETH_BUFFER];
	struct lpc178x_eth_dma_bd rx_bd[CONFIG_SYS_RX_ETH_BUFFER];

	/*
	 * DMA status arrays
	 *
	 * Receive status array must be aligned on a 8 byte (64bit)
	 * address boundary.
	 */
	struct lpc178x_eth_dma_tx_status tx_status[CONFIG_SYS_TX_ETH_BUFFER];
	struct lpc178x_eth_dma_rx_status rx_status[CONFIG_SYS_RX_ETH_BUFFER]
		__attribute__ ((aligned (8)));

	/*
	 * ETH DMAed buffers have length of 1536B (> max eth frm len)
	 */
	u8 tx_buf[CONFIG_SYS_TX_ETH_BUFFER][PKTSIZE_ALIGN];
	u8 rx_buf[CONFIG_SYS_RX_ETH_BUFFER][PKTSIZE_ALIGN];
};

/*
 * LPC178X ETH device
 */
struct lpc178x_eth_dev {
	/*
	 * Standard ethernet device
	 */
	struct eth_device netdev;

	/* 32-bit ID combined from PHY ID 1 and PHY ID 2 registers */
	u32 phy_id;
	/* 5-bit PHY address */
	u32 phy_adr;

	volatile struct lpc178x_eth_dma_data *dma;
};
#define to_lpc178x_eth(_nd)	container_of(_nd, struct lpc178x_eth_dev, netdev)

/*
 * Prototypes
 */
static  int lpc178x_eth_init(struct eth_device *dev, bd_t *bd);
static  int lpc178x_eth_send(struct eth_device *dev, volatile void *pkt, int len);
static  int lpc178x_eth_recv(struct eth_device *dev);
static void lpc178x_eth_halt(struct eth_device *dev);

static  int lpc178x_phy_write(struct lpc178x_eth_dev *mac, u16 reg, u16 val);
static  int lpc178x_phy_read(struct lpc178x_eth_dev *mac, u16 reg, u16 *val);

/*
 * Initialize driver
 */
int lpc178x_eth_driver_init(bd_t *bd)
{
	struct lpc178x_eth_dev *mac;
	struct eth_device *netdev;
	int rv;

	mac = malloc(sizeof(struct lpc178x_eth_dev));
	if (!mac) {
		printf("Error: failed to allocate %dB of memory for %s\n",
			sizeof(struct lpc178x_eth_dev), LPC178X_MAC_NAME);
		rv = -ENOMEM;
		goto out;
	}
	memset(mac, 0, sizeof(struct lpc178x_eth_dev));

	netdev = &mac->netdev;

	/*
	 * Autodetect PHY
	 */
	mac->phy_adr = 0;
	mac->phy_id = 0xFF;

	mac->dma = (volatile struct lpc178x_eth_dma_data *)CONFIG_MEM_ETH_DMA_BUF_BASE;

	sprintf(netdev->name, LPC178X_MAC_NAME);

	netdev->init = lpc178x_eth_init;
	netdev->halt = lpc178x_eth_halt;
	netdev->send = lpc178x_eth_send;
	netdev->recv = lpc178x_eth_recv;

	rv = eth_register(netdev);
out:
	if (rv != 0 && mac)
		free(mac);

	return rv;
}

/*
 * Initialize PHY: autodetect PHY address
 *
 */
static int lpc178x_phy_init(struct lpc178x_eth_dev *mac)
{
	int i, rv;
	u16 val;

	/*
	 * Check if we already inited
	 */
	if (mac->phy_id != 0xFF)
		goto ok;

	/*
	 * Probe (find) a PHY
	 */
	for (i = 0; i < 32; i++) {
		mac->phy_adr = i;
		rv = lpc178x_phy_read(mac, PHY_PHYIDR1, &val);
		if (rv != 0 || val == 0xFFFF || val == 0)
			continue;

		mac->phy_id = (val & 0xFFFF) << 16;
		rv = lpc178x_phy_read(mac, PHY_PHYIDR2, &val);
		if (rv == 0 && val != 0xFFFF && val != 0)
			mac->phy_id |= val & 0xFFFF;
		break;
	}
	if (i == 32) {
		mac->phy_id = 0xFF;
		printf("%s: PHY not found.\n", __func__);
		rv = -ENODEV;
		goto out;
	}

ok:
	debug("%s: found PHY id = %#x at addr %#x\n", __func__,
	      mac->phy_id, mac->phy_adr);
	rv = 0;
out:
	return rv;
}

/*
 * Get link status
 */
static int lpc178x_phy_link_get(
	struct lpc178x_eth_dev *mac, int *link_up, int *full_dup, int *speed)
{
	u16 bmcr, bmsr;	/* Basic Control and Status registers */
	u16 anar;	/* Auto Negotiation Advertisement */
	u16 anlpar;	/* Auto Negotiation Link Partner Ability */
	int rv;

	rv = lpc178x_phy_read(mac, PHY_BMCR, &bmcr);
	if (rv != 0)
		goto out;
	rv = lpc178x_phy_read(mac, PHY_BMSR, &bmsr);
	if (rv != 0)
		goto out;

	/*
	 * If autonegotiation is possible and enabled
	 */
	if ((bmsr & PHY_BMSR_AUTN_ABLE) && (bmcr & PHY_BMCR_AUTON)) {
		if ((bmsr & PHY_BMSR_AUTN_COMP) == 0) {
			/* Auto-negotiation is in progress */
			*link_up = 0;
			goto out;
		}

		if (bmsr & PHY_BMSR_LS)
			*link_up = 1;

		/*
		 * Auto-negotiation complete.
		 */
		/*
		 * Read Auto Negotiation Advertisement
		 */
		rv = lpc178x_phy_read(mac, PHY_ANAR, &anar);
		if (rv != 0)
			goto out;

		/*
		 * AND with Link Partner Ability
		 */
		rv = lpc178x_phy_read(mac, PHY_ANLPAR, &anlpar);
		if (rv != 0)
			goto out;
		anar &= anlpar;

		/*
		 * Determine the link mode
		 */
		*speed = 10;
		*full_dup = 0;
		if (anar & (PHY_ANLPAR_TXFD | PHY_ANLPAR_TX)) {
			*speed = 100;

			if (anar & PHY_ANLPAR_TXFD)
				*full_dup = 1;
		} else if (anar & PHY_ANLPAR_10FD) {
			*full_dup = 1;
		}
	} else {
		/*
		 * Auto-negotiation disabled
		 */
		*link_up = (bmsr & PHY_BMSR_LS) ? 1 : 0;

		*speed = (bmcr & PHY_BMCR_100_MBPS) ? 100 : 10;
		*full_dup = (bmcr & PHY_BMCR_DPLX) ? 1 : 0;
	}

	rv = 0;
out:
	return rv;
}

/*
 * Setup link status
 */
static int lpc178x_phy_link_setup(struct lpc178x_eth_dev *mac)
{
	u16 bmsr;
	int rv, timeout;
	int link_up, full_dup, speed;

	rv = lpc178x_phy_read(mac, PHY_BMSR, &bmsr);
	if (rv != 0)
		goto out;

	if (bmsr & PHY_BMSR_AUTN_ABLE) {
		printf("Auto-negotiation...");

		/*
		 * Enable auto-negotiation. Force negotiation reset.
		 */
		rv = lpc178x_phy_write(mac, PHY_BMCR, PHY_BMCR_AUTON);
		if (rv != 0)
			goto out;

		/*
		 * Wait for auto-negotiation to finish
		 */
		timeout = LPC178X_PHY_AUTONEG_TIMEOUT;
		while (timeout-- > 0 && !(bmsr & PHY_BMSR_AUTN_COMP)) {
			udelay(100);
			rv = lpc178x_phy_read(mac, PHY_BMSR, &bmsr);
			if (rv != 0)
				goto out;
		}

		if (bmsr & PHY_BMSR_AUTN_COMP)
			printf("completed.\n");
		else
			printf("timeout.\n");
	}

	/*
	 * Read the results of auto-negotiation
	 */
	rv = lpc178x_phy_link_get(mac, &link_up, &full_dup, &speed);
	if (rv != 0)
		goto out;
	if (!link_up) {
		printf("Link is DOWN.\n");
		rv = -ENETUNREACH;
		goto out;
	}

	printf("%s: link UP ", mac->netdev.name);

	/*
	 * Configure 100MBit/10MBit mode
	 */
	if (speed == 100) {	/* 100MBase mode */
		printf("(100/");
		LPC178X_ETH->supp = LPC178X_ETH_SUPP_SPEED_MSK;
	} else {		/* 10MBase mode */
		printf("(10/");
		LPC178X_ETH->supp = 0;
	}

	/*
	 * Configure Full/Half Duplex mode
	 */
	if (full_dup == 1) {
		printf("Full)\n");

		LPC178X_ETH->mac2 |= LPC178X_ETH_MAC2_FULL_DUPLEX_MSK;
		LPC178X_ETH->command |= LPC178X_ETH_COMMAND_FULL_DUPLEX_MSK;
		LPC178X_ETH->ipgt = (LPC178X_ENET_BB_GAP_FULL <<
			LPC178X_ETH_IPGT_GAP_BITS);
	} else {
		printf("Half)\n");

		LPC178X_ETH->ipgt = (LPC178X_ENET_BB_GAP_HALF <<
			LPC178X_ETH_IPGT_GAP_BITS);
	}

	rv = 0;
out:
	return rv;
}

/*
 * Helper function used in lpc178x_phy_read() and lpc178x_phy_write()
 */
static int lpc178x_phy_wait_busy(int timeout)
{
	int rv;

	rv = -ETIMEDOUT;
	while (timeout-- > 0) {
		if (LPC178X_ETH->mind & LPC178X_ETH_MIND_BUSY_MSK) {
			udelay(100);
		} else {
			timeout = 0;
			rv = 0;
		}
	}

	if (rv != 0)
		printf("lpc178x_phy_wait_busy: timed out\n");

	return rv;
}

/*
 * This function will be called from `lpc178x_phy_final_reset()` which resides
 * in `.ramcode`.
 * If the compiler makes this funtion `inline`, nothing will be broken, because
 * `lpc178x_phy_final_reset()` cannot become `inline` (it is not `static`.)
 */
static void __attribute__((section(".ramcode")))
	    __attribute__ ((long_call))
lpc178x_phy_write_nowait(u32 phy_adr, u16 reg, u16 val)
{
	LPC178X_ETH->mcmd = 0;
	LPC178X_ETH->madr = (phy_adr << LPC178X_ETH_MADR_PA_BITS) |
		(reg << LPC178X_ETH_MADR_RA_BITS);
	LPC178X_ETH->mwtd = val;
}

/*
 * Final PHY reset before performing SYSRESET of SoC
 *
 * If we do not reset the Ethernet PHY immediately before resetting the SoC,
 * the Ethernet block of the SoC will hang later and will not allow us to use
 * Ethernet after SoC reset.
 *
 * This function will be called from `lpc178x_pre_reset_cpu()` which resides
 * in `.ramcode`.
 */
void __attribute__((section(".ramcode")))
     __attribute__ ((long_call))
lpc178x_phy_final_reset(void)
{
	/*
	 * Enable power on the Ethernet block
	 */
	lpc178x_periph_enable(LPC178X_SCC_PCONP_PCENET_MSK, 1);

	/*
	 * Minimal MAC initialization
	 */
	LPC178X_ETH->mcfg = (CONFIG_LPC178X_ETH_DIV_SEL << LPC178X_ETH_MCFG_CS_BITS);
	LPC178X_ETH->mac1 = 0;
	LPC178X_ETH->mac2 = 0;

	/*
	 * Reset PHY and wait for write completion
	 */
	lpc178x_phy_write_nowait(CONFIG_LPC178X_ETH_PHY_ADDR, PHY_BMCR, PHY_BMCR_RESET);
	while (LPC178X_ETH->mind & LPC178X_ETH_MIND_BUSY_MSK);

	/*
	 * Disable power on the Ethernet block
	 */
	lpc178x_periph_enable(LPC178X_SCC_PCONP_PCENET_MSK, 0);
}

/*
 * Write PHY
 */
static int lpc178x_phy_write(struct lpc178x_eth_dev *mac, u16 reg, u16 val)
{
	lpc178x_phy_write_nowait(mac->phy_adr, reg, val);

	return lpc178x_phy_wait_busy(LPC178X_PHY_WRITE_TIMEOUT);
}

/*
 * Read PHY
 */
static int lpc178x_phy_read(struct lpc178x_eth_dev *mac, u16 reg, u16 *val)
{
	int rv;

	LPC178X_ETH->mcmd = LPC178X_ETH_MCMD_READ_MSK;
	LPC178X_ETH->madr = (mac->phy_adr << LPC178X_ETH_MADR_PA_BITS) |
		(reg << LPC178X_ETH_MADR_RA_BITS);
	rv = lpc178x_phy_wait_busy(LPC178X_PHY_READ_TIMEOUT);
	LPC178X_ETH->mcmd = 0;

	if (rv == 0)
		*val = LPC178X_ETH->mrdd;

	return rv;
}

/*
 * Reset PHY
 */
static int lpc178x_phy_reset(struct lpc178x_eth_dev *mac)
{
	int rv, read_rv, timeout;
	u16 bmcr;

	/* Reset the PHY and wait for reset to complete */
	rv = lpc178x_phy_write(mac, PHY_BMCR, PHY_BMCR_RESET);
	if (rv != 0)
		goto out;

	/*
	 * Wait for reset to complete
	 */
	timeout = LPC178X_PHY_RESET_TIMEOUT;
	rv = -ETIMEDOUT;
	while (timeout-- > 0) {
		read_rv = lpc178x_phy_read(mac, PHY_BMCR, &bmcr);
		if (read_rv != 0 || (bmcr & PHY_BMCR_RESET) != 0) {
			udelay(100);
		} else {
			timeout = 0;
			rv = 0;
		}
	}

out:
	if (rv != 0)
		printf("lpc178x_phy_reset: timed out\n");
	return rv;
}

/*
 * Init LPC178x/7x MAC buffer descriptors
 */
static void lpc178x_mac_bd_init(struct lpc178x_eth_dev *mac)
{
	int i;

	/*
	 * Setup pointers to TX structures
	 */
	LPC178X_ETH->txdesc = &mac->dma->tx_bd[0];
	LPC178X_ETH->txstatus = &mac->dma->tx_status[0];
	/* The TxDescriptorNumber is minus one encoded */
	LPC178X_ETH->txdescnum = CONFIG_SYS_TX_ETH_BUFFER - 1;

	for (i = 0; i < CONFIG_SYS_TX_ETH_BUFFER; i++) {
		mac->dma->tx_bd[i].packet = &mac->dma->tx_buf[i][0];
		mac->dma->tx_bd[i].control = 0;
	}

	/*
	 * Setup pointers to RX structures
	 */
	LPC178X_ETH->rxdesc = &mac->dma->rx_bd[0];
	LPC178X_ETH->rxstatus = &mac->dma->rx_status[0];
	/* The TxDescriptorNumber is minus one encoded */
	LPC178X_ETH->rxdescnum = CONFIG_SYS_RX_ETH_BUFFER - 1;

	for (i = 0; i < CONFIG_SYS_RX_ETH_BUFFER; i++) {
		mac->dma->rx_bd[i].packet = &mac->dma->rx_buf[i][0];
		mac->dma->rx_bd[i].control =
			((PKTSIZE_ALIGN - 1) << LPC178X_DMA_RBD_SIZE_BITS);
	}
}

/*
 * Set MAC address
 */
static void lpc178x_mac_address_set(struct lpc178x_eth_dev *mac)
{
	struct eth_device *netdev = &mac->netdev;

	debug("%s: mac is %#x:%#x:%#x:%#x:%#x:%#x.\n", __func__,
	      netdev->enetaddr[0], netdev->enetaddr[1],
	      netdev->enetaddr[2], netdev->enetaddr[3],
	      netdev->enetaddr[4], netdev->enetaddr[5]);

	LPC178X_ETH->sa2 = netdev->enetaddr[0] | (netdev->enetaddr[1] << 8);
	LPC178X_ETH->sa1 = netdev->enetaddr[2] | (netdev->enetaddr[3] << 8);
	LPC178X_ETH->sa0 = netdev->enetaddr[4] | (netdev->enetaddr[5] << 8);
}

/*
 * Reset all MAC logic
 */
static void lpc178x_mac_reset(void)
{
	LPC178X_ETH->mac1 =
		LPC178X_ETH_MAC1_RST_TX_MSK |
		LPC178X_ETH_MAC1_RST_MCSTX_MSK |
		LPC178X_ETH_MAC1_RST_RX_MSK |
		LPC178X_ETH_MAC1_RST_MCSRX_MSK |
		LPC178X_ETH_MAC1_RST_SIM_MSK |
		LPC178X_ETH_MAC1_RST_SOFT_MSK;
	LPC178X_ETH->command =
		LPC178X_ETH_COMMAND_REG_RST_MSK |
		LPC178X_ETH_COMMAND_TX_RST_MSK |
		LPC178X_ETH_COMMAND_RX_RST_MSK;
}

/*
 * Init LPC178x/7x MAC hardware
 */
static int lpc178x_mac_hw_init(struct lpc178x_eth_dev *mac)
{
	static int phy_reset_done;
	int rv;

	/*
	 * Enable power on the Ethernet block
	 */
	lpc178x_periph_enable(LPC178X_SCC_PCONP_PCENET_MSK, 1);

	/*
	 * Set RMII management clock rate
	 */
	LPC178X_ETH->mcfg = (CONFIG_LPC178X_ETH_DIV_SEL << LPC178X_ETH_MCFG_CS_BITS);

	/*
	 * Reset all MAC logic
	 */
	lpc178x_mac_reset();

	/*
	 * Initial MAC initialization
	 */
	LPC178X_ETH->mac1 = LPC178X_ETH_MAC1_PASS_ALL_RX_MSK;
	LPC178X_ETH->mac2 =
		LPC178X_ETH_MAC2_CRC_EN_MSK |
		LPC178X_ETH_MAC2_PAD_CRC_EN_MSK;
	LPC178X_ETH->maxf = PKTSIZE_ALIGN;

	/*
	 * Maximum number of retries, 0x37 collision window, gap
	 */
	LPC178X_ETH->clrt =
		(LPC178X_ENET_RETR_MAX << LPC178X_ETH_CLRT_RETR_MAX_BITS) |
		(LPC178X_ENET_COLL_WND << LPC178X_ETH_CLRT_COLL_WND_BITS);
	LPC178X_ETH->ipgr =
		(LPC178X_ENET_GAP2 << LPC178X_ETH_IPGR_GAP2_BITS) |
		(LPC178X_ENET_GAP1 << LPC178X_ETH_IPGR_GAP1_BITS);

#ifdef CONFIG_LPC178X_ENET_USE_PHY_RMII
	/* RMII setup */
	LPC178X_ETH->command =
		LPC178X_ETH_COMMAND_PASS_RUNT_MSK |
		LPC178X_ETH_COMMAND_RMII;
	LPC178X_ETH->supp = 0; /* 10 Mbps */
#else
	/* MII setup */
	LPC178X_ETH->command = LPC178X_ETH_COMMAND_PASS_RUNT_MSK;
#endif

	/*
	 * Init PHY
	 */
	rv = lpc178x_phy_init(mac);
	if (rv != 0)
		goto out;

	/*
	 * Reset PHY
	 */
	if (!phy_reset_done) {
		(void)lpc178x_phy_reset(mac);
		phy_reset_done = 1;
	}

	/*
	 * Setup link, and complete MAC initialization
	 */
	rv = lpc178x_phy_link_setup(mac);

out:
	return rv;
}

/*
 * Init LPC178x/7x MAC and DMA
 */
static int lpc178x_eth_init(struct eth_device *dev, bd_t *bd)
{
	struct lpc178x_eth_dev *mac = to_lpc178x_eth(dev);
	int rv;

	/*
	 * Init hw
	 */
	rv = lpc178x_mac_hw_init(mac);
	if (rv != 0)
		goto out;

	/*
	 * Set MAC address
	 */
	lpc178x_mac_address_set(mac);

	/*
	 * Init buffer descriptors
	 */
	lpc178x_mac_bd_init(mac);

	/*
	 * Enable broadcast and matching address packets
	 */
	LPC178X_ETH->rxfiltctrl =
		LPC178X_ETH_RXFILTCTRL_ACC_BROADCAST_MSK |
		LPC178X_ETH_RXFILTCTRL_ACC_PERFECT_MSK;

	/*
	 * Enable receive and transmit mode of MAC ethernet core
	 */
	LPC178X_ETH->command |=
		LPC178X_ETH_COMMAND_RX_EN_MSK | LPC178X_ETH_COMMAND_TX_EN_MSK;
	LPC178X_ETH->mac1 |= LPC178X_ETH_MAC1_RX_EN_MSK;

out:
	if (rv != 0)
		printf("%s: failed (%d).\n", __func__, rv);
	return rv;
}

static void memcpy_volatile(
	volatile void *dest, const volatile void *src, unsigned int count)
{
	volatile char *d = (volatile char *)dest, *s = (volatile char *)src;
	while (count--)
		*d++ = *s++;
}

/*
 * Send frame
 */
static int lpc178x_eth_send(struct eth_device *dev, volatile void *pkt, int len)
{
	struct lpc178x_eth_dev *mac = to_lpc178x_eth(dev);
	int rv, timeout;
	u32 idx, free_bufs;

	if (len > PKTSIZE_ALIGN) {
		printf("%s: frame too long (%d).\n", __func__, len);
		rv = -EINVAL;
		goto out;
	}

	/* Wait for a free buffer */
	idx = LPC178X_ETH->txprodidx;
	timeout = LPC178X_MAC_TX_TIMEOUT;
	rv = -ETIMEDOUT;
	while (timeout-- > 0) {
		free_bufs = (CONFIG_SYS_TX_ETH_BUFFER +
			LPC178X_ETH->txconsidx - idx - 1) %
			CONFIG_SYS_TX_ETH_BUFFER;

		if (free_bufs <= 1) /* full or almost full */
			udelay(1);
		else {
			timeout = 0;
			rv = 0;
		}
	}
	if (rv != 0) {
		printf("%s: timeout.\n", __func__);
		goto out;
	}

	/* Update descriptor with new frame size */
	mac->dma->tx_bd[idx].control =
		LPC178X_DMA_TBD_OVERRIDE_MSK | LPC178X_DMA_TBD_PAD_MSK |
		LPC178X_DMA_TBD_CRC_MSK | LPC178X_DMA_TBD_LAST_MSK |
		((len - 1) << LPC178X_DMA_TBD_SIZE_BITS);

	/* Move data to buffer */
	memcpy_volatile(&mac->dma->tx_buf[idx][0], pkt, len);

	/* Get next index for transmit data DMA buffer and descriptor */
	LPC178X_ETH->txprodidx = (idx + 1) % CONFIG_SYS_TX_ETH_BUFFER;

	rv = 0;
out:
	return rv;
}

/*
 * Process received frames (if any)
 */
static int lpc178x_eth_recv(struct eth_device *dev)
{
	struct lpc178x_eth_dev *mac = to_lpc178x_eth(dev);
	int len;
	/* Index of the buffer where the next packet will be available */
	u32 idx;

	len = 0;

	/* Determine if a frame has been received */
	idx = LPC178X_ETH->rxconsidx;
	if (idx != LPC178X_ETH->rxprodidx) {
		/* Frame received, get size of RX packet */
		len = ((mac->dma->rx_status[idx].info & LPC178X_DMA_RBS_SIZE_MSK) >>
			LPC178X_DMA_RBS_SIZE_BITS) + 1;

		/* Pass the packet up to the protocol layer */
		if (len > 0)
			NetReceive(&mac->dma->rx_buf[idx][0], len);

		/* Return DMA buffer */
		LPC178X_ETH->rxconsidx = (idx + 1) % CONFIG_SYS_RX_ETH_BUFFER;
	}

	return len;
}

/*
 * Halt MAC
 */
static void lpc178x_eth_halt(struct eth_device *dev)
{
	/*
	 * Reset all MAC logic
	 */
	lpc178x_mac_reset();

	/*
	 * Disable power on the Ethernet block
	 */
	lpc178x_periph_enable(LPC178X_SCC_PCONP_PCENET_MSK, 0);
}
