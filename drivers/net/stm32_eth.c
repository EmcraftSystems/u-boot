/*
 * (C) Copyright 2011
 *
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
 * STM32 F2 Ethernet driver
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

#include <asm/arch/stm32.h>
#include <asm/arch/stm32f2_gpio.h>

#if defined(CONFIG_STM32_SYS_CLK_HSI)
# error "Can't run MAC with this CLK configuration."
#endif

/*
 * Device name
 */
#define STM32_MAC_NAME			"STM32_MAC"

/*
 * STM32 MAC/DMA definitions
 */
/*
 * MAC registers base
 */
#define STM32_MAC_BASE			(STM32_AHB1PERIPH_BASE + 0x8000)

/*
 * MACCR reg fields
 */
#define STM32_MAC_CR_RE			(1 << 2)	/* Received enable    */
#define STM32_MAC_CR_TE			(1 << 3)	/* Transmitter enable */
#define STM32_MAC_CR_DM			(1 << 11)	/* Duplex mode	      */
#define STM32_MAC_CR_FES		(1 << 14)	/* Fast Eth speed     */

/*
 * MACMIIAR reg fields
 */
#define STM32_MAC_MIIAR_MB		(1 << 0)	/* MII busy	      */
#define STM32_MAC_MIIAR_MW		(1 << 1)	/* MII write	      */

#define STM32_MAC_MIIAR_CR_BIT		2		/* Clock range	      */
#define STM32_MAC_MIIAR_CR_MSK		0x7
#define STM32_MAC_MIIAR_CR_DIV42	0x0		/* 60-100 MHz	      */
#define STM32_MAC_MIIAR_CR_DIV62	0x1		/* 100-168 MHz	      */
#define STM32_MAC_MIIAR_CR_DIV16	0x2		/* 20-35 MHz	      */
#define STM32_MAC_MIIAR_CR_DIV26	0x3		/* 35-60 MHz	      */

#define STM32_MAC_MIIAR_MR_BIT		6		/* MII register	      */
#define STM32_MAC_MIIAR_MR_MSK		0x1F

#define STM32_MAC_MIIAR_PA_BIT		11		/* PHY address	      */
#define STM32_MAC_MIIAR_PA_MSK		0x1F

/*
 * DMABMR reg fields
 */
#define STM32_MAC_DMABMR_SR		(1 << 0)	/* Software reset     */

#define STM32_MAC_DMABMR_PBL_BIT	8		/* Burst length	      */
#define STM32_MAC_DMABMR_PBL_MSK	0x3F

#define STM32_MAC_DMABMR_RTPR_BIT	14		/* Rx:Tx priority rat.*/
#define STM32_MAC_DMABMR_RTPR_MSK	0x3
#define STM32_MAC_DMABMR_RTPR_1_1	0x0		/* 1 : 1	      */
#define STM32_MAC_DMABMR_RTPR_2_1	0x1		/* 2 : 1	      */
#define STM32_MAC_DMABMR_RTPR_3_1	0x2		/* 3 : 1	      */
#define STM32_MAC_DMABMR_RTPR_4_1	0x3		/* 4 : 1	      */

#define STM32_MAC_DMABMR_FB		(1 << 16)	/* Fixed burst	      */

#define STM32_MAC_DMABMR_RDP_BIT	17		/* RX DMA PBL	      */
#define STM32_MAC_DMABMR_RDP_MSK	0x3F

#define STM32_MAC_DMABMR_USP		(1 << 23)	/* Use separate PBL   */
#define STM32_MAC_DMABMR_AAB		(1 << 25)	/* Adr-aligned beats  */

/*
 * DMASR reg fields
 */
#define STM32_MAC_DMASR_TBUS		(1 << 2)	/* Tx buf unavailable */
#define STM32_MAC_DMASR_RBUS		(1 << 7)	/* Rx buf unavailable */

/*
 * DMAOMR reg fields
 */
#define STM32_MAC_DMAOMR_SR		(1 << 1)	/* Start/stop rx      */
#define STM32_MAC_DMAOMR_ST		(1 << 13)	/* Start/stop tx      */
#define STM32_MAC_DMAOMR_FTF		(1 << 20)	/* Flush tx FIFO      */

/*
 * DMA transmit buffer descriptor bits
 */
#define STM32_DMA_TBD_DMA_OWN		(1 << 31)	/* DMA/CPU owns bd    */
#define STM32_DMA_TBD_LS		(1 << 29)	/* Last segment	      */
#define STM32_DMA_TBD_FS		(1 << 28)	/* First segment      */
#define STM32_DMA_TBD_TCH		(1 << 20)	/* 2nd address chained*/

/*
 * DMA receive buffer descriptor bits
 */
#define STM32_DMA_RBD_DMA_OWN		(1 << 31)	/* DMA/CPU owns bd    */
#define STM32_DMA_RBD_FL_BIT		16		/* Frame length	      */
#define STM32_DMA_RBD_FL_MSK		0x3FFF
#define STM32_DMA_RBD_FS		(1 << 9)	/* First descriptor   */
#define STM32_DMA_RBD_LS		(1 << 8)	/* Last descriptor    */

#define STM32_DMA_RBD_RCH		(1 << 14)	/* 2nd address chained*/

/*
 * STM32 SYSCFG definitions
 */
#define STM32_SYSCFG_BASE		(STM32_APB2PERIPH_BASE + 0x3800)

/*
 * PMC reg fields
 */
#define STM32_SYSCFG_PMC_SEL_BIT	23		/* MII/RMII selection */
#define STM32_SYSCFG_PMC_SEL_MSK	0x1

#define STM32_SYSCFG_PMC_SEL_MII	0
#define STM32_SYSCFG_PMC_SEL_RMII	1

/*
 * STM32 RCC MAC specific definitions
 */
#define STM32_RCC_AHB1RSTR_MAC		(1 << 25)	/* Reset MAC	      */

#define STM32_RXX_ENR_SYSCFG		(1 << 14)	/* SYSCFG clock	      */

#define STM32_RCC_ENR_ETHMACEN		(1 << 25)	/* Ethernet MAC clock */
#define STM32_RCC_ENR_ETHMACTXEN	(1 << 26)	/* Ethernet Tx clock  */
#define STM32_RCC_ENR_ETHMACRXEN	(1 << 27)	/* Ethernet Rx clock  */
#define STM32_RCC_ENR_ETHMACPEN		(1 << 28)	/* Ethernet ? clock  */

/*
 * Different timeouts
 */
#define STM32_PHY_READ_TIMEOUT		2500	/* x 100 usec = 250 ms */
#define STM32_PHY_WRITE_TIMEOUT		2500	/* x 100 usec = 250 ms */
#define STM32_PHY_AUTONEG_TIMEOUT	100000	/* x 100 usec = 10 s */

#define STM32_MAC_TX_TIMEOUT		1000000	/* x 1 usec = 1000 ms */
#define STM32_MAC_INIT_TIMEOUT		20000	/* x 100 usec = 2 s */

/*
 * MAC, MMC, PTP, DMA register map
 */
struct stm32_mac_regs {
	u32	maccr;		/* MAC configuration			      */
	u32	macffr;		/* MAC frame filter			      */
	u32	machthr;	/* MAC hash table high			      */
	u32	machtlr;	/* MAC hash table low			      */
	u32	macmiiar;	/* MAC MII address			      */
	u32	macmiidr;	/* MAC MII data				      */
	u32	macfcr;		/* MAC flow control			      */
	u32	macvlantr;	/* MAC VLAN tag				      */
	u32	rsv0[2];
	u32	macrwuffr;	/* MAC remote wakeup frame filter	      */
	u32	macpmtcsr;	/* MAC PMT control and status		      */
	u32	rsv1;
	u32	macdbgr;	/* MAC debug				      */
	u32	macsr;		/* MAC interrupt status			      */
	u32	macimr;		/* MAC interrupt mask			      */
	u32	maca0hr;	/* MAC address 0 high			      */
	u32	maca0lr;	/* MAC address 0 low			      */
	u32	maca1hr;	/* MAC address 1 high			      */
	u32	maca1lr;	/* MAC address 1 low			      */
	u32	maca2hr;	/* MAC address 2 high			      */
	u32	maca2lr;	/* MAC address 2 low			      */
	u32	maca3hr;	/* MAC address 3 high			      */
	u32	maca3lr;	/* MAC address 3 low			      */
	u32	rsv2[40];
	u32	mmccr;		/* MMC control				      */
	u32	mmcrir;		/* MMC receive interrupt		      */
	u32	mmctir;		/* MMC transmit interrupt		      */
	u32	mmcrimr;	/* MMC receive interrupt mask		      */
	u32	mmctimr;	/* MMC transmit interrupt mask		      */
	u32	rsv3[14];
	u32	mmctgfsccr;	/* MMC transmitted good frms after single col */
	u32	mmctgfmsccr;	/* MMC transmitted good frms after more col   */
	u32	rsv4[5];
	u32	mmctgfcr;	/* MMC transmitted good frames counter	      */
	u32	rsv5[10];
	u32	mmcrfcecr;	/* MMC received frames with CRC error counter */
	u32	mmcrfaecr;	/* MMC received frames with alignment error   */
	u32	rsv6[10];
	u32	mmcrgufcr;	/* MMC received good unicast frames counter   */
	u32	rsv7[334];
	u32	ptptscr;	/* PTP time stamp control		      */
	u32	ptpssir;	/* PTP subsecond increment		      */
	u32	ptptshr;	/* PTP time stamp high			      */
	u32	ptptslr;	/* PTP time stamp low			      */
	u32	ptptshur;	/* PTP time stamp high update		      */
	u32	ptptslur;	/* PTP time stamp low update		      */
	u32	ptptsar;	/* PTP time stamp addend		      */
	u32	ptptthr;	/* PTP target time high			      */
	u32	ptpttlr;	/* PTP target time low			      */
	u32	rsv8;
	u32	ptptssr;	/* PTP time stamp status		      */
	u32	ptpppscr;	/* PTP PPS control			      */
	u32	rsv9[564];
	u32	dmabmr;		/* DMA bus mode				      */
	u32	dmatpdr;	/* DMA transmit poll demand		      */
	u32	dmarpdr;	/* DMA receive poll demand		      */
	u32	dmardlar;	/* DMA receive descriptor list address	      */
	u32	dmatdlar;	/* DMA transmit descriptor list address	      */
	u32	dmasr;		/* DMA status				      */
	u32	dmaomr;		/* DMA operation mode			      */
	u32	dmaier;		/* DMA interrupt enable			      */
	u32	dmamfbocr;	/* DMA missed frame and buffer overflow	      */
	u32	dmarswtr;	/* DMA receive status watchdog timer	      */
	u32	rsv10[8];
	u32	dmachtdr;	/* DMA current host transmit descriptor	      */
	u32	dmachrdr;	/* DMA current host receive descriptor	      */
	u32	dmachtbar;	/* DMA current host transmit buffer address   */
	u32	dmachrbar;	/* DMA current host receive buffer address    */
};
#define STM32_MAC			((volatile struct stm32_mac_regs *) \
					STM32_MAC_BASE)

/*
 * SYSCFG register map
 */
struct stm32_syscfg_regs {
	u32	memrmp;		/* Memory remap				      */
	u32	pmc;		/* Peripheral mode configuration	      */
	u32	exticr[4];	/* External interrupt configuration	      */
	u32	rsv0[2];
	u32	cmpcr;		/* Compensation cell control		      */
};
#define STM32_SYSCFG			((volatile struct stm32_syscfg_regs *) \
					STM32_SYSCFG_BASE)

/*
 * STM32 ETH Normal DMA buffer descriptors
 */
struct stm_eth_dma_bd {
	volatile u32			stat;	/* Status		      */
	volatile u32			ctrl;	/* Control, and buffer length */
	volatile u8			*buf;	/* Pointer to buffer	      */
	volatile struct stm_eth_dma_bd	*next;	/* Pointer to next BD in chain*/
};

/*
 * STM32 ETH device
 */
struct stm_eth_dev {
	/*
	 * Standard ethernet device
	 */
	struct eth_device		netdev;

	/*
	 * PHY settings
	 */
	u32				phy_id;
	u32				phy_adr;

	/*
	 * DMA buffer descriptors, and index of last processed buf:
	 * - have one Tx buffer descriptor;
	 * - have CONFIG_SYS_RX_ETH_BUFFER rx buffer descriptors.
	 */
	volatile struct stm_eth_dma_bd	tx_bd;
	volatile struct stm_eth_dma_bd	rx_bd[PKTBUFSRX];
	s32				rx_bd_idx;

	/*
	 * ETH DMAed buffers:
	 * - send requested buffers directly, i.e. have no local storage;
	 * - receive buffers have length of 1536B (> max eth frm len)
	 */
	volatile u8			rx_buf[PKTBUFSRX][PKTSIZE_ALIGN];
};
#define to_stm_eth(_nd)	container_of(_nd, struct stm_eth_dev, netdev)

/*
 * Ethernet GPIOs:
 *				       STM32F7-SOM	STM32F746-DISCO	STM32F769I-DISCO
 * ETH_MII_RX_CLK/ETH_RMII_REF_CLK---> PA1		=		=
 * ETH_MDIO -------------------------> PA2		=		=
 * ETH_MII_RX_DV/ETH_RMII_CRS_DV ----> PA7		=		=
 * ETH_PPS_OUT ----------------------> PB5		-		-
 * ETH_MII_TXD3 ---------------------> PB8		-		-
 * ETH_MDC --------------------------> PC1		=		=
 * ETH_MII_TXD2 ---------------------> PC2		-		-
 * ETH_MII_TX_CLK -------------------> PC3		-		-
 * ETH_MII_RXD0/ETH_RMII_RXD0 -------> PC4		=		=
 * ETH_MII_RXD1/ETH_RMII_RXD1 -------> PC5		=		=
 * ETH_MII_TX_EN/ETH_RMII_TX_EN -----> PG11		=		=
 * ETH_MII_TXD0/ETH_RMII_TXD0 -------> PG13		=		=
 * ETH_MII_TXD1/ETH_RMII_TXD1 -------> PG14		=		=
 * ETH_MII_CRS ----------------------> PH2		-		-
 * ETH_MII_COL ----------------------> PH3		-		-
 * ETH_MII_RXD2 ---------------------> PH6		-		-
 * ETH_MII_RXD3 ---------------------> PH7		-		-
 * ETH_MII_RX_ER --------------------> PI10		PG2		PD5
 */
static struct stm32f2_gpio_dsc mac_gpio[] = {
	{STM32F2_GPIO_PORT_A, 1},
	{STM32F2_GPIO_PORT_A, 2},
	{STM32F2_GPIO_PORT_A, 7},

#ifndef CONFIG_STM32_ETH_RMII
	{STM32F2_GPIO_PORT_B, 5},
	{STM32F2_GPIO_PORT_B, 8},
#endif

	{STM32F2_GPIO_PORT_C, 1},
#ifndef CONFIG_STM32_ETH_RMII
	{STM32F2_GPIO_PORT_C, 2},
	{STM32F2_GPIO_PORT_C, 3},
#endif
	{STM32F2_GPIO_PORT_C, 4},
	{STM32F2_GPIO_PORT_C, 5},

	{STM32F2_GPIO_PORT_G, 11},
	{STM32F2_GPIO_PORT_G, 13},
	{STM32F2_GPIO_PORT_G, 14},

#ifndef CONFIG_STM32_ETH_RMII
	{STM32F2_GPIO_PORT_H, 2},
	{STM32F2_GPIO_PORT_H, 3},
	{STM32F2_GPIO_PORT_H, 6},
	{STM32F2_GPIO_PORT_H, 7},

/* ETH_MII_RX_ER is different on STM32F7-{SOM,DISCO} */
#ifndef CONFIG_SYS_STM32F7_DISCO
	{STM32F2_GPIO_PORT_I, 10}
#elif defined(CONFIG_SYS_STM32F769I_DISCO)
	{STM32F2_GPIO_PORT_D, 5}
#else
	{STM32F2_GPIO_PORT_G, 2}
#endif

#endif
};

/*
 * Prototypes
 */
static  s32 stm_eth_init(struct eth_device *dev, bd_t *bd);
static  s32 stm_eth_send(struct eth_device *dev, volatile void *pkt, s32 len);
static  s32 stm_eth_recv(struct eth_device *dev);
static void stm_eth_halt(struct eth_device *dev);

static  s32 stm_phy_write(struct stm_eth_dev *mac, u16 reg, u16 val);
static  s32 stm_phy_read(struct stm_eth_dev *mac, u16 reg, u16 *val);

/*
 * Initialize driver
 */
s32 stm32_eth_init(bd_t *bd)
{
	struct stm_eth_dev	*mac;
	struct eth_device	*netdev;
	s32			rv;
	u32			val;

	mac = malloc(sizeof(struct stm_eth_dev));
	if (!mac) {
		printf("Error: failed to allocate %dB of memory for %s\n",
			sizeof(struct stm_eth_dev), STM32_MAC_NAME);
		rv = -ENOMEM;
		goto out;
	}
	memset(mac, 0, sizeof(struct stm_eth_dev));

	/*
	 * Enable SYSCFG clock
	 */
	STM32_RCC->apb2enr |= STM32_RXX_ENR_SYSCFG;

	/*
	 * Set MII mode
	 */
	val = STM32_SYSCFG->pmc;
	val &= ~(STM32_SYSCFG_PMC_SEL_MSK << STM32_SYSCFG_PMC_SEL_BIT);
#ifndef CONFIG_STM32_ETH_RMII
	val |= STM32_SYSCFG_PMC_SEL_MII << STM32_SYSCFG_PMC_SEL_BIT;
#else
	val |= STM32_SYSCFG_PMC_SEL_RMII << STM32_SYSCFG_PMC_SEL_BIT;
#endif
	STM32_SYSCFG->pmc = val;

	netdev = &mac->netdev;

	/*
	 * Map registers
	 */
	netdev->iobase = STM32_MAC_BASE;

	/*
	 * Autodetect PHY
	 */
	mac->phy_adr = 0;
	mac->phy_id  = 0xFF;

	sprintf(netdev->name, STM32_MAC_NAME);

	netdev->init = stm_eth_init;
	netdev->halt = stm_eth_halt;
	netdev->send = stm_eth_send;
	netdev->recv = stm_eth_recv;

	rv = eth_register(netdev);
out:
	if (rv != 0 && mac)
		free(mac);

	return rv;
}

/*
 * Initialize PHY
 */
static s32 stm_phy_init(struct stm_eth_dev *mac)
{
	s32	i, rv;
	u16	val;

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
		rv = stm_phy_read(mac, PHY_PHYIDR1, &val);
		if (rv != 0 || val == 0xFFFF || val == 0)
			continue;

		mac->phy_id = (val & 0xFFFF) << 16;
		rv = stm_phy_read(mac, PHY_PHYIDR2, &val);
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

#ifdef CONFIG_KSZ8081_RMII_FORCE
	/*
	 * If exiting from PHY power-off state on STM32F7-SOM (KSZ8051 phy) we
	 * may sometimes have 16h.0 (MII Override) bit set. Obviously, this
	 * results to an incorrect PHY operating. So, we always
	 * set the necessary override val (16h.1 RMII Override) here
	 */
	rv = stm_phy_read(mac, PHY_PCSR, &val);
	if (rv == 0) {
		val &= ~(1 << 0);	/* Clear MII */
		val |=  (1 << 1);	/* Set RMII  */
		stm_phy_write(mac, PHY_PCSR, val);
	}
#endif

	rv = 0;
out:
	return rv;
}

/*
 * Get link status
 */
static s32 stm_phy_link_get(struct stm_eth_dev *mac,
			    s32 *link_up, s32 *full_dup, s32 *speed)
{
	u16	val;
	s32	rv;

	rv = stm_phy_read(mac, PHY_BMSR, &val);
	if (rv != 0)
		goto out;
	*link_up  = (val & PHY_BMSR_LS) ? 1 : 0;

	rv = stm_phy_read(mac, PHY_BMCR, &val);
	if (rv != 0)
		goto out;
	*full_dup = (val & PHY_BMSR_EXT_STAT) ? 1 : 0;
	*speed    = (val & (PHY_BMSR_100TXH | PHY_BMSR_100TXF)) ? 100 : 10;

	rv = 0;
out:
	return rv;
}

/*
 * Setup link status
 */
static s32 stm_phy_link_setup(struct stm_eth_dev *mac)
{
	static s32	link_inited;

	s32		link_up, full_dup, speed, rv, timeout;
	u32		cr_val;
	u16		val;

	/*
	 * Get link status
	 */
	rv = stm_phy_link_get(mac, &link_up, &full_dup, &speed);
	if (rv != 0)
		goto out;

	/*
	 * Force auto-negotiation procedure on each U-Boot start. If
	 * CPU had been reseted (with 'RESET' button), but PHY didn't,
	 * then because of GPIO reinitialization - the sync with PHY may
	 * be lost, and the very first frame sent to PHY will be lost as
	 * well. Autonegotiation procedure fixes this, so at very first
	 * time after start we force it.
	 */
	if (link_up && link_inited)
		goto link_set;

	/*
	 * Enable auto-negotiation
	 */
	printf("Auto-negotiation...");
	rv = stm_phy_read(mac, PHY_BMCR, &val);
	if (rv != 0)
		goto out;
	rv = stm_phy_write(mac, PHY_BMCR,
			   val | PHY_BMCR_AUTON | PHY_BMCR_RST_NEG);
	if (rv != 0)
		goto out;

	/*
	 * Wait until auto-negotioation complete
	 */
	timeout = STM32_PHY_AUTONEG_TIMEOUT;
	while (timeout-- > 0) {
		if (stm_phy_read(mac, PHY_BMSR, &val) == 0 &&
		    (val & PHY_BMSR_AUTN_COMP))
			timeout = 0;
		else
			udelay(100);
	}
	if (val & PHY_BMSR_AUTN_COMP)
		printf("completed.\n");
	else
		printf("timeout.\n");

	/*
	 * Get link status
	 */
	rv = stm_phy_link_get(mac, &link_up, &full_dup, &speed);
	if (rv != 0)
		goto out;
	if (!link_up) {
		printf("Link is DOWN.\n");
		rv = -ENETUNREACH;
		goto out;
	}

link_set:
	cr_val = STM32_MAC->maccr;
	printf("%s: link UP ", mac->netdev.name);
	if (speed == 100) {
		printf("(100/");
		cr_val |= STM32_MAC_CR_FES;
	} else {
		printf("(10/");
		cr_val &= ~STM32_MAC_CR_FES;
	}

	if (full_dup) {
		printf("Full)\n");
		cr_val |= STM32_MAC_CR_DM;
	} else {
		printf("Half)\n");
		cr_val &= ~STM32_MAC_CR_DM;
	}
	STM32_MAC->maccr = cr_val;

	link_inited = 1;
	rv = 0;
out:
	return rv;
}

/*
 * Helper function used in stm_phy_read() and stm_phy_write()
 */
static int stm_phy_wait_busy(int timeout)
{
	int rv;

	rv = -ETIMEDOUT;
	while (timeout-- > 0) {
		if (STM32_MAC->macmiiar & STM32_MAC_MIIAR_MB) {
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
static s32 stm_phy_write(struct stm_eth_dev *mac, u16 reg, u16 val)
{
	u16	adr = mac->phy_adr;
	u32	tmp;
	s32	rv;

	if (stm_phy_wait_busy(STM32_PHY_WRITE_TIMEOUT) != 0) {
		/*
		 * MII is busy
		 */
		rv = -EBUSY;
		goto out;
	}

	/*
	 * Prepare MII register address value:
	 * - keep CR;
	 * - set PHY device address
	 * - set PHY register address
	 * - set write mode
	 * - set MII Busy
	 */
	tmp = STM32_MAC->macmiiar;
	tmp &= STM32_MAC_MIIAR_CR_MSK << STM32_MAC_MIIAR_CR_BIT;

	adr &= STM32_MAC_MIIAR_PA_MSK;
	tmp |= adr << STM32_MAC_MIIAR_PA_BIT;

	reg &= STM32_MAC_MIIAR_MR_MSK;
	tmp |= reg << STM32_MAC_MIIAR_MR_BIT;

	tmp |= STM32_MAC_MIIAR_MW | STM32_MAC_MIIAR_MB;

	/*
	 * Write to regs, and wait for completion
	 */
	STM32_MAC->macmiidr = val;
	STM32_MAC->macmiiar = tmp;

	if (stm_phy_wait_busy(STM32_PHY_WRITE_TIMEOUT) != 0) {
		/*
		 * Transaction failed: Write timeout
		 */
		rv = -ETIMEDOUT;
		goto out;
	}

	/*
	 * Transaction OK
	 */
	rv = 0;
out:
	return rv;
}

/*
 * Read PHY
 */
static s32 stm_phy_read(struct stm_eth_dev *mac, u16 reg, u16 *val)
{
	u16	adr = mac->phy_adr;
	u32	tmp;
	s32	rv;

	if (stm_phy_wait_busy(STM32_PHY_READ_TIMEOUT) != 0) {
		/*
		 * MII is busy
		 */
		rv = -EBUSY;
		goto out;
	}

	/*
	 * Prepare MII register address value:
	 * - keep CR;
	 * - set PHY device address
	 * - set PHY register address
	 * - set read mode
	 * - set MII Busy
	 */
	tmp = STM32_MAC->macmiiar;
	tmp &= STM32_MAC_MIIAR_CR_MSK << STM32_MAC_MIIAR_CR_BIT;

	adr &= STM32_MAC_MIIAR_PA_MSK;
	tmp |= adr << STM32_MAC_MIIAR_PA_BIT;

	reg &= STM32_MAC_MIIAR_MR_MSK;
	tmp |= reg << STM32_MAC_MIIAR_MR_BIT;

	tmp |= STM32_MAC_MIIAR_MB;

	/*
	 * Write to reg, and wait for completion
	 */
	STM32_MAC->macmiiar  = tmp;

	if (stm_phy_wait_busy(STM32_PHY_READ_TIMEOUT) != 0) {
		/*
		 * Transaction failed: read timeout
		 */
		rv = -ETIMEDOUT;
		goto out;
	}

	/*
	 * Transaction OK
	 */
	*val = STM32_MAC->macmiidr;

	rv = 0;
out:
	return rv;
}

/*
 * Init STM32 MAC buffer descriptors
 */
static void stm_mac_bd_init(struct stm_eth_dev *mac)
{
	s32	i;

	/*
	 * Init Tx buffer descriptor
	 */
	mac->tx_bd.stat = STM32_DMA_TBD_TCH;
	mac->tx_bd.ctrl = 0;
	mac->tx_bd.buf  = NULL;
	mac->tx_bd.next = &mac->tx_bd;

	/*
	 * Init Rx buffer descriptors
	 */
	for (i = 0; i < PKTBUFSRX; i++) {
		mac->rx_bd[i].stat = STM32_DMA_RBD_DMA_OWN;
		mac->rx_bd[i].ctrl = STM32_DMA_RBD_RCH | PKTSIZE_ALIGN;
		mac->rx_bd[i].buf  = &mac->rx_buf[i][0];
		mac->rx_bd[i].next = &mac->rx_bd[(i + 1) % PKTBUFSRX];
	}

	/*
	 * Set our internal rx bd pointer to start
	 */
	mac->rx_bd_idx = 0;

	/*
	 * Program DMA with the addresses of descriptor chains
	 */
	STM32_MAC->dmatdlar = (u32)&mac->tx_bd;
	STM32_MAC->dmardlar = (u32)&mac->rx_bd[0];
}

/*
 * Set MAC address
 */
static void stm_mac_address_set(struct stm_eth_dev *mac)
{
	struct eth_device	*netdev = &mac->netdev;

	debug("%s: mac is %#x:%#x:%#x:%#x:%#x:%#x.\n", __func__,
	      netdev->enetaddr[0], netdev->enetaddr[1],
	      netdev->enetaddr[2], netdev->enetaddr[3],
	      netdev->enetaddr[4], netdev->enetaddr[5]);

	STM32_MAC->maca0hr = (netdev->enetaddr[5] <<  8) |
			     (netdev->enetaddr[4] <<  0);
	STM32_MAC->maca0lr = (netdev->enetaddr[3] << 24) |
			     (netdev->enetaddr[2] << 16) |
			     (netdev->enetaddr[1] <<  8) |
			     (netdev->enetaddr[0] <<  0);
}

/*
 * Init GPIOs used by MAC
 */
static s32 stm_mac_gpio_init(struct stm_eth_dev *mac)
{
	static s32	gpio_inited;

	s32		i, rv;

	/*
	 * Init GPIOs only once at start. Otherwise, reiniting then on
	 * each halt/init call from u-boot Net subsystem we may loose
	 * the very first frame sending to net: MAC in this case reports
	 * that frame had been successfully sent, but there is no frame
	 * on wires. Probably, some synchronization with PHY is lost if
	 * we do this GPIO re-initialization.
	 */
	if (gpio_inited) {
		rv = 0;
		goto out;
	}

	/*
	 * Set GPIOs Alternative function
	 */
	for (i = 0; i < sizeof(mac_gpio)/sizeof(mac_gpio[0]); i++) {
		rv = stm32f2_gpio_config(&mac_gpio[i],
					 STM32F2_GPIO_ROLE_ETHERNET);
		if (rv != 0)
			goto out;
	}

	gpio_inited = 1;
	rv = 0;
out:
	return rv;
}

/*
 * Init STM32 MAC hardware
 */
static s32 stm_mac_hw_init(struct stm_eth_dev *mac)
{
	u32	tmp, hclk;
	s32	rv, timeout;

	/*
	 * Init GPIOs
	 */
	rv = stm_mac_gpio_init(mac);
	if (rv != 0)
		goto out;

	/*
	 * Enable Ethernet clocks
	 */
	STM32_RCC->ahb1enr |= STM32_RCC_ENR_ETHMACEN   |
			      STM32_RCC_ENR_ETHMACTXEN |
			      STM32_RCC_ENR_ETHMACRXEN;

	/*
	 * Reset all MAC subsystem internal regs and logic
	 */
	STM32_RCC->ahb1rstr |= STM32_RCC_AHB1RSTR_MAC;
	STM32_RCC->ahb1rstr &= ~STM32_RCC_AHB1RSTR_MAC;

	STM32_MAC->dmabmr |= STM32_MAC_DMABMR_SR;

	timeout = STM32_MAC_INIT_TIMEOUT;
	rv = -EBUSY;
	while (timeout-- > 0) {
		if (STM32_MAC->dmabmr & STM32_MAC_DMABMR_SR) {
			udelay(100);
		} else {
			timeout = 0;
			rv = 0;
		}
	}
	if (rv != 0) {
		printf("%s: failed reset MAC subsystem.\n", __func__);
		goto out;
	}

	/*
	 * Configure DMA:
	 * - address aligned beats (32-bit aligned src & dst addresses),
	 * - fixed burst, and 32 beat max burst lengths,
	 * - round-robin DMA arbitration Rx:Tx<->2:1;
	 * - enable use of separate PBL for Rx and Tx.
	 */
	STM32_MAC->dmabmr = (32 << STM32_MAC_DMABMR_PBL_BIT) |
			    (32 << STM32_MAC_DMABMR_RDP_BIT) |
			    (STM32_MAC_DMABMR_RTPR_2_1 <<
			     STM32_MAC_DMABMR_RTPR_BIT) |
			    STM32_MAC_DMABMR_FB | STM32_MAC_DMABMR_USP |
			    STM32_MAC_DMABMR_AAB;

	/*
	 * Configure Ethernet CSR Clock Range
	 */
	hclk = clock_get(CLOCK_HCLK);
	if (hclk >= 20000000 && hclk < 35000000) {
		/* CSR Clock range between 20-35 MHz */
		tmp = STM32_MAC_MIIAR_CR_DIV16 << STM32_MAC_MIIAR_CR_BIT;
	} else if (hclk >= 35000000 && hclk < 60000000) {
		/* CSR Clock range between 35-60 MHz */
		tmp = STM32_MAC_MIIAR_CR_DIV26 << STM32_MAC_MIIAR_CR_BIT;
	} else if (hclk >= 60000000 && hclk < 100000000) {
		/* CSR Clock range between 60-100 MHz */
		tmp = STM32_MAC_MIIAR_CR_DIV42 << STM32_MAC_MIIAR_CR_BIT;
	} else {
		/* CSR Clock range between 100-180 MHz */
		tmp = STM32_MAC_MIIAR_CR_DIV62 << STM32_MAC_MIIAR_CR_BIT;
	}

	if (stm_phy_wait_busy(STM32_PHY_READ_TIMEOUT) != 0) {
		/*
		 * MII is busy
		 */
		rv = -EBUSY;
		goto out;
	}
	STM32_MAC->macmiiar = tmp;

	/*
	 * Init PHY
	 */
	rv = stm_phy_init(mac);
	if (rv != 0)
		goto out;

	/*
	 * Setup link, and complete MAC initialization
	 */
	rv = stm_phy_link_setup(mac);
	if (rv != 0)
		goto out;
out:
	return rv;
}

/*
 * Init STM32 MAC and DMA
 */
static s32 stm_eth_init(struct eth_device *dev, bd_t *bd)
{
	struct stm_eth_dev	*mac = to_stm_eth(dev);
	s32			rv;

	/*
	 * Init hw
	 */
	rv = stm_mac_hw_init(mac);
	if (rv != 0)
		goto out;

	/*
	 * Set MAC address
	 */
	stm_mac_address_set(mac);

	/*
	 * Init buffer descriptors
	 */
	stm_mac_bd_init(mac);

	/*
	 * Enable TX
	 */
	STM32_MAC->maccr |= STM32_MAC_CR_TE;

	/*
	 * Flush Transmit FIFO
	 */
	STM32_MAC->dmaomr |= STM32_MAC_DMAOMR_FTF;
	while (STM32_MAC->dmaomr & STM32_MAC_DMAOMR_FTF);

	/*
	 * Enable RX
	 */
	STM32_MAC->maccr |= STM32_MAC_CR_RE;

	/*
	 * Start DMA TX and RX
	 */
	STM32_MAC->dmaomr |= STM32_MAC_DMAOMR_ST;
	STM32_MAC->dmaomr |= STM32_MAC_DMAOMR_SR;
out:
	if (rv != 0)
		printf("%s: failed (%d).\n", __func__, rv);
	return rv;
}

/*
 * Send frame
 */
static s32 stm_eth_send(struct eth_device *dev, volatile void *pkt, s32 len)
{
	struct stm_eth_dev	*mac = to_stm_eth(dev);
	s32			rv, tout;

	if (len > PKTSIZE_ALIGN) {
		printf("%s: frame too long (%d).\n", __func__, len);
		rv = -EINVAL;
		goto out;
	}

	/*
	 * Make sure nothing is txing now
	 */
	if (mac->tx_bd.stat & STM32_DMA_TBD_DMA_OWN) {
		printf("%s: busy.\n", __func__);
		rv = -EBUSY;
		goto out;
	}

	/*
	 * Set up BD
	 */
	mac->tx_bd.buf   = pkt;
	mac->tx_bd.ctrl  = len;
	mac->tx_bd.stat |= STM32_DMA_TBD_FS | STM32_DMA_TBD_LS |
			   STM32_DMA_TBD_DMA_OWN;

	/*
	 * If Tx buffer unavailable flag is set, then clear it and resume
	 */
	if (STM32_MAC->dmasr & STM32_MAC_DMASR_TBUS) {
		STM32_MAC->dmasr &= ~STM32_MAC_DMASR_TBUS;
		STM32_MAC->dmatpdr = 0;
	}

	/*
	 * Wait until transmit completes
	 */
	tout = STM32_MAC_TX_TIMEOUT;
	rv = -ETIMEDOUT;
	while (tout-- > 0) {
		if (mac->tx_bd.stat & STM32_DMA_TBD_DMA_OWN)
			udelay(1);
		else {
			tout = 0;
			rv = 0;
		}
	}
	if (rv != 0) {
		printf("%s: timeout.\n", __func__);
		goto out;
	}

	/*
	 * Tx done.
	 */
	rv = 0;
out:
	return rv;
}

/*
 * Process received frames (if any)
 */
static s32 stm_eth_recv(struct eth_device *dev)
{
	volatile struct stm_eth_dma_bd	*bd;
	struct stm_eth_dev		*mac = to_stm_eth(dev);
	u32				len;

	/*
	 * Walk through the list of rx bds and process rxed frames until
	 * detect BD owned by DMA
	 */
	while (!(mac->rx_bd[mac->rx_bd_idx].stat & STM32_DMA_RBD_DMA_OWN)) {
		bd = &mac->rx_bd[mac->rx_bd_idx];

		/*
		 * RX buf size we use should be enough for storing the whole
		 * ethernet frame with checksum (1518), so the following
		 * shouldn't happen
		 */
		if ((bd->stat & (STM32_DMA_RBD_FS | STM32_DMA_RBD_LS)) !=
		    (STM32_DMA_RBD_FS | STM32_DMA_RBD_LS)) {
			printf("%s: warn, frame split (0x%08x).\n", __func__,
				bd->stat);
		}

		/*
		 * Get length, and take 4 CRC bytes into account
		 */
		len  = (bd->stat >> STM32_DMA_RBD_FL_BIT) &
		       STM32_DMA_RBD_FL_MSK;
		len -= 4;

		/*
		 * Pass frame upper
		 */
		NetReceive(bd->buf, len);

		/*
		 * Mark BD as ready for rx again, and switch to the next BD
		 */
		bd->stat = STM32_DMA_RBD_DMA_OWN;
		mac->rx_bd_idx = (mac->rx_bd_idx + 1) % PKTBUFSRX;

		/*
		 * If rx buf unavailable flag is set, clear it and resume
		 * reception
		 */
		if (STM32_MAC->dmasr & STM32_MAC_DMASR_RBUS) {
			/*
			 * This is actually overflow, frame(s) lost
			 */
			printf("%s: RX overflow.\n", __func__);
			STM32_MAC->dmasr &= ~STM32_MAC_DMASR_RBUS;
			STM32_MAC->dmarpdr = 0;
		}
	}

	return 0;
}

/*
 * Halt MAC
 */
static void stm_eth_halt(struct eth_device *dev)
{
	/*
	 * Stop DMA, and disable receiver and transmitter
	 */
	STM32_MAC->dmaomr &= ~(STM32_MAC_DMAOMR_ST | STM32_MAC_DMAOMR_SR);
	STM32_MAC->maccr  &= ~(STM32_MAC_CR_TE | STM32_MAC_CR_RE);
}
