/*
 * Copyright (C) 2012 Vladimir Khusainov, Emcraft Systems, vlad@emcraft.com
 * Copyright (C) 2012 Yuri Tikhonov, Emcraft Systems, yur@emcraft.com
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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
#include <common.h>
#include <spi.h>
#include <malloc.h>
#include <clock.h>

/*
 * Debug output control. While debugging, have SPI_M2S_DEBUG defined.
 * In deployment, make sure that SPI_M2S_DEBUG is undefined
 * to avoid performance and size overhead of debug messages.
 */
#if defined(CONFIG_SYS_M2S_SPI_DEBUG)
#define SPI_M2S_DEBUG
#endif

/*
 * Base address of the MSS-based SPI controllers
 */
#define MSS_SPI0_REGS			0x40001000
#define MSS_SPI1_REGS			0x40011000

/*
 * Base address of PDMA controller
 */
#define MSS_PDMA_REGS			0x40003000

/*
 * Some bits in various regs
 */
#define M2S_SYS_SOFT_RST_CR_SPI1	(1 << 10)
#define M2S_SYS_SOFT_RST_CR_SPI0	(1 << 9)
#define M2S_SYS_SOFT_RST_CR_PDMA	(1 << 5)

#define SPI_CONTROL_ENABLE		(1 << 0)
#define SPI_CONTROL_MASTER		(1 << 1)
#define SPI_CONTROL_PROTO_MSK		(3 << 2)
#define SPI_CONTROL_PROTO_MOTO		(0 << 2)
#define SPI_CONTROL_CNT_MSK		(0xffff << 8)
#define SPI_CONTROL_CNT_SHF		(8)
#define SPI_CONTROL_SPO			(1 << 24)
#define SPI_CONTROL_SPH			(1 << 25)
#define SPI_CONTROL_SPS			(1 << 26)
#define SPI_CONTROL_BIGFIFO		(1 << 29)
#define SPI_CONTROL_CLKMODE		(1 << 28)
#define SPI_CONTROL_RESET		(1 << 31)

/*
 * PDMA register bits
 */
#define PDMA_CONTROL_PER_SEL_SPI0_RX	(0x4 << 23)
#define PDMA_CONTROL_PER_SEL_SPI0_TX	(0x5 << 23)
#define PDMA_CONTROL_PER_SEL_SPI1_RX	(0x6 << 23)
#define PDMA_CONTROL_PER_SEL_SPI1_TX	(0x7 << 23)
/*
 * TBD: calculte ADJ value dynamically, basing on SPI clk value?
 * With smaller values we just hang-up
 */
#define PDMA_CONTROL_WRITE_ADJ		(0xF << 14)

#define PDMA_CONTROL_DST_ADDR_INC_MSK	(0x3 << 12)
#define PDMA_CONTROL_DST_ADDR_INC_0	(0x0 << 12)
#define PDMA_CONTROL_DST_ADDR_INC_1	(0x1 << 12)
#define PDMA_CONTROL_SRC_ADDR_INC_MSK	(0x3 << 10)
#define PDMA_CONTROL_SRC_ADDR_INC_0	(0x0 << 10)
#define PDMA_CONTROL_SRC_ADDR_INC_1	(0x1 << 10)
#define PDMA_CONTROL_CLR_B		(1 << 8)
#define PDMA_CONTROL_CLR_A		(1 << 7)
#define PDMA_CONTROL_RESET		(1 << 5)
#define PDMA_CONTROL_PAUSE		(1 << 4)
#define PDMA_CONTROL_XFER_SIZE_1B	(0x0 << 2)
#define PDMA_CONTROL_DIR		(1 << 1)
#define PDMA_CONTROL_PERIPH		(1 << 0)

#define PDMA_STATUS_BUF_SEL		(1 << 2)

/*
 * Access handle for the control registers
 */
#define MSS_SPI_REGS(regs)	((volatile struct mss_spi *)(regs))
#define MSS_SPI(s)		(MSS_SPI_REGS(s->regs))
#define MSS_PDMA		((volatile struct mss_pdma *)MSS_PDMA_REGS)

/*
 * Service to print debug messages
 */
#if defined(SPI_M2S_DEBUG)
# define d_printk(level, fmt, args...)					\
	if (spi_m2s_debug >= level) printf("%s: " fmt, __func__, ## args)
#else
# define d_printk(level, fmt, args...)
#endif

/*
 * Private data structure for an SPI slave
 */
struct m2s_spi_slave {
	struct spi_slave	slave;		/* Generic slave */
	void			*regs;		/* Registers base */
	u32			hz;		/* SPI bus rate */
	u32			hb;		/* SPI base clock */
	u32			mode;		/* SPI bus mode */

	u8			drx;		/* Rx PDMA channel */
	u8			dtx;		/* Tx PDMA channel */

	u32			rst_clr;	/* RESET CLR mask */
	u32			drx_sel;	/* RX DMA peripheral */
	u32			dtx_sel;	/* TX DMA peripheral */
};

/*
 * Description of the the SmartFusion SPI hardware interfaces.
 * This is a 1-to-1 mapping of Actel's documenation onto a C structure.
 * Refer to SmartFusion Data Sheet for details.
 */
struct mss_spi {
	u32	control;
	u32	txrxdf_size;
	u32	status;
	u32	int_clear;
	u32	rx_data;
	u32	tx_data;
	u32	clk_gen;
	u32	slave_select;
	u32	mis;
	u32	ris;
};

 /*
  * Peripheral DMA registers
  */
struct mss_pdma {
	u32	ratio;
	u32	status;
	u32	reserved[(0x20 - 0x08) >> 2];
	struct mss_pdma_chan {
		u32	control;
		u32	status;
		struct {
			u32	src;
			u32	dst;
			u32	cnt;
		} buf[2];			/* Buffers A-B */
	} chan[8];				/* Channels 0-7 */
};


#if defined(SPI_M2S_DEBUG)
/*
 * Driver verbosity level: 0->silent; >0->verbose (1 to 4, growing verbosity)
 */
static int spi_m2s_debug = 4;
#endif

/*
 * PDMA usage counter
 */
static int pdma_used;

/*
 * Handler to get access to the driver specific slave data structure
 * @param c		generic slave
 * @returns		driver specific slave
 */
static inline struct m2s_spi_slave *to_m2s_spi(struct spi_slave *slave)
{
	return container_of(slave, struct m2s_spi_slave, slave);
}

#if defined(SPI_M2S_DEBUG)
/*
 * Dump PDMA registers
 */
static void pdma_dump(char *comment, u8 drx, u8 dtx)
{
	printf("DMA %s. status=0x%08x\n", comment, MSS_PDMA->status);
	printf(" rx %d. status=0x%08x\n", drx, MSS_PDMA->chan[drx].status);
	printf("  A: src=0x%08x,dst=0x%08x,cnt=0x%08x\n",
		MSS_PDMA->chan[drx].buf[0].src, MSS_PDMA->chan[drx].buf[0].dst,
		MSS_PDMA->chan[drx].buf[0].cnt);
	printf("  B: src=0x%08x,dst=0x%08x,cnt=0x%08x\n",
		MSS_PDMA->chan[drx].buf[1].src, MSS_PDMA->chan[drx].buf[1].dst,
		MSS_PDMA->chan[drx].buf[1].cnt);
	printf(" tx %d. status=0x%08x\n", dtx, MSS_PDMA->chan[dtx].status);
	printf("  A: src=0x%08x,dst=0x%08x,cnt=0x%08x\n",
		MSS_PDMA->chan[dtx].buf[0].src, MSS_PDMA->chan[dtx].buf[0].dst,
		MSS_PDMA->chan[dtx].buf[0].cnt);
	printf("  B: src=0x%08x,dst=0x%08x,cnt=0x%08x\n",
		MSS_PDMA->chan[dtx].buf[1].src, MSS_PDMA->chan[dtx].buf[1].dst,
		MSS_PDMA->chan[dtx].buf[1].cnt);
}
#endif

/*
 * Set chip select
 * @param s		slave
 * @param cs		chip select: [0..7]->slave, otherwise->deselect all
 * @returns		0->good,!=0->bad
 */
static inline int spi_m2s_hw_cs_set(struct m2s_spi_slave *s, int cs)
{
	unsigned int v = (0 <= cs && cs <= 7) ? (1 << cs) : 0;
	int ret = 0;

	MSS_SPI(s)->slave_select = v;

	d_printk(3, "bus=%d,cs=%d,slave_select=0x%x,ret=%d\n",
		 s->slave.bus, cs, MSS_SPI(s)->slave_select, ret);
	return ret;
}

/*
 * Set controller clock rate
 * @param s		slave
 * @param spd		clock rate in Hz
 * @returns		0->good,!=0->bad
 */
static inline int spi_m2s_hw_clk_set(struct m2s_spi_slave *s, unsigned int spd)
{
	int i;
	unsigned int h;
	int ret = 0;

	/*
	 * Calculate the clock rate that works for this slave
	 */
	h = s->hb;
	for (i = 0; i <= 255; i++) {
		if (h / ((i + 1) << 1) <= spd)
			break;
	}

	/*
	 * Can't provide a rate that is slow enough for the slave
	 */
	if (i > 255) {
		ret = -1;
		goto done;
	}

	/*
	 * Set the clock rate
	 */
	MSS_SPI(s)->clk_gen = i;

done:
	d_printk(3, "bus=%d,cnt_hz=%d,slv_hz=%d,rsl_hz=%d,clk_gen=%d,ret=%d\n",
		s->slave.bus, h, spd, h / ((i + 1) << 1),
		MSS_SPI(s)->clk_gen, ret);

	return ret;
}

/*
 * Set frame size (making an assumption that the supplied size is
 * supported by this controller)
 * @param s		slave
 * @param bt		frame size
 * @returns		0->good,!=0->bad
 */
static inline int spi_m2s_hw_bt_set(struct m2s_spi_slave *s, int bt)
{
	int ret = 0;

	/*
	 * Disable the SPI contoller. Writes to data frame size have
	 * no effect when the controller is enabled.
	 */
	MSS_SPI(s)->control &= ~SPI_CONTROL_ENABLE;

	/*
	 * Set the new data frame size.
	 */
	MSS_SPI(s)->txrxdf_size = bt;

	/*
	 * Re-enable the SPI contoller
	 */
	MSS_SPI(s)->control |= SPI_CONTROL_ENABLE;

	d_printk(3, "bus=%d,bt=%d,txrxdf_size=%d,ret=%d\n",
		 s->slave.bus, bt, MSS_SPI(s)->txrxdf_size, ret);
	return ret;
}

/*
 * Set transfer length
 * @param s		slave
 * @param len		transfer size
 */
static inline void spi_m2s_hw_tfsz_set(struct m2s_spi_slave *s, int len)
{
	/*
	 * Disable the SPI contoller. Writes to transfer length have
	 * no effect when the controller is enabled.
	 */
	MSS_SPI(s)->control &= ~SPI_CONTROL_ENABLE;

	/*
	 * Set the new data frame size.
	 */
	MSS_SPI(s)->control &= ~SPI_CONTROL_CNT_MSK;
	MSS_SPI(s)->control |= len << SPI_CONTROL_CNT_SHF;

	/*
	 * Re-enable the SPI contoller
	 */
	MSS_SPI(s)->control |= SPI_CONTROL_ENABLE;

	d_printk(3, "bus=%d,len=%d\n", s->slave.bus, len);
}

/*
 * Set SPI mode
 * @param s		slave
 * @param mode		mode
 * @returns		0->good;!=0->bad
 */
static inline int spi_m2s_hw_mode_set(struct m2s_spi_slave *s,
				      unsigned int mode)
{
	int ret = 0;

	/*
	 * Set the mode
	 */
	if (mode & SPI_CPHA)
		MSS_SPI(s)->control |= SPI_CONTROL_SPH;
	else
		MSS_SPI(s)->control &= ~SPI_CONTROL_SPH;

	if (mode & SPI_CPOL)
		MSS_SPI(s)->control |= SPI_CONTROL_SPO;
	else
		MSS_SPI(s)->control &= ~SPI_CONTROL_SPO;

	d_printk(2, "bus=%d,mode=%x,control=%x,ret=%d\n",
		 s->slave.bus, mode, MSS_SPI(s)->control, ret);

	return ret;
}

/*
 * Initialization of the entire driver
 */
void spi_init()
{
	d_printk(1, "ok\n");
}

/*
 * Prepare to use an SPI slave
 * @param b		SPI controller
 * @param cs		slave Chip Select
 * @param hz		max freq this slave can run at
 * @param m		slave access mode
 * @returns		driver specific slave
 */
struct spi_slave *spi_setup_slave(unsigned int b, unsigned int cs,
				  unsigned int hz, unsigned int m)
{
	struct m2s_spi_slave	*s;
	struct spi_slave	*slv = NULL;

	/*
	 * Validate input parameters. Can be anything since this is
	 * part of user build-time configuration.
	 */
	if (! (0 <= b && b <= 1)) {
		goto done;
	}
	if (! (0 <= cs && cs <= 7)) {
		goto done;
	}

	/*
	 * Allocate the driver-specific slave data structure
	 */
	s = malloc(sizeof(struct m2s_spi_slave));
	if (!s) {
		goto done;
	}

	/*
	 * Fill in the driver-specific slave data structure
	 */
	s->slave.bus = b;
	s->slave.cs = cs;
	s->mode = m;
	s->hz = hz;
	slv = &s->slave;

	if (b == 0) {
		s->regs = (void *)MSS_SPI0_REGS;
		s->hb = clock_get(CLOCK_PCLK0);

		s->drx = 0;
		s->dtx = 1;
		s->drx_sel = PDMA_CONTROL_PER_SEL_SPI0_RX;
		s->dtx_sel = PDMA_CONTROL_PER_SEL_SPI0_TX;

		s->rst_clr = M2S_SYS_SOFT_RST_CR_SPI0;
	} else {
		s->regs = (void *)MSS_SPI1_REGS;
		s->hb = clock_get(CLOCK_PCLK1);

		s->drx = 2;
		s->dtx = 3;
		s->drx_sel = PDMA_CONTROL_PER_SEL_SPI1_RX;
		s->dtx_sel = PDMA_CONTROL_PER_SEL_SPI1_TX;

		s->rst_clr = M2S_SYS_SOFT_RST_CR_SPI1;
	}

	d_printk(2, "bus=%d,regs=%p,cs=%d,hz=%d,mode=0x%x\n",
		b, s->regs, cs, hz, m);

done :
	d_printk(2, "slv=%p\n", slv);
	return slv;
}

/*
 * Done with an an SPI slave
 * @param slv		SPI slave
 */
void spi_free_slave(struct spi_slave *slv)
{
	struct m2s_spi_slave *s = to_m2s_spi(slv);

	/*
	 * Release the driver-specific slave data structure
	 */
	free(s);

	d_printk(2, "slv=%p\n", slv);
}

/*
 * Set up the SPI controller
 * @param slv		SPI slave
 * @returns		0->success; !0->failure
 */
int spi_claim_bus(struct spi_slave *slv)
{
	unsigned int ret = 0;
	struct m2s_spi_slave *s = to_m2s_spi(slv);

	/*
	 * Reset the MSS SPI controller and then bring it out of reset
	 */
	M2S_SYSREG->soft_reset_cr |= s->rst_clr;
	M2S_SYSREG->soft_reset_cr &= ~s->rst_clr;

	/*
	 * Set the master mode
	 */
	MSS_SPI(s)->control |= SPI_CONTROL_MASTER;

	/*
	 * Set the transfer protocol. We are using the Motorola
	 * SPI mode, with no user interface to configure it to
	 * some other mode.
	 */
	MSS_SPI(s)->control &= ~SPI_CONTROL_PROTO_MSK;
	MSS_SPI(s)->control |= SPI_CONTROL_PROTO_MOTO;

	/*
	 * Set-up the controller in such a way that it doesn't remove
	 * Chip Select until the entire message has been transferred,
	 * even if at some points TX FIFO becomes empty.
	 */
	MSS_SPI(s)->control |= SPI_CONTROL_SPS | SPI_CONTROL_BIGFIFO |
			       SPI_CONTROL_CLKMODE;

	/*
	 * Enable the SPI contoller
	 * It is critical to clear RESET in the control bit.
	 */
	MSS_SPI(s)->control &= ~SPI_CONTROL_RESET;
	MSS_SPI(s)->control |= SPI_CONTROL_ENABLE;

	/*
	 * Configure DMA (preliminary)
	 */
	if (!pdma_used++)
		M2S_SYSREG->soft_reset_cr &= ~M2S_SYS_SOFT_RST_CR_PDMA;

	MSS_PDMA->chan[s->drx].control = PDMA_CONTROL_RESET |
					 PDMA_CONTROL_CLR_B |
					 PDMA_CONTROL_CLR_A;
	MSS_PDMA->chan[s->drx].control |= PDMA_CONTROL_WRITE_ADJ |
					  PDMA_CONTROL_SRC_ADDR_INC_0 |
					  PDMA_CONTROL_XFER_SIZE_1B;
	MSS_PDMA->chan[s->drx].control |= s->drx_sel |
					  PDMA_CONTROL_PERIPH;

	MSS_PDMA->chan[s->dtx].control = PDMA_CONTROL_RESET |
					 PDMA_CONTROL_CLR_B |
					 PDMA_CONTROL_CLR_A;
	MSS_PDMA->chan[s->dtx].control |= PDMA_CONTROL_WRITE_ADJ |
					  PDMA_CONTROL_DST_ADDR_INC_0 |
					  PDMA_CONTROL_XFER_SIZE_1B;
	MSS_PDMA->chan[s->dtx].control |= s->dtx_sel |
					  PDMA_CONTROL_DIR |
					  PDMA_CONTROL_PERIPH;

	d_printk(2, "bus=%d,soft_reset_cr=0x%x,control=0x%x\n",
		slv->bus, M2S_SYSREG->soft_reset_cr, MSS_SPI(s)->control);

	d_printk(2, "slv=%p\n", slv);
	return ret;
}

/*
 * Shut down the SPI controller
 * @param slv		SPI slave
 */
void spi_release_bus(struct spi_slave *slv)
{
	struct m2s_spi_slave *s = to_m2s_spi(slv);

	/*
	 * Reset DMAs
	 */
	MSS_PDMA->chan[s->drx].control = PDMA_CONTROL_RESET;
	MSS_PDMA->chan[s->dtx].control = PDMA_CONTROL_RESET;
	if (!--pdma_used)
		M2S_SYSREG->soft_reset_cr |= M2S_SYS_SOFT_RST_CR_PDMA;

	/*
	 * Disable the SPI contoller
	 */
	MSS_SPI(s)->control &= ~SPI_CONTROL_ENABLE;

	/*
	 * Put the SPI controller into reset
	 */
	M2S_SYSREG->soft_reset_cr |= s->rst_clr;

	d_printk(2, "slv=%p\n", slv);
}

/*
 * Perform an SPI transfer
 * @param slv		SPI slave
 * @param bl		transfer length (in bits)
 * @param dout		data out
 * @param din		data in
 * @param fl		transfer flags
 * @returns		0->success; !0->failure
 */
int spi_xfer(struct spi_slave *slv, unsigned int bl,
	     const void *dout, void *din, unsigned long fl)
{
	/*
	 * The SPI framework implemented in U-boot doesn't work too well
	 * for the SmartFusion SPI hardware. The U-boot implementation makes
	 * an assumption that there is always a way to activate / deactive
	 * CS under explicit software control. This is not the case for
	 * the SmartFusion; CS is controlled by the SPI controller hardware
	 * and is active as long as a current transcation is active, which is
	 * defined by the total length of the transaction, as programmed into
	 * a certain CSR.
	 * An implication is that we must calculate the size of the total
	 * transaction before we can start any transfer of that transaction.
	 * Hence the static variables below that are needed to maintain
	 * the state of things across potentially (typically, even)
	 * multiple calls to this function across a single transaction.
	 */
	static struct {
		const void	*dout;
		void		*din;
		int		len;
	} xfer_arr[32];
	static int xfer_len;
	static int xfer_ttl;
	static u8 dummy;

	int i, btx, brx, ret = 0;
	void *p;
	struct m2s_spi_slave *s = to_m2s_spi(slv);
	volatile struct mss_pdma_chan *chan;

	/*
	 * If this is a first transfer in a transaction, reset
	 * the static variables to the "zero" state.
	 */
	if (fl & SPI_XFER_BEGIN) {
		xfer_len = 0;
		xfer_ttl = 0;
	}

	/*
	 * Save the parameters of this transfer and continue
	 * to calculate the size of the total transaction.
	 */
	xfer_arr[xfer_len].dout = dout;
	xfer_arr[xfer_len].din = din;
	xfer_arr[xfer_len].len = bl / 8;
	xfer_ttl += xfer_arr[xfer_len].len;

	/*
	 * If this is not the last transfer in a transaction,
	 * prepare for a next transfer and bail out.
	 */
	if (!(fl & SPI_XFER_END)) {
		xfer_len++;
		goto done;
	}

	/*
	 * Finally, this is the last transfer in a transaction.
	 * Ready to perform the actual transaction.
	 * Set for this slave: frame size, clock, slave select, mode
	 */
	if (spi_m2s_hw_bt_set(s, 8) ||
	    spi_m2s_hw_clk_set(s, s->hz) ||
	    spi_m2s_hw_cs_set(s, s->slave.cs) ||
	    spi_m2s_hw_mode_set(s, s->mode)) {
		ret = -1;
		goto done;
	}

	/*
	 * We can't provide persistent TxFIFO data flow even with PDMA, so to
	 * avoid resetting #SS - set up frame counter
	 */
	spi_m2s_hw_tfsz_set(s, xfer_ttl);

	/*
	 * We don't use double buffering scheme, because we should be able to
	 * change ADDR_INC value in each xfer_arr[i] transaction (to set it to
	 * zero in cases of dummy tx/rx (null xfer_arr[i].din/dout value).
	 * Note, bad address (null) can't be used as src/dst; in this case PDMA
	 * just terminate execution.
	 * Below we use different vars for indexing in A/B bufs of TX & RX DMAs
	 * (brx and btx), though actually these are always the same; so do such
	 * way just for more clearance
	 */
	for (i = 0; i <= xfer_len; i++) {
		/*
		 * Set-up RX
		 */
		chan = &MSS_PDMA->chan[s->drx];
		brx = !!(chan->status & PDMA_STATUS_BUF_SEL);
		chan->control |= brx ? PDMA_CONTROL_CLR_B : PDMA_CONTROL_CLR_A;
		if (xfer_arr[i].din) {
			p = xfer_arr[i].din;
			chan->control &= ~PDMA_CONTROL_DST_ADDR_INC_MSK;
			chan->control |= PDMA_CONTROL_DST_ADDR_INC_1;
		} else {
			p = &dummy;
			chan->control &= ~PDMA_CONTROL_DST_ADDR_INC_MSK;
			chan->control |= PDMA_CONTROL_DST_ADDR_INC_0;
		}
		chan->buf[brx].src = (u32)&MSS_SPI(s)->rx_data;
		chan->buf[brx].dst = (u32)p;

		/*
		 * Set-up TX
		 */
		chan = &MSS_PDMA->chan[s->dtx];
		btx = !!(chan->status & PDMA_STATUS_BUF_SEL);
		chan->control |= brx ? PDMA_CONTROL_CLR_B : PDMA_CONTROL_CLR_A;
		if (xfer_arr[i].dout) {
			p = (void *)xfer_arr[i].dout;
			chan->control &= ~PDMA_CONTROL_SRC_ADDR_INC_MSK;
			chan->control |= PDMA_CONTROL_SRC_ADDR_INC_1;
		} else {
			p = &dummy;
			chan->control &= ~PDMA_CONTROL_SRC_ADDR_INC_MSK;
			chan->control |= PDMA_CONTROL_SRC_ADDR_INC_0;
		}
		chan->buf[btx].src = (u32)p;
		chan->buf[btx].dst = (u32)&MSS_SPI(s)->tx_data;

		/*
		 * Start RX, and TX
		 */
		MSS_PDMA->chan[s->drx].buf[brx].cnt = xfer_arr[i].len;
		MSS_PDMA->chan[s->dtx].buf[btx].cnt = xfer_arr[i].len;

		/*
		 * Wait for transaction completes (basing on RX status)
		 */
		while (!(MSS_PDMA->chan[s->drx].status & (1 << brx)));
	}

#if defined(SPI_M2S_DEBUG)
	pdma_dump("xfer done", s->drx, s->dtx);
	for (i = 0; i <= xfer_len; i++) {
		d_printk(4, "%d:%x,%x\n", i,
			xfer_arr[i].dout ?
				((unsigned char *) xfer_arr[i].dout)[0] : -1,
			xfer_arr[i].din ?
				((unsigned char *) xfer_arr[i].din)[0] : -1);
	}
#endif
done:
	d_printk(3, "slv=%p,bl=%d,fl=0x%lx\n", slv, bl,fl);
	return ret;
}

#if defined(CONFIG_CMD_M2S_SPI_TEST)

void m2s_spi_test(unsigned int bus, unsigned char cmd)
{
	struct spi_slave *s;
	unsigned char rsp[32];
	int len;

	s = spi_setup_slave(bus, 0, 1000000, SPI_MODE_3);
	if (!s) {
		printf("%s: failed to setup slave\n", __func__);
		goto done_do_nothing;
	}
	if (spi_claim_bus(s)) {
		printf("%s: failed to claim bus\n", __func__);
		goto done_free_slave;
	}
	if (spi_xfer(s, 8, &cmd, NULL, SPI_XFER_BEGIN)) {
		printf("%s: failed to send command: 0%x\n",
			 __func__, cmd);
		goto done_free_slave;
	}
	len = 4;
	if (spi_xfer(s, 8 * len, NULL, rsp, SPI_XFER_END)) {
		printf("%s: failed to receive responce\n", __func__);
		goto done_free_slave;
	}

	spi_release_bus(s);
done_free_slave:
	spi_free_slave(s);
done_do_nothing:
	return;
}

#endif /* CONFIG_CMD_M2S_SPI_TEST */
