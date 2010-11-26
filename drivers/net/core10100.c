/*
 * Copyright (C) 2010 Dmitry Cherkassov, Sergei Poselenov, Emcraft Systems
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <config.h>
#include <common.h>
#include <net.h>
#include <malloc.h>
#include <miiphy.h>
#include <asm/io.h>
#include "core10100.h"

/* Set to 1 to turn on debugging */
static u32 dbg;

static inline u32 find_next_desc(u32 cur, u32 size)
{
	return ((++cur) % size);
}

/* Stop transmission and receiving */
static void stop_tx_rx(struct core10100_dev *bp)
{
	int i;

	/* Stop transmission and receiving */
	write_reg(CSR6, read_reg(CSR6) & ~(CSR6_ST | CSR6_SR));

	/* Wait for the transmission and receiving processes stopped */
	for (i = 0; i < TIMEOUT_LOOPS; i++) {
		if (((read_reg(CSR5) >> CSR5_TS_SHIFT) & CSR5_TS_MASK) ==
		    CSR5_TS_STOP &&
		    ((read_reg(CSR5) >> CSR5_RS_SHIFT) & CSR5_RS_MASK) ==
		    CSR5_RS_STOP) {
			break;
		}
		udelay(TIMEOUT_UDELAY);
	}
	write_reg(CSR5, read_reg(CSR5) | CSR5_TPS);
	write_reg(CSR5, read_reg(CSR5) | CSR5_RPS);
}


/* Read a register value via MII */
static ushort mii_read(struct core10100_dev *bp, u16 reg)
{
	u16 data;	/* Read data */

	bb_miiphy_read(NULL, bp->phy_addr, reg, &data);

	return data;
}

/* Write a register value via MII */
static void mii_write(struct core10100_dev *bp, u16 reg, u16 data)
{
	if (bb_miiphy_write (NULL, bp->phy_addr, reg, data)) {
		printf ("%s error!\n", __func__);
	}
}

/* Perform auto-negotiation */
static void phy_auto_negotiation(struct core10100_dev *bp)
{
	int i;
	printf("Auto-negotiation...");

	/* Enable and restart auto-negotiation */
	mii_write(bp, PHY_BMCR, mii_read(bp, PHY_BMCR) | PHY_BMCR_AUTON
			  | PHY_BMCR_RST_NEG);

	/* Wait for auto-negotiation completion */
	for (i = 0; i < TIMEOUT_LOOPS; i++) {
		if (mii_read(bp, PHY_BMSR) & PHY_BMSR_AUTN_COMP) {
			printf("completed\n");
			return;
		}
		udelay(TIMEOUT_UDELAY);
	}
	printf("timeout\n");
}

/* Initialize PHY */
static int phy_init(struct core10100_dev *bp)
{
	int i;
	u32 val;

	if (bp->phy_id != 0xff) {
		/* Already inited */
		return 0;
	}
	/* Probe (find) a PHY */
	for (i = 0; i < 32; i++) {
		bp->phy_addr = i;
		val = mii_read(bp, PHY_PHYIDR1);
		if (val != 0 && val != 0xffff) {
			bp->phy_id = (val & 0xffff) << 16;
			val = mii_read(bp, PHY_PHYIDR2);
			if (val != 0 && val != 0xffff) {
				bp->phy_id |= (val & 0xffff);
			}
			break;
		}
	}

	if (i == 32) {
		/* Have not found a PHY */
		bp->phy_id = 0xff;
		printf("%s: PHY not found\n", __func__);
		return -1;
	}

	if(dbg)printf("%s: found PHY id = %#x at addr %#x\n", __func__,
				  bp->phy_id, bp->phy_addr);

	/* Software reset */
	mii_write(bp, PHY_BMCR, PHY_BMCR_RESET);

	return 0;
}

/* get link status */
static int phy_link_stat(struct core10100_dev *bp)
{
	u16 val;
	u32 link_stat = 0;

	/* If not initialized, return no link */
	if (bp->phy_id == 0xff) {
		return 0;
	}

	/* Read link up/down */
	val = mii_read(bp, PHY_BMSR);

	/* Update link up/down */
	if (val &  PHY_BMSR_LS) {
		link_stat |= LINK_UP;
	}

	/* Read link speed and duplex */
	val = mii_read(bp, PHY_BMCR);

	/* Update link speed */
	if (val & (PHY_BMSR_100TXH | PHY_BMSR_100TXF)) {
		link_stat |= LINK_100;
	}

	/* Update link speed */
	if (val & PHY_BMSR_EXT_STAT) {
		link_stat |= LINK_FD;
	}

	return link_stat;
}

/* set link status */
static int setup_link(struct core10100_dev *bp)
{
	u32 link_stat;

	if (bp->phy_id == 0xff) {
		printf("%s: PHY not inited?\n", __func__);
		return -1;
	}

	/* Get link status */
	link_stat = phy_link_stat(bp);

	if (!(link_stat & LINK_UP)) {
		bp->flags &= ~(LINK_UP | LINK_100 | LINK_FD);
		/* Perform auto-negotiation */
		phy_auto_negotiation(bp);

		/* Get link status */
		link_stat = phy_link_stat(bp);
		if (!(link_stat & LINK_UP)) {
			printf("Link is DOWN\n");
			bp->flags &= ~(LINK_UP | LINK_100 | LINK_FD);
			bp->phy_id = 0xff; /* To restart PHY auto-discovery */
			return -1;
		}
	}

	/* Link is up */
	bp->flags |= LINK_UP;
	printf("%s: link up ", bp->netdev.name);
	/* Update MAC register */
	if (link_stat & LINK_100) {
		printf("(100/");
		bp->flags |= LINK_100;
		write_reg(CSR6, read_reg(CSR6) | CSR6_TTM);
	} else {
		printf("(10/");
		bp->flags &= ~LINK_100;
		write_reg(CSR6, read_reg(CSR6) & ~CSR6_TTM);
	}

	/* Update MAC register */
	if (link_stat & LINK_FD) {
		printf("Full)\n");
		bp->flags |= LINK_FD;
		write_reg(CSR6, read_reg(CSR6) | CSR6_FD);
	} else {
		printf("Half)\n");
		bp->flags &= ~LINK_FD;
		write_reg(CSR6, read_reg(CSR6) & ~CSR6_FD);
	}

	return 0;
}

static void dump_desc(struct rxtx_desc *tx_desc, char *s, int dbg)
{
	if (dbg) {
		printf("  DUMP of %sDESC @%#x\n", s, (int)tx_desc);
		printf("  OWNSTAT: %#x\n", tx_desc->own_stat);
		printf("  CNTLSIZ: %#x\n", tx_desc->cntl_size);
		printf("  BUF1   : %#x\n", (int)tx_desc->buf1);
		printf("  BUF2   : %#x\n", (int)tx_desc->buf2);
	}
}

static void core_set_mac(struct eth_device *netdev)
{
	int i;
	struct core10100_dev *bp = to_core(netdev);

	if(dbg)printf("%s: mac is %#x:%#x:%#x:%#x:%#x:%#x\n", __func__,
				  netdev->enetaddr[0], netdev->enetaddr[1],
		   netdev->enetaddr[2], netdev->enetaddr[3],
		   netdev->enetaddr[4], netdev->enetaddr[5]);

	for (i = 0; i < 192; i += 12) {
		memcpy((void *)bp->mac_filter + i, netdev->enetaddr, 6);
	}

	bp->tx_mac->own_stat = DESC_OWN;
	bp->tx_mac->cntl_size = DESC_TCH | DESC_SET | 192;
	bp->tx_mac->buf1 = bp->mac_filter;
	bp->tx_mac->buf2 = (struct rxtx_desc *) bp->tx_mac;
	dump_desc((struct rxtx_desc *)bp->tx_mac, "MAC TX", dbg);
	write_reg(CSR4, (u32) bp->tx_mac);

	/* Start transmission */
	write_reg(CSR6, read_reg(CSR6) | CSR6_ST);

	/* Wait for the packet transmission end */
	for (i = 0; i < TIMEOUT_LOOPS; i++) {
		/* Transmit poll demand */
		write_reg(CSR1, CSR1_TPD);
		/* Wait until Core10/100 returns the descriptor ownership */
		if (!(bp->tx_mac->own_stat & DESC_OWN)) {
			break;
		}
		udelay(TIMEOUT_UDELAY);
		/* WDT_RESET; */
	}

	if (i == TIMEOUT_LOOPS) {
		printf( "%s: MAC addr setup TX timeout\n", __func__);
		return;
	}

	/* Stop transmission */
	write_reg(CSR6, read_reg(CSR6) & ~CSR6_ST);
	/* Wait for the transmission process stopped */
	for (i = 0; i < TIMEOUT_LOOPS; i++) {
		if (((read_reg(CSR5) >> CSR5_TS_SHIFT) & CSR5_TS_MASK) ==
		    CSR5_TS_STOP) {
			break;
		}
		udelay(TIMEOUT_UDELAY);
		/* WDT_RESET; */
	}

	if (i == TIMEOUT_LOOPS) {
		printf("%s: Can't stop TX!\n", __func__);
		return ;
	}

	write_reg(CSR5, read_reg(CSR5) | CSR5_TPS);

	/* Restore the real TX descriptors pointers */
	write_reg(CSR4, (unsigned long)&bp->tx_descs[0]);
}

static int core_init(struct eth_device *netdev, bd_t *bd)
{
	struct core10100_dev *bp = to_core(netdev);
	int a;

	volatile char *p = (volatile char *)CONFIG_CORE10100_INTRAM_ADDRESS;

	/* Reset Controller */
	write_reg(CSR0,  read_reg(CSR0) | CSR0_SWR);
	for (a = 0; a < TIMEOUT_LOOPS; a++) {
        if (!(read_reg(CSR0) & CSR0_SWR)) {
            break;
        }
        udelay(TIMEOUT_UDELAY);
    }

    if (a == TIMEOUT_LOOPS) {
        printf("%s reset timeout\n", __func__);
        return -1;
    }

	/* Setup the little endian mode for the data descriptors  */
    write_reg(CSR0, read_reg(CSR0) & ~CSR0_DBO);


	bp->rx_descs = (struct rxtx_desc *)p;
	p += sizeof(struct rxtx_desc) * RX_RING_SIZE;

	bp->tx_descs = (struct rxtx_desc *)p;
	p += sizeof(struct rxtx_desc) * TX_RING_SIZE;

	bp->tx_mac = (struct rxtx_desc *)p;
	p += sizeof(struct rxtx_desc);

	bp->mac_filter = p;
	p += 192;

	/*
	  Setup RX descriptor as follows:
	  - owned by Core
	  - chained
	  - buffer size is MAX_DATA_SIZE_ALIGNED
	  - buffer1 points to rx_buf
	  - buffer2 points to the descriptor itself
	*/

	for (a = 0; a < RX_RING_SIZE; a++) {
		/* Give the ownership to the MAC */
		bp->rx_descs[a].own_stat = DESC_OWN;

		/*
		  The size field of the descriptor is 10 bits in size,
		  so lets check that the
		  CFG_MAX_ETH_MSG_SIZE is not bigger than 2047
		*/

		bp->rx_descs[a].cntl_size =
			DESC_TCH |
			(CORE10100_MAX_DATA_SIZE_ALIGNED > 0x7FF ?
			 0x7FF : CORE10100_MAX_DATA_SIZE_ALIGNED);

		bp->rx_buffs[a] = p;
		p += CORE10100_MAX_DATA_SIZE_ALIGNED;
		bp->rx_descs[a].buf1 = bp->rx_buffs[a];
		bp->rx_descs[a].buf2 =	(struct rxtx_desc *) &bp->rx_descs[find_next_desc(a, RX_RING_SIZE)];

		dump_desc((struct rxtx_desc *)&bp->rx_descs[a], "RX", dbg);
	}
	bp->rx_cur = 0;
	write_reg(CSR3, (u32) bp->rx_descs);

	/*
	  Setup TX descriptors as follows (two descriptor are used,
	  refer to the Core10/100 header file (core_mac.h) for details):
	  - chained
	  - buffer1  will be set later to skb->data
	  - buffer2 points to the following itself
	*/
	for (a = 0; a < TX_RING_SIZE; a++) {
		/* Give the ownership to the host */
		bp->tx_descs[a].own_stat = 0;
		bp->tx_descs[a].cntl_size = 0; /* Will be set in start_xmit() */

		bp->tx_buffs[a] = p;
		p += CORE10100_MAX_DATA_SIZE_ALIGNED;
		bp->tx_descs[a].buf1 = bp->tx_buffs[a];
		bp->tx_descs[a].buf2 = (void *)&bp->tx_descs[find_next_desc(a, TX_RING_SIZE)];
	}
	bp->tx_cur = 0;

	write_reg(CSR4, (u32) bp->tx_descs);

	core_set_mac(netdev);

	if (phy_init(bp) != 0 || setup_link(bp) != 0) {
		printf("%s: phy init error\n", __func__);
		return -1;
	}

	/* Start transmission and receiving */
	write_reg(CSR6, read_reg(CSR6) | CSR6_ST | CSR6_SR);
	bp->flags |= TX_RX_ENABLED;
	return 0;
}


static void  core_halt(struct eth_device *netdev)
{
	struct core10100_dev *bp = to_core(netdev);

	/* Disable TX and RX */
	stop_tx_rx(bp);
}

/* Send a packet. We don't wait here for the packet to be fully transmitted,
 * instead, make sure the descriptor is free (which means that transfer was
 * completed OK).
 */
static int core_send(struct eth_device *netdev, volatile void *packet,
		     int length)
{	u32 i;

	struct core10100_dev *bp = to_core(netdev);

	if (!(bp->flags & LINK_UP)) {
		printf("%s: no link\n", __func__);
		return -1;
	}

	if(dbg)printf("%s: len %d\n", __func__, length);

	if (!length) {
		printf("%s: zero len?\n", __func__);
		return -1;
	}

	for (i = 0; i < TIMEOUT_LOOPS; i++) {
		if (!(bp->tx_descs[bp->tx_cur].own_stat & DESC_OWN)) {
			break;
		}
		udelay(TIMEOUT_UDELAY);
	}

	if (i == TIMEOUT_LOOPS) {
		printf ("%s: Tx desc %d is not free?\n", __func__, bp->tx_cur);
		return -1;
	}

	/*
	  Prepare the descriptors as follows:
	  - set the last descriptor flag
	  - set the first descriptor flag
	  - set the packet length
	*/

	bp->tx_descs[bp->tx_cur].cntl_size = DESC_TCH | DESC_TLS | DESC_TFS | length;

	memcpy((void *)bp->tx_descs[bp->tx_cur].buf1, (void *)packet, length);

	dump_desc((struct rxtx_desc *)&bp->tx_descs[bp->tx_cur], "TX", dbg);

	/* Give the current descriptor ownership to Core10/100.	*/
	bp->tx_descs[bp->tx_cur].own_stat = DESC_OWN;

	/* Start transmission */
	write_reg(CSR6, read_reg(CSR6) | CSR6_ST);

	/* Transmit poll demand */
	write_reg(CSR1, CSR1_TPD);

	/* Bump ptr to next descriptor */
	bp->tx_cur = find_next_desc(bp->tx_cur, TX_RING_SIZE);

	return 0;
}


static int core10100_check_rxframe(struct rxtx_desc *rx_desc)
{
	int badframe = 0;
	/*
	  Check that the descriptor contains the whole packet,
	  i.e. the fist and last descriptor flags are set.
	*/
	if (!(rx_desc->own_stat & DESC_RFS) ||
		!(rx_desc->own_stat & DESC_RLS)) {
		printf("%s:receive_frame error: not whole packet\n", __func__);
		return 1;
	}

	if (rx_desc->own_stat & DESC_RTL) {
		printf("%s: Too long frame\n", __func__);
		return 1;
	}

	if (rx_desc->own_stat & DESC_RLS) {
		/* The DESC_RES bit is valid only when the DESC_RLS is set */
#if 0
		if((rx_desc->own_stat & DESC_RES)) {/* don't report collisions */
			printf(KERN_INFO "receive_frame error: DESC_RES flag is set, len %d\n", (rx_desc->own_stat >> 16) & 0x3fff);
			dump_desc(rx_desc, "RX", 1);
			/* Chesk status: may be status cache is out of sync */
			/* link_stat(pd); */
			badframe = 1;
		}
#endif
		if (rx_desc->own_stat & DESC_RDE) {
			printf("%s: Descriptor Error (no Rx buffer avail)\n", __func__);
			badframe = 1;
		}
		if (rx_desc->own_stat & DESC_RRF) {
			printf("%s: Runt Frame (damaged)\n", __func__);
			badframe = 1;
		}
#if 1 //psl
		if (rx_desc->own_stat & DESC_RCS) {
			printf("%s: Collision\n", __func__);
			badframe = 1;
		}
#endif
		if (rx_desc->own_stat & DESC_RRE) {
			printf("%s: RMII error\n", __func__);
			badframe = 1;
		}
		if (rx_desc->own_stat & DESC_RDB) {
			printf("%s: Frame is not byte-aligned\n", __func__);
			badframe = 1;
		}
		if (rx_desc->own_stat & DESC_RCE) {
			printf("%s: Frame CRC err\n", __func__);
			badframe = 1;
		}
		if (badframe)
			return 1;
	}

	if (rx_desc->own_stat & DESC_RZERO) {
		printf("%s: Bad frame len\n", __func__);
		return 1;
	}
#if 0
	if (rx_desc->own_stat & DESC_RLS) {
		if (rx_desc->own_stat & DESC_RMF) {
			printf("%s: Multicast Frame\n", __func__);
		}
 /* always set? */
		if (rx_desc->own_stat & DESC_RFT) {
			printf("%s: Not 802.3 frame type, len %d\n", __func__, (rx_desc->own_stat >> 16) & 0x3fff);
		}
	}
#endif
	return 0;
}

static int core_recv(struct eth_device *netdev)
{
	struct core10100_dev *bp = to_core(netdev);
	int j;
	u32 size;

	if (!(bp->flags & LINK_UP)) {
		printf("%s: no link\n", __func__);
		return -1;
	}

	for (j = 0; j < RX_RING_SIZE; j++, bp->rx_cur = find_next_desc(bp->rx_cur, RX_RING_SIZE)) {
		if (bp->rx_descs[bp->rx_cur].own_stat & DESC_OWN) {
			if(dbg)printf("%s: %d not ready, exiting, try %d\n", __func__, bp->rx_cur, j);
			break;
		}
		if(dbg)printf("rx_cur = %d, i %d\n", bp->rx_cur, j);

		if (core10100_check_rxframe((struct rxtx_desc *)&bp->rx_descs[bp->rx_cur])) {
			goto end_alloc;
		}

		/* Check the received packet size */
		size = (bp->rx_descs[bp->rx_cur].own_stat >> 16) & 0x3fff;
		if (size > CORE10100_MAX_DATA_SIZE_ALIGNED) {
			/* Drop the packet */
			printf("%s: pkt sz %d > bufsize %d", __func__, size, CORE10100_MAX_DATA_SIZE_ALIGNED);
			goto end_alloc;
		}

		if(dbg)printf("%s: NetReceive pktlen %#x\n", __func__, size);
		NetReceive(bp->rx_descs[bp->rx_cur].buf1, size);

end_alloc:
		/* Prepare the packet for the following receiving */
		bp->rx_descs[bp->rx_cur].cntl_size = DESC_RCH | CORE10100_MAX_DATA_SIZE_ALIGNED;
		/* Give the descriptor ownership to Core */
		bp->rx_descs[bp->rx_cur].own_stat = DESC_OWN;
	}
	/* Receive poll demand */
	write_reg(CSR2, 1);

	return 0;
}

int core_eth_init(bd_t *bd)
{
	struct core10100_dev *bp;
	struct eth_device *netdev;

	bp = malloc(sizeof(struct core10100_dev));
	if (!bp) {
		printf("Error: Failed to allocate memory for core10100\n");
		return -1;
	}

	memset(bp, 0, sizeof(struct core10100_dev));

	netdev = &bp->netdev;

	bp->phy_addr = 0; /* Will auto-discover */
	bp->phy_id = 0xff; /* Mark as not initialized */

	netdev->iobase = (u32)MAC_BASE;
	bp->base = (char *)MAC_BASE;

	sprintf(netdev->name, "MAC0");
	netdev->init = core_init;

	netdev->halt = core_halt;
	netdev->send = core_send;
	netdev->recv = core_recv;

	eth_register(netdev);
#ifdef CONFIG_CMD_MII
	miiphy_register("core10100", bb_miiphy_read, bb_miiphy_write);
#endif
	return 0;
}
