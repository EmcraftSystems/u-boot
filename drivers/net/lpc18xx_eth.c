/*
 * (C) Copyright 2012
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
 * LPC18xx/43xx Ethernet driver
 *
 * Based on the code from LPC178x/7x Ethernet driver (lpc178x_eth.c).
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
#error The LPC18xx/43xx DMA controller cannot work with less than 2 buffers
#endif

#if !defined(CONFIG_SYS_TX_ETH_BUFFER)
#error CONFIG_SYS_TX_ETH_BUFFER should be set
#elif CONFIG_SYS_TX_ETH_BUFFER < 2
#error The LPC18xx/43xx DMA controller cannot work with less than 2 buffers
#endif

/*
 * Device name
 */
#define LPC18XX_MAC_NAME		"LPC18XX_MAC"

#define LPC18XX_PHY_READ_TIMEOUT	2500	/* x 100 usec = 250 ms */
#define LPC18XX_PHY_WRITE_TIMEOUT	2500	/* x 100 usec = 250 ms */
#define LPC18XX_PHY_AUTONEG_TIMEOUT	100000	/* x 100 usec = 10 s */
#define LPC18XX_PHY_RESET_TIMEOUT	4000	/* x 100 usec = 400 ms */

#define LPC18XX_MAC_RESET_TIMEOUT	30	/* x 100 usec = 3 ms */
#define LPC18XX_MAC_TX_TIMEOUT		1000000	/* x 1 usec = 1000 ms */

/*
 * MAC configuration register
 */
/* Receiver enable */
#define ETH_MAC_CONFIG_RE_MSK		(1 << 2)
/* Transmitter Enable */
#define ETH_MAC_CONFIG_TE_MSK		(1 << 3)
/* Back-Off Limit */
#define ETH_MAC_CONFIG_BL_BITS		5
/* Checksum Offload */
#define ETH_MAC_CONFIG_IPC_MSK		(1 << 10)
/* Duplex Mode */
#define ETH_MAC_CONFIG_DM_MSK		(1 << 11)
/* Disable Receive Own */
#define ETH_MAC_CONFIG_DO_MSK		(1 << 13)
/* Speed: 1 = 100 Mbps */
#define ETH_MAC_CONFIG_FES_MSK		(1 << 14)
/* Port select, always = 1 */
#define ETH_MAC_CONFIG_PS_MSK		(1 << 15)
/* Inter-frame gap */
#define ETH_MAC_CONFIG_IFG_BITS		17

/*
 * MII address register
 */
/* MII busy */
#define ETH_MAC_MII_ADDR_GB_MSK		(1 << 0)
/* MII write */
#define ETH_MAC_MII_ADDR_W_MSK		(1 << 1)
/* CSR clock range */
#define ETH_MAC_MII_ADDR_CR_BITS	2
#define ETH_MAC_MII_ADDR_CR_MSK		(0xf << ETH_MAC_MII_ADDR_CR_BITS)
/* MII register */
#define ETH_MAC_MII_ADDR_GR_BITS	6
/* Physical layer address */
#define ETH_MAC_MII_ADDR_PA_BITS	11

/*
 * DMA bus mode register
 */
/* Software reset */
#define ETH_DMA_BUS_MODE_SWR_MSK	(1 << 0)
/* Programmable burst length */
#define ETH_DMA_BUS_MODE_PBL_BITS	8
/* RxDMA programmable burst length */
#define ETH_DMA_BUS_MODE_RPBL_BITS	17

/*
 * DMA status register
 */
/* Transmit interrupt */
#define ETH_DMA_STAT_TI_MSK		(1 << 0)
/* Transmit process stopped */
#define ETH_DMA_STAT_TPS_MSK		(1 << 1)
/* Transmit buffer unavailable */
#define ETH_DMA_STAT_TU_MSK		(1 << 2)
/* Transmit jabber timeout */
#define ETH_DMA_STAT_TJT_MSK		(1 << 3)
/* Receive overflow */
#define ETH_DMA_STAT_OVF_MSK		(1 << 4)
/* Transmit underflow */
#define ETH_DMA_STAT_UNF_MSK		(1 << 5)
/* Receive interrupt */
#define ETH_DMA_STAT_RI_MSK		(1 << 6)
/* Receive buffer unavailable */
#define ETH_DMA_STAT_RU_MSK		(1 << 7)
/* Received process stopped */
#define ETH_DMA_STAT_RPS_MSK		(1 << 8)
/* Receive watchdog timeout */
#define ETH_DMA_STAT_RWT_MSK		(1 << 9)
/* Early transmit interrupt */
#define ETH_DMA_STAT_ETI_MSK		(1 << 10)
/* Fatal bus error interrupt */
#define ETH_DMA_STAT_FBI_MSK		(1 << 13)
/* Early receive interrupt */
#define ETH_DMA_STAT_ERI_MSK		(1 << 14)
/* Abnormal interrupt summary */
#define ETH_DMA_STAT_AIE_MSK		(1 << 15)
/* Normal interrupt summary */
#define ETH_DMA_STAT_NIS_MSK		(1 << 16)
/* All meaningful bits in the DMA_STAT register */
#define ETH_DMA_STAT_ALL_MSK \
	(ETH_DMA_STAT_TI_MSK | \
	ETH_DMA_STAT_TPS_MSK | \
	ETH_DMA_STAT_TU_MSK | \
	ETH_DMA_STAT_TJT_MSK | \
	ETH_DMA_STAT_OVF_MSK | \
	ETH_DMA_STAT_UNF_MSK | \
	ETH_DMA_STAT_RI_MSK | \
	ETH_DMA_STAT_RU_MSK | \
	ETH_DMA_STAT_RPS_MSK | \
	ETH_DMA_STAT_RWT_MSK | \
	ETH_DMA_STAT_ETI_MSK | \
	ETH_DMA_STAT_FBI_MSK | \
	ETH_DMA_STAT_ERI_MSK | \
	ETH_DMA_STAT_AIE_MSK | \
	ETH_DMA_STAT_NIS_MSK)

/*
 * DMA operation mode register
 */
/* Start/stop receive */
#define ETH_DMA_OP_MODE_SR_MSK		(1 << 1)
/* Receive threshold control */
#define ETH_DMA_OP_MODE_RTC_BITS	3
/* Start/Stop transmission command */
#define ETH_DMA_OP_MODE_ST_MSK		(1 << 13)
/* Transmit threshold control */
#define ETH_DMA_OP_MODE_TTC_BITS	14
/* Flush transmit FIFO */
#define ETH_DMA_OP_MODE_FTF_MSK		(1 << 20)

/*
 * Transmit buffer descriptor
 */
/* Second address chained */
#define ETH_TDES0_TCH_MSK		(1 << 20)
/* Transmit end of ring */
#define ETH_TDES0_TER_MSK		(1 << 21)
/* Checksum insertion control */
#define ETH_TDES0_CIC_BITS		22
/* First segment */
#define ETH_TDES0_FS_MSK		(1 << 28)
/* Last segment */
#define ETH_TDES0_LS_MSK		(1 << 29)
/* Own bit */
#define ETH_TDES0_OWN_MSK		(1 << 31)
/* Transmit buffer 1 size */
#define ETH_TDES1_TBS1_BITS		0

/*
 * Receive buffer descriptor
 */
/* Error summary */
#define ETH_RDES0_ES_MSK		(1 << 15)
/* Frame length */
#define ETH_RDES0_FL_BITS		16
#define ETH_RDES0_FL_MSK		(0x3fff << ETH_RDES0_FL_BITS)
/* Own bit: Descriptor owned by the DMA */
#define ETH_RDES0_OWN_MSK		(1 << 31)
/* Receive buffer 1 size */
#define ETH_RDES1_RBS1_BITS		0
/* Second Address Chained */
#define ETH_RDES1_RCH_MSK		(1 << 14)
/* Receive End of Ring */
#define ETH_RDES1_RER_MSK		(1 << 15)

/*
 * Ethernet module register map
 */
struct lpc18xx_eth_regs {
	u32 mac_config;		/* MAC configuration register */
	u32 mac_frame_filter;	/* MAC frame filter */
	u32 mac_hashtable_high;	/* Hash table high register */
	u32 mac_hashtable_low;	/* Hash table low register */
	u32 mac_mii_addr;	/* MII address register */
	u32 mac_mii_data;	/* MII data register */
	u32 mac_flow_ctrl;	/* Flow control register */
	u32 mac_vlan_tag;	/* VLAN tag register */
	u32 rsv0;
	u32 mac_debug;		/* Debug register */
	u32 mac_rwake_frflt;	/* Remote wake-up frame filter */
	u32 mac_pmt_ctrl_stat;	/* PMT control and status */
	u32 rsv1[2];
	u32 mac_intr;		/* Interrupt status register */
	u32 mac_intr_mask;	/* Interrupt mask register */
	u32 mac_addr0_high;	/* MAC address 0 high register */
	u32 mac_addr0_low;	/* MAC address 0 low register */
	u32 rsv2[430];

	u32 mac_timestp_ctrl;	/* Time stamp control register */
	u32 subsecond_incr;	/* Sub-second increment register */
	u32 seconds;		/* System time seconds register */
	u32 nanoseconds;	/* System time nanoseconds register */
	u32 secondsupdate;	/* System time seconds update register */
	u32 nanosecondsupdate;	/* System time nanoseconds update register */
	u32 addend;		/* Time stamp addend register */
	u32 targetseconds;	/* Target time seconds register */
	u32 targetnanoseconds;	/* Target time nanoseconds register */
	u32 highword;		/* System time higher word seconds register */
	u32 timestampstat;	/* Time stamp status register */
	u32 ppsctrl;		/* PPS control register */
	u32 auxnanoseconds;	/* Auxiliary time stamp nanoseconds register */
	u32 auxseconds;		/* Auxiliary time stamp seconds register */
	u32 rsv3[562];

	u32 dma_bus_mode;	/* Bus Mode Register */
	u32 dma_trans_poll_dm;	/* Transmit poll demand register */
	u32 dma_rec_poll_dm;	/* Receive poll demand register */
	u32 dma_rec_des_addr;	/* Receive descriptor list address register */
	u32 dma_trans_des_addr;	/* Transmit descriptor list address register */
	u32 dma_stat;		/* Status register */
	u32 dma_op_mode;	/* Operation mode register */
	u32 dma_int_en;		/* Interrupt enable register */
	u32 dma_mfrm_bufof;	/* Missed frame and buffer overflow register */
	u32 dma_rec_int_wdt;	/* Receive interrupt watchdog timer register */
	u32 rsv4[8];
	u32 dma_curhost_tx_des;	/* Current host transmit descriptor register */
	u32 dma_curhost_rx_des;	/* Current host receive descriptor register */
	u32 dma_curhost_tx_buf;	/* Current host transmit buffer address reg. */
	u32 dma_curhost_rx_buf;	/* Current host receive buffer address reg. */
};

#define LPC18XX_ETH_BASE		0x40010000
#define LPC18XX_ETH			((volatile struct lpc18xx_eth_regs *) \
					LPC18XX_ETH_BASE)

/*
 * Transmit buffer descriptor (without timestamp)
 */
struct lpc18xx_eth_tx_bd {
	u32 ctrlstat;	/* TDES control and status word */
	u32 bsize;	/* Buffer 1/2 byte counts */
	u32 b1add;	/* Buffer 1 address */
	u32 b2add;	/* Buffer 2 or next descriptor address */
};

/*
 * Receive buffer descriptor (without timestamp)
 */
struct lpc18xx_eth_rx_bd {
	u32 status;	/* RDES status word */
	u32 ctrl;	/* Buffer 1/2 byte counts and control */
	u32 b1add;	/* Buffer 1 address */
	u32 b2add;	/* Buffer 2 or next descriptor address */
};

struct lpc18xx_eth_dma_data {
	/*
	 * DMA buffer descriptors:
	 * - have CONFIG_SYS_TX_ETH_BUFFER Tx buffer descriptors;
	 * - have CONFIG_SYS_RX_ETH_BUFFER Rx buffer descriptors.
	 */
	struct lpc18xx_eth_tx_bd tx_bd[CONFIG_SYS_TX_ETH_BUFFER];
	struct lpc18xx_eth_rx_bd rx_bd[CONFIG_SYS_RX_ETH_BUFFER];

	/*
	 * ETH DMAed buffers have length of 1536B (> max eth frm len)
	 */
	u8 tx_buf[CONFIG_SYS_TX_ETH_BUFFER][PKTSIZE_ALIGN];
	u8 rx_buf[CONFIG_SYS_RX_ETH_BUFFER][PKTSIZE_ALIGN];
};

/*
 * LPC18xx Ethernet device
 */
struct lpc18xx_eth_dev {
	/*
	 * Standard Ethernet device
	 */
	struct eth_device netdev;

	/* 32-bit ID combined from PHY ID 1 and PHY ID 2 registers */
	u32 phy_id;
	/* 5-bit PHY address */
	u32 phy_adr;

	volatile struct lpc18xx_eth_dma_data *dma;

	/* Next descriptor to use for transmission */
	int tx_prod_idx;
	/* Next descriptor to check for data given back by the DMA */
	int rx_cons_idx;
};
#define to_lpc18xx_eth(_nd) \
	container_of(_nd, struct lpc18xx_eth_dev, netdev)

/*
 * Prototypes
 */
static  int lpc18xx_eth_init(struct eth_device *dev, bd_t *bd);
static  int lpc18xx_eth_send(
	struct eth_device *dev, volatile void *pkt, int len);
static  int lpc18xx_eth_recv(struct eth_device *dev);
static void lpc18xx_eth_halt(struct eth_device *dev);

static  int lpc18xx_phy_write(struct lpc18xx_eth_dev *mac, u16 reg, u16 val);
static  int lpc18xx_phy_read(struct lpc18xx_eth_dev *mac, u16 reg, u16 *val);

/*
 * Initialize driver
 */
int lpc18xx_eth_driver_init(bd_t *bd)
{
	struct lpc18xx_eth_dev *mac;
	struct eth_device *netdev;
	int rv;

	mac = malloc(sizeof(struct lpc18xx_eth_dev));
	if (!mac) {
		printf("Error: failed to allocate %dB of memory for %s\n",
			sizeof(struct lpc18xx_eth_dev), LPC18XX_MAC_NAME);
		rv = -ENOMEM;
		goto out;
	}
	memset(mac, 0, sizeof(struct lpc18xx_eth_dev));

	netdev = &mac->netdev;

	/*
	 * Autodetect PHY
	 */
	mac->phy_adr = 0;
	mac->phy_id = 0xFF;

	mac->dma = (volatile struct lpc18xx_eth_dma_data *)
		CONFIG_MEM_ETH_DMA_BUF_BASE;

	sprintf(netdev->name, LPC18XX_MAC_NAME);

	netdev->init = lpc18xx_eth_init;
	netdev->halt = lpc18xx_eth_halt;
	netdev->send = lpc18xx_eth_send;
	netdev->recv = lpc18xx_eth_recv;

	rv = eth_register(netdev);
out:
	if (rv != 0 && mac)
		free(mac);

	return rv;
}

/*
 * Initialize PHY: autodetect PHY address
 */
static int lpc18xx_phy_init(struct lpc18xx_eth_dev *mac)
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
		rv = lpc18xx_phy_read(mac, PHY_PHYIDR1, &val);
		if (rv != 0 || val == 0xFFFF || val == 0)
			continue;

		mac->phy_id = (val & 0xFFFF) << 16;
		rv = lpc18xx_phy_read(mac, PHY_PHYIDR2, &val);
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
static int lpc18xx_phy_link_get(
	struct lpc18xx_eth_dev *mac, int *link_up, int *full_dup, int *speed)
{
	u16 bmcr, bmsr;	/* Basic Control and Status registers */
	u16 anar;	/* Auto Negotiation Advertisement */
	u16 anlpar;	/* Auto Negotiation Link Partner Ability */
	int rv;

	rv = lpc18xx_phy_read(mac, PHY_BMCR, &bmcr);
	if (rv != 0)
		goto out;
	rv = lpc18xx_phy_read(mac, PHY_BMSR, &bmsr);
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
		rv = lpc18xx_phy_read(mac, PHY_ANAR, &anar);
		if (rv != 0)
			goto out;

		/*
		 * AND with Link Partner Ability
		 */
		rv = lpc18xx_phy_read(mac, PHY_ANLPAR, &anlpar);
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
static int lpc18xx_phy_link_setup(struct lpc18xx_eth_dev *mac)
{
	u16 bmsr;
	int rv, timeout;
	int link_up, full_dup, speed;

#if defined(CONFIG_LPC18XX_PHY_RMII_REG) && defined(CONFIG_LPC18XX_PHY_RMII_MASK)
	rv = lpc18xx_phy_read(mac, CONFIG_LPC18XX_PHY_RMII_REG, &bmsr);
	if (rv != 0)
		goto out;
	bmsr |= CONFIG_LPC18XX_PHY_RMII_MASK;
	rv = lpc18xx_phy_write(mac, CONFIG_LPC18XX_PHY_RMII_REG, bmsr);
	if (rv != 0)
		goto out;
#endif

	rv = lpc18xx_phy_read(mac, PHY_BMSR, &bmsr);
	if (rv != 0)
		goto out;

	if (bmsr & PHY_BMSR_AUTN_ABLE) {
		printf("Auto-negotiation...");

		/*
		 * Enable auto-negotiation. Force negotiation reset.
		 */
		rv = lpc18xx_phy_write(mac, PHY_BMCR, PHY_BMCR_AUTON);
		if (rv != 0)
			goto out;

		/*
		 * Wait for auto-negotiation to finish
		 */
		timeout = LPC18XX_PHY_AUTONEG_TIMEOUT;
		while (timeout-- > 0 && !(bmsr & PHY_BMSR_AUTN_COMP)) {
			udelay(100);
			rv = lpc18xx_phy_read(mac, PHY_BMSR, &bmsr);
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
	rv = lpc18xx_phy_link_get(mac, &link_up, &full_dup, &speed);
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

		LPC18XX_ETH->mac_config |= ETH_MAC_CONFIG_FES_MSK;
	} else {		/* 10MBase mode */
		printf("(10/");

		LPC18XX_ETH->mac_config &= ~ETH_MAC_CONFIG_FES_MSK;
	}

	/*
	 * Configure Full/Half Duplex mode
	 */
	if (full_dup == 1) {
		printf("Full)\n");

		LPC18XX_ETH->mac_config |= ETH_MAC_CONFIG_DM_MSK;
	} else {
		printf("Half)\n");

		LPC18XX_ETH->mac_config &= ~ETH_MAC_CONFIG_DM_MSK;
	}

	rv = 0;
out:
	return rv;
}

/*
 * Helper function used in lpc18xx_phy_read() and lpc18xx_phy_write()
 */
static int lpc18xx_phy_wait_busy(int timeout)
{
	int rv;

	rv = -ETIMEDOUT;
	while (timeout-- > 0) {
		if (LPC18XX_ETH->mac_mii_addr & ETH_MAC_MII_ADDR_GB_MSK) {
			udelay(100);
		} else {
			timeout = 0;
			rv = 0;
		}
	}

	if (rv != 0)
		printf("%s: timed out\n", __func__);

	return rv;
}

/*
 * Write PHY
 */
static int lpc18xx_phy_write(struct lpc18xx_eth_dev *mac, u16 reg, u16 val)
{
	/* It is important to preserve MAC_MII_ADDR[CR] (CSR clock range) */
	LPC18XX_ETH->mac_mii_addr =
		(LPC18XX_ETH->mac_mii_addr & ETH_MAC_MII_ADDR_CR_MSK) |
		(mac->phy_adr << ETH_MAC_MII_ADDR_PA_BITS) |
		(reg << ETH_MAC_MII_ADDR_GR_BITS) |
		ETH_MAC_MII_ADDR_W_MSK;
	LPC18XX_ETH->mac_mii_data = val;
	LPC18XX_ETH->mac_mii_addr |= ETH_MAC_MII_ADDR_GB_MSK;

	return lpc18xx_phy_wait_busy(LPC18XX_PHY_WRITE_TIMEOUT);
}

/*
 * Read PHY
 */
static int lpc18xx_phy_read(struct lpc18xx_eth_dev *mac, u16 reg, u16 *val)
{
	int rv;

	/* It is important to preserve MAC_MII_ADDR[CR] (CSR clock range) */
	LPC18XX_ETH->mac_mii_addr =
		(LPC18XX_ETH->mac_mii_addr & ETH_MAC_MII_ADDR_CR_MSK) |
		(mac->phy_adr << ETH_MAC_MII_ADDR_PA_BITS) |
		(reg << ETH_MAC_MII_ADDR_GR_BITS);
	LPC18XX_ETH->mac_mii_addr |= ETH_MAC_MII_ADDR_GB_MSK;

	rv = lpc18xx_phy_wait_busy(LPC18XX_PHY_READ_TIMEOUT);
	if (rv == 0)
		*val = LPC18XX_ETH->mac_mii_data;

	return rv;
}

/*
 * Reset PHY
 */
static int lpc18xx_phy_reset(struct lpc18xx_eth_dev *mac)
{
	int rv, read_rv, timeout;
	u16 bmcr;

	/* Reset the PHY and wait for reset to complete */
	rv = lpc18xx_phy_write(mac, PHY_BMCR, PHY_BMCR_RESET);
	if (rv != 0)
		goto out;

	/*
	 * Wait for reset to complete
	 */
	timeout = LPC18XX_PHY_RESET_TIMEOUT;
	rv = -ETIMEDOUT;
	while (timeout-- > 0) {
		read_rv = lpc18xx_phy_read(mac, PHY_BMCR, &bmcr);
		if (read_rv != 0 || (bmcr & PHY_BMCR_RESET) != 0) {
			udelay(100);
		} else {
			timeout = 0;
			rv = 0;
		}
	}

out:
	if (rv != 0)
		printf("%s: timed out\n", __func__);
	return rv;
}

/*
 * Init Ethernet MAC buffer descriptors
 */
static void lpc18xx_mac_bd_init(struct lpc18xx_eth_dev *mac)
{
	int i;

	/*
	 * Initialize the list of transmit descriptors
	 */
	for (i = 0; i < CONFIG_SYS_TX_ETH_BUFFER; i++) {
		mac->dma->tx_bd[i].ctrlstat =
			ETH_TDES0_TCH_MSK |
			(3 << ETH_TDES0_CIC_BITS);
		mac->dma->tx_bd[i].b1add = (u32)&mac->dma->tx_buf[i][0];
		mac->dma->tx_bd[i].b2add = (u32)&mac->dma->tx_bd[i + 1];
	}
	mac->dma->tx_bd[i - 1].ctrlstat |= ETH_TDES0_TER_MSK;
	mac->dma->tx_bd[i - 1].b2add = (u32)&mac->dma->tx_bd[0];

	/*
	 * Initialize the list of receive descriptors
	 */
	for (i = 0; i < CONFIG_SYS_RX_ETH_BUFFER; i++) {
		mac->dma->rx_bd[i].ctrl =
			ETH_RDES1_RCH_MSK |
			(PKTSIZE_ALIGN << ETH_RDES1_RBS1_BITS);
		mac->dma->rx_bd[i].status = ETH_RDES0_OWN_MSK;
		mac->dma->rx_bd[i].b1add = (u32)&mac->dma->rx_buf[i][0];
		mac->dma->rx_bd[i].b2add = (u32)&mac->dma->rx_bd[i + 1];
	}
	mac->dma->rx_bd[i - 1].ctrl |= ETH_RDES1_RER_MSK;
	mac->dma->rx_bd[i - 1].b2add = (u32)&mac->dma->rx_bd[0];

	/*
	 * Setup pointers to descriptor tables
	 */
	LPC18XX_ETH->dma_trans_des_addr = (u32)mac->dma->tx_bd;
	LPC18XX_ETH->dma_rec_des_addr = (u32)mac->dma->rx_bd;
}

/*
 * Set MAC address
 */
static void lpc18xx_mac_address_set(struct lpc18xx_eth_dev *mac)
{
	struct eth_device *netdev = &mac->netdev;

	debug("%s: mac is %#x:%#x:%#x:%#x:%#x:%#x.\n", __func__,
	      netdev->enetaddr[0], netdev->enetaddr[1],
	      netdev->enetaddr[2], netdev->enetaddr[3],
	      netdev->enetaddr[4], netdev->enetaddr[5]);

	LPC18XX_ETH->mac_addr0_high =
		((u32)netdev->enetaddr[5] << 8) |
		(u32)netdev->enetaddr[4];
	LPC18XX_ETH->mac_addr0_low =
		((u32)netdev->enetaddr[3] << 24) |
		((u32)netdev->enetaddr[2] << 16) |
		((u32)netdev->enetaddr[1] << 8) |
		(u32)netdev->enetaddr[0];
}

/*
 * Reset all MAC logic
 */
static void lpc18xx_mac_reset(void)
{
	int rv;
	int timeout;

	LPC18XX_ETH->dma_bus_mode |= ETH_DMA_BUS_MODE_SWR_MSK;

	rv = -ETIMEDOUT;
	timeout = LPC18XX_MAC_RESET_TIMEOUT;
	while (timeout-- > 0) {
		if (LPC18XX_ETH->dma_bus_mode & ETH_DMA_BUS_MODE_SWR_MSK) {
			udelay(100);
		} else {
			timeout = 0;
			rv = 0;
		}
	}

	if (rv != 0)
		printf("%s: timed out\n", __func__);
}

/*
 * Init LPC18xx/43xx Ethernet MAC hardware
 */
static int lpc18xx_mac_hw_init(struct lpc18xx_eth_dev *mac)
{
	static int phy_reset_done;
	int rv;

	/*
	 * Initialize descriptor tracking indices
	 */
	mac->tx_prod_idx = 0;
	mac->rx_cons_idx = 0;

	/*
	 * Reset all MAC logic
	 */
	lpc18xx_mac_reset();

	/*
	 * Do not use enhanced (32-byte) buffer descriptors, because we do not
	 * need the timestamp functionality.
	 */
	LPC18XX_ETH->dma_bus_mode =
		(1 << ETH_DMA_BUS_MODE_PBL_BITS) |
		(1 << ETH_DMA_BUS_MODE_RPBL_BITS);

	/*
	 * Initial MAC configuration for checksum offload, full duplex,
	 * 100Mbps, disable receive own in half duplex, inter-frame gap
	 * of 64-bits.
	 */
	LPC18XX_ETH->mac_config =
		(0 << ETH_MAC_CONFIG_BL_BITS) |
		ETH_MAC_CONFIG_IPC_MSK |
		ETH_MAC_CONFIG_DM_MSK |
		ETH_MAC_CONFIG_DO_MSK |
		ETH_MAC_CONFIG_FES_MSK |
		ETH_MAC_CONFIG_PS_MSK |
		(3 << ETH_MAC_CONFIG_IFG_BITS);

	/*
	 * Receive only packets for our MAC address (+ broadcast packets)
	 */
	LPC18XX_ETH->mac_frame_filter = 0;

	/*
	 * Initialize MDC clock divider
	 */
	LPC18XX_ETH->mac_mii_addr =
		(CONFIG_LPC18XX_ETH_DIV_SEL << ETH_MAC_MII_ADDR_CR_BITS);

	/*
	 * Init PHY
	 */
	rv = lpc18xx_phy_init(mac);
	if (rv != 0)
		goto out;

	/*
	 * Reset PHY
	 */
	if (!phy_reset_done) {
		(void)lpc18xx_phy_reset(mac);
		phy_reset_done = 1;
	}

	/*
	 * Setup link, and complete MAC initialization
	 */
	rv = lpc18xx_phy_link_setup(mac);

out:
	return rv;
}

/*
 * Init Ethernet MAC and DMA
 */
static int lpc18xx_eth_init(struct eth_device *dev, bd_t *bd)
{
	struct lpc18xx_eth_dev *mac = to_lpc18xx_eth(dev);
	int rv;

	/*
	 * Init hw
	 */
	rv = lpc18xx_mac_hw_init(mac);
	if (rv != 0)
		goto out;

	/*
	 * Set MAC address
	 */
	lpc18xx_mac_address_set(mac);

	/*
	 * Init buffer descriptors
	 */
	lpc18xx_mac_bd_init(mac);

	/*
	 * Flush transmit FIFO
	 */
	LPC18XX_ETH->dma_op_mode = ETH_DMA_OP_MODE_FTF_MSK/* |
		(1 << 26) | (1 << 6) | (1 << 7)*/;

	/*
	 * Setup DMA to flush receive FIFOs at 32 bytes, service TX FIFOs
	 * at 64 bytes.
	 */
	LPC18XX_ETH->dma_op_mode |=
		(1 << ETH_DMA_OP_MODE_RTC_BITS) |
		(0 << ETH_DMA_OP_MODE_TTC_BITS);

	/*
	 * Clear all MAC interrupts
	 */
	LPC18XX_ETH->dma_stat = ETH_DMA_STAT_ALL_MSK;

	/*
	 * Disable MAC interrupts
	 */
	LPC18XX_ETH->dma_int_en = 0;

	/*
	 * Enable receive and transmit DMA processes
	 */
	LPC18XX_ETH->dma_op_mode |=
		ETH_DMA_OP_MODE_ST_MSK | ETH_DMA_OP_MODE_SR_MSK;

	/*
	 * Enable packet reception
	 */
	LPC18XX_ETH->mac_config |=
		ETH_MAC_CONFIG_RE_MSK | ETH_MAC_CONFIG_TE_MSK;

	/*
	 * Start receive polling
	 */
	LPC18XX_ETH->dma_rec_poll_dm = 1;

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
static int lpc18xx_eth_send(struct eth_device *dev, volatile void *pkt, int len)
{
	struct lpc18xx_eth_dev *mac = to_lpc18xx_eth(dev);
	int rv, timeout;
	int idx;

	if (len > PKTSIZE_ALIGN) {
		printf("%s: frame too long (%d).\n", __func__, len);
		rv = -EINVAL;
		goto out;
	}

	/*
	 * Wait for a free buffer
	 */
	idx = mac->tx_prod_idx;
	timeout = LPC18XX_MAC_TX_TIMEOUT;
	rv = -ETIMEDOUT;
	while (timeout-- > 0) {
		if (mac->dma->tx_bd[idx].ctrlstat & ETH_TDES0_OWN_MSK) {
			udelay(1);
		} else {
			timeout = 0;
			rv = 0;
		}
	}
	if (rv != 0) {
		printf("%s: timeout.\n", __func__);
		goto out;
	}

	/*
	 * Move data to buffer
	 */
	memcpy_volatile(&mac->dma->tx_buf[idx][0], pkt, len);

	/*
	 * Prepare transmit buffer descriptor
	 */
	mac->dma->tx_bd[idx].b1add = (u32)&mac->dma->tx_buf[idx][0];
	mac->dma->tx_bd[idx].bsize = (len << ETH_TDES1_TBS1_BITS);
	mac->dma->tx_bd[idx].ctrlstat =
		ETH_TDES0_TCH_MSK |
		(3 << ETH_TDES0_CIC_BITS) |
		ETH_TDES0_FS_MSK |
		ETH_TDES0_LS_MSK;
	if (idx == CONFIG_SYS_TX_ETH_BUFFER - 1)
		mac->dma->tx_bd[idx].ctrlstat |= ETH_TDES0_TER_MSK;

	mac->tx_prod_idx = (idx + 1) % CONFIG_SYS_TX_ETH_BUFFER;

	/*
	 * Feed buffer descriptor to the Ethernet DMA
	 */
	mac->dma->tx_bd[idx].ctrlstat |= ETH_TDES0_OWN_MSK;
	LPC18XX_ETH->dma_trans_poll_dm = 1;

	rv = 0;
out:
	return rv;
}

/*
 * Process received frames (if any)
 */
static int lpc18xx_eth_recv(struct eth_device *dev)
{
	struct lpc18xx_eth_dev *mac = to_lpc18xx_eth(dev);
	int len;
	/* Index of the buffer where the next packet will be available */
	u32 idx;
	u32 status;

	len = 0;
	idx = mac->rx_cons_idx;
	status = mac->dma->rx_bd[idx].status;

	/*
	 * Determine if a frame has been received
	 */
	if (status & ETH_RDES0_OWN_MSK)
		goto out;

	if (status & ETH_RDES0_ES_MSK) {
		printf("%s: Received packet has errors.\n", __func__);
		goto out;
	}

	/*
	 * Frame received, get size of RX packet
	 */
	len = (status & ETH_RDES0_FL_MSK) >> ETH_RDES0_FL_BITS;

	/*
	 * Pass the packet up to the protocol layer
	 */
	if (len > 0)
		NetReceive(&mac->dma->rx_buf[idx][0], len);

	/*
	 * Feed descriptor back to the Ethernet MAC
	 */
	mac->dma->rx_bd[idx].status = ETH_RDES0_OWN_MSK;
	LPC18XX_ETH->dma_rec_poll_dm = 1;

	mac->rx_cons_idx = (idx + 1) % CONFIG_SYS_RX_ETH_BUFFER;

out:
	return len;
}

/*
 * Halt MAC
 */
static void lpc18xx_eth_halt(struct eth_device *dev)
{
	/*
	 * Reset all MAC logic
	 */
	lpc18xx_mac_reset();
}
