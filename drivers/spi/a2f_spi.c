/*
 * Copyright (C) 2012 Vladimir Khusainov, Emcraft Systems, vlad@emcraft.com
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
 * Debug output control. While debugging, have SPI_A2F_DEBUG defined.
 * In deployment, make sure that SPI_A2F_DEBUG is undefined
 * to avoid performance and size overhead of debug messages.
 */
#if defined(CONFIG_SYS_A2F_SPI_DEBUG)
#define SPI_A2F_DEBUG
#endif

#if defined(SPI_A2F_DEBUG)

/*
 * Driver verbosity level: 0->silent; >0->verbose (1 to 4, growing verbosity)
 */
static int spi_a2f_debug = 4;

/*
 * Service to print debug messages
 */
#define d_printk(level, fmt, args...)					\
	if (spi_a2f_debug >= level) printf("%s: " fmt, __func__, ## args)
#else

#define d_printk(level, fmt, args...)

#endif /* defined(SPI_A2F_DEBUG) */

/*
 * Private data structure for an SPI slave
 */
struct a2f_spi_slave {
	struct spi_slave		slave;		/* Generic slave */
	void *	 			regs;		/* Registers base */
	unsigned int			hz;		/* SPI bus rate */
	unsigned int			mode;		/* SPI bus mode */
};

/*
 * Handler to get access to the driver specific slave data structure
 * @param c		generic slave
 * @returns		driver specific slave 
 */
static inline struct a2f_spi_slave *to_a2f_spi(struct spi_slave *slave)
{
	return container_of(slave, struct a2f_spi_slave, slave);
}

/*
 * Generic I/O access services
 */
#define writel(v, a)	(* ((volatile unsigned int *) (a)) = (v))
#define readl(a)	(* ((volatile unsigned int *) (a)))

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
 * Access handle for the SPI control registers
 */
#define MSS_SPI_REGS(regs)		((volatile struct mss_spi *)(regs))
#define MSS_SPI(s)			(MSS_SPI_REGS(s->regs))

/*
 * Base address of the MSS-based SPI controllers
 */
#define MSS_SPI0_REGS			0x40001000
#define MSS_SPI1_REGS			0x40011000

/*
 * Base address of the FPGA-based SPI controller
 */
#define FPGA_SPI_REGS			0x31000000

/*
 * Some bits in various CSRs 
 */
#define SPI0_RST_CLR			(1<<9)
#define SPI1_RST_CLR			(1<<10)
#define SPI_CONTROL_ENABLE		(1<<0)
#define SPI_CONTROL_MASTER		(1<<1)
#define SPI_CONTROL_PROTO_MSK		(3<<2)
#define SPI_CONTROL_PROTO_MOTO		(0<<2)
#define SPI_CONTROL_CNT_MSK		(0xffff<<8)
#define SPI_CONTROL_CNT_SHF		(8)
#define SPI_CONTROL_SPO			(1<<24)
#define SPI_CONTROL_SPH			(1<<25)
#define SPI_CONTROL_SPS			(1<<26)
#define SPI_CONTROL_BIGFIFO		(1<<29)
#define SPI_CONTROL_RESET		(1<<31)
#define SPI_STATUS_RXFIFOOVR		(1<<2)
#define SPI_STATUS_RXFIFOEMP		(1<<6)
#define SPI_STATUS_TXFIFOFUL		(1<<8)
#define SPI_INTCLR_RXFIFOOVR		(1<<2)

/*
 * Set chip select
 * @param s		slave
 * @param cs		chip select: [0..7]->slave, otherwise->deselect all
 * @returns		0->good,!=0->bad
 */
static inline int spi_a2f_hw_cs_set(struct a2f_spi_slave *s, int cs)
{
	unsigned int v = 0<=cs && cs<=7 ? 1<<cs : 0;
	int ret = 0;

	writel(v, &MSS_SPI(s)->slave_select);

	d_printk(3, "bus=%d,cs=%d,slave_select=0x%x,ret=%d\n", 
		 s->slave.bus, cs, readl(&MSS_SPI(s)->slave_select), ret);
	return ret;
}

/*
 * Set controller clock rate
 * @param s		slave
 * @param spd		clock rate in Hz
 * @returns		0->good,!=0->bad
 */
static inline int spi_a2f_hw_clk_set(
	struct a2f_spi_slave *s, unsigned int spd)
{
	int i;
	unsigned int h;
	int ret = 0;

	/*
 	 * Calculate the clock rate that works for this slave
 	 */
	h = s->slave.bus == 2 ? 40000000 :
		clock_get(s->slave.bus == 0 ? CLOCK_PCLK0 : CLOCK_PCLK1);
	for (i = 1; i <= 8; i ++) {
		if (h / (1 << i) <= spd) break;
	}

	/*
 	 * Can't provide a rate that is slow enough for the slave
 	 */
	if (i == 9) {
		ret = -1;
		goto Done;
	}

	/*
 	 * Set the clock rate
 	 */
	writel(i - 1, &MSS_SPI(s)->clk_gen);

Done:
	d_printk(3, "bus=%d,cnt_hz=%d,slv_hz=%d,rsl_hz=%d,clk_gen=%d,ret=%d\n",
		s->slave.bus, h, spd, h / (1 << i),
		readl(&MSS_SPI(s)->clk_gen), ret);
	return ret;
}

/*
 * Set frame size (making an assumption that the supplied size is
 * supported by this controller)
 * @param s		slave
 * @param bt		frame size
 * @returns		0->good,!=0->bad
 */
static inline int spi_a2f_hw_bt_set(struct a2f_spi_slave *s, int bt)
{
	int ret = 0;

	/*
 	 * Disable the SPI contoller. Writes to data frame size have
 	 * no effect when the controller is enabled.
 	 */
	writel(~SPI_CONTROL_ENABLE & readl(&MSS_SPI(s)->control),
		&MSS_SPI(s)->control);

	/*
 	 * Set the new data frame size.
 	 */
	writel(bt, &MSS_SPI(s)->txrxdf_size);

	/*
 	 * Re-enable the SPI contoller 
 	 */
	writel(SPI_CONTROL_ENABLE | readl(&MSS_SPI(s)->control),
		&MSS_SPI(s)->control);

	d_printk(3, "bus=%d,bt=%d,txrxdf_size=%d,ret=%d\n",
		 s->slave.bus, bt, readl(&MSS_SPI(s)->txrxdf_size), ret);
	return ret;
}

/*
 * Set transfer length
 * @param s		slave
 * @param len		transfer size
 */
static inline void spi_a2f_hw_tfsz_set(struct a2f_spi_slave *s, int len)
{
	/*
 	 * Disable the SPI contoller. Writes to transfer length have
 	 * no effect when the controller is enabled.
 	 */
	writel(~SPI_CONTROL_ENABLE & readl(&MSS_SPI(s)->control),
		&MSS_SPI(s)->control);

	/*
 	 * Set the new data frame size.
 	 */
	writel(~SPI_CONTROL_CNT_MSK & readl(&MSS_SPI(s)->control),
		&MSS_SPI(s)->control);
	writel((len << SPI_CONTROL_CNT_SHF) | readl(&MSS_SPI(s)->control),
		&MSS_SPI(s)->control);

	/*
 	 * Re-enable the SPI contoller 
 	 */
	writel(SPI_CONTROL_ENABLE | readl(&MSS_SPI(s)->control),
		&MSS_SPI(s)->control);

	d_printk(3, "bus=%d,len=%d\n", s->slave.bus, len);
}

/*
 * Set SPI mode
 * @param s		slave
 * @param mode		mode
 * @returns		0->good;!=0->bad
 */
static inline int spi_a2f_hw_mode_set(
	struct a2f_spi_slave *s, unsigned int mode)
{
	int ret = 0;

	/*
 	 * Set the mode
 	 */
	if (mode & SPI_CPHA) {
		writel(SPI_CONTROL_SPH | readl(&MSS_SPI(s)->control),
			&MSS_SPI(s)->control);
	}
	else {
		writel(~SPI_CONTROL_SPH & readl(&MSS_SPI(s)->control),
			&MSS_SPI(s)->control);
	}
	if (mode & SPI_CPOL) {
		writel(SPI_CONTROL_SPO | readl(&MSS_SPI(s)->control),
			&MSS_SPI(s)->control);
	}
	else {
		writel(~SPI_CONTROL_SPO & readl(&MSS_SPI(s)->control),
			&MSS_SPI(s)->control);
	}

	d_printk(2, "bus=%d,mode=%x,control=%x,ret=%d\n",
		 s->slave.bus, mode, readl(&MSS_SPI(s)->control), ret);
	return ret;
}

/*
 * Is transmit FIFO full?
 * @param s		slave
 * @returns		!0->full;0->not full
 */
static inline int spi_a2f_hw_txfifo_full(struct a2f_spi_slave *s)
{
	return readl(&MSS_SPI(s)->status) & SPI_STATUS_TXFIFOFUL;
}

/*
 * Put a frame into the transmit FIFO
 * @param s		slave
 * @param wb		frame size in full bytes
 * @param tx		transmit buf (can be NULL)
 * @param i		index of frame in buf
 */
static inline void spi_a2f_hw_txfifo_put(
	struct a2f_spi_slave *s, int wb, const void *tx, int i)
{
	int j;
	unsigned int d = 0;
	unsigned char *p = (unsigned char *)tx;

	if (p) {
		for (j = 0; j < wb; j++) {
			d <<= 8;
			d |= p[i*wb + j];
		}
	}
	writel(d, &MSS_SPI(s)->tx_data);
}

/*
 * Is receive FIFO empty?
 * @param s		slave
 * @returns		!0->empty,0->not empty
 */
static inline int spi_a2f_hw_rxfifo_empty(struct a2f_spi_slave *s)
{
	return readl(&MSS_SPI(s)->status) & SPI_STATUS_RXFIFOEMP;
}

/*
 * Is receive FIFO overflown?
 * @param s		slave
 * @returns		!0->error,0->no error
 */
static inline int spi_a2f_hw_rxfifo_error(struct a2f_spi_slave *s)
{
	return readl(&MSS_SPI(s)->status) & SPI_STATUS_RXFIFOOVR;
}

/*
 * Retrieve a frame from the receive FIFO
 * @param s		slave
 * @param wb		frame size in full bytes
 * @param rx		receive buf (can be NULL)
 * @param i		index of frame in buf
 */
static inline void spi_a2f_hw_rxfifo_get(
	struct a2f_spi_slave *s, unsigned int wb, void *rx, int i)
{
	int j;
	unsigned int d = readl(&MSS_SPI(s)->rx_data);
	unsigned char *p = (unsigned char *)rx;

	if (p) {
		for (j = wb-1; j >= 0; j--) {
			p[i*wb + j] = d & 0xFF;
			d >>= 8;
		}
	}
}

/*
 * Receive FIFO overflown; clean-up
 * @param s		slave
 * @param rx		receive buf (can be NULL)
 * @param i		index of frame in buf
 */
static inline void spi_a2f_hw_rxfifo_purge(struct a2f_spi_slave *s) 
{
	while (!spi_a2f_hw_rxfifo_empty(s)) {
		spi_a2f_hw_rxfifo_get(s, 1, NULL, 0);
	}
	writel(SPI_INTCLR_RXFIFOOVR | readl(&MSS_SPI(s)->int_clear),
		&MSS_SPI(s)->int_clear);
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
struct spi_slave *spi_setup_slave(
	unsigned int b, unsigned int cs, unsigned int hz, unsigned int m)
{
	void			*r;
	struct a2f_spi_slave	*s;
	struct spi_slave	*slv = NULL;

	/*
 	 * Validate input parameters. Can be anything since this is
 	 * part of user build-time configuration.
 	 */
	if (! (0 <= b && b <= 2)) {
		goto Done;
	}
	if (! (0 <= cs && cs <= 7)) {
		goto Done;
	}

	/*
 	 * Calculate where the SPI controller registers live at
 	 */
	r = (void *)
		(b==0 ? MSS_SPI0_REGS :
		(b==1 ? MSS_SPI1_REGS : FPGA_SPI_REGS));

	/*
 	 * Allocate the driver-specific slave data structure
 	 */
	s = malloc(sizeof(struct a2f_spi_slave));
	if (!s) {
		goto Done;
	}

	/*
 	 * Fill in the driver-specific slave data structure
 	 */
	s->slave.bus = b;
	s->slave.cs = cs;
	s->mode = m;
	s->hz = hz;
	s->regs = r;
	slv = &s->slave;

	d_printk(2, "bus=%d,regs=%p,cs=%d,hz=%d,mode=0x%x\n", 
		b, r, cs, hz, m);

Done :
	d_printk(2, "slv=%p\n", slv);
	return slv;
}

/*
 * Done with an an SPI slave
 * @param slv		SPI slave
 */
void spi_free_slave(struct spi_slave *slv)
{
	struct a2f_spi_slave *s = to_a2f_spi(slv);

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
	unsigned int v;
	unsigned int ret = 0;
	struct a2f_spi_slave *s = to_a2f_spi(slv);

	/*
	 * Reset the MSS SPI controller and then bring it out of reset
 	 */
	if (slv->bus < 2) {
		v = slv->bus==0 ? SPI0_RST_CLR : SPI1_RST_CLR;
		writel(v | readl(&M2S_SYSREG->soft_reset_cr),
			&M2S_SYSREG->soft_reset_cr);
		writel(~v & readl(&M2S_SYSREG->soft_reset_cr),
			&M2S_SYSREG->soft_reset_cr);
	}

	/*
 	 * Set the master mode
 	 */
	writel(SPI_CONTROL_MASTER | readl(&MSS_SPI(s)->control),
		&MSS_SPI(s)->control);

	/*
 	 * Set the transfer protocol. We are using the Motorola
 	 * SPI mode, with no user interface to configure it to 
 	 * some other mode.
 	 */
	writel(~SPI_CONTROL_PROTO_MSK & readl(&MSS_SPI(s)->control),
		&MSS_SPI(s)->control);
	writel(SPI_CONTROL_PROTO_MOTO | readl(&MSS_SPI(s)->control),
		&MSS_SPI(s)->control);

	/*
	 * We are running on the A2F500 device, we have an option
	 * to set-up the controller in such a way that it doesn't remove
	 * Chip Select until the entire message has been transferred,
	 * even if at some points TX FIFO becomes empty.
	 * ...
	 * Similarly on A2F500, we have an option to extend FIFO to
	 * 32 8-bit FIFO frames.
 	 */
	writel(SPI_CONTROL_SPS | SPI_CONTROL_BIGFIFO |
		readl(&MSS_SPI(s)->control),
		&MSS_SPI(s)->control);

	/*
 	 * Enable the SPI contoller
	 * On the A2F500 it is critical to clear RESET in
	 * the control bit. This bit is not defined for A2F200.
 	 */
	writel(~SPI_CONTROL_RESET & readl(&MSS_SPI(s)->control),
		&MSS_SPI(s)->control);
	writel(SPI_CONTROL_ENABLE | readl(&MSS_SPI(s)->control),
		&MSS_SPI(s)->control);

	d_printk(2, "bus=%d,soft_reset_cr=0x%x,control=0x%x\n",
		slv->bus, readl(&M2S_SYSREG->soft_reset_cr),
		readl(&MSS_SPI(s)->control));

	d_printk(2, "slv=%p\n", slv);
	return ret;
}

/*
 * Shut down the SPI controller 
 * @param slv		SPI slave
 */
void spi_release_bus(struct spi_slave *slv)
{
	unsigned int v;
	struct a2f_spi_slave *s = to_a2f_spi(slv);

	/*
 	 * Disable the SPI contoller
 	 */
	writel(~SPI_CONTROL_ENABLE & readl(&MSS_SPI(s)->control),
		&MSS_SPI(s)->control);

	/*
 	 * Put the SPI controller into reset
 	 */
	if (slv->bus < 2) {
		v = slv->bus==0 ? SPI0_RST_CLR : SPI1_RST_CLR;
		writel(v | readl(&M2S_SYSREG->soft_reset_cr),
			&M2S_SYSREG->soft_reset_cr);
	}

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
int spi_xfer(
	struct spi_slave *slv, unsigned int bl,
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
 	 * a certain CSR (this is the model that works for the A2F500; it is
 	 * even more restricive for the A2F200).
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

	int i, ri, ti, rx_i, tx_i, rx_l, tx_l, rx_t, tx_t;
	struct a2f_spi_slave *s = to_a2f_spi(slv);
	int ret = 0;

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
		goto Done;
	}

	/*
 	 * Finally, this is the last transfer in a transaction.
 	 * Ready to perform the actual transaction.
 	 * Set for this slave: frame size, clock, slave select, mode
 	 */
	if (spi_a2f_hw_bt_set(s, 8) ||
	    spi_a2f_hw_clk_set(s, s->hz) ||
	    spi_a2f_hw_cs_set(s, s->slave.cs) ||
	    spi_a2f_hw_mode_set(s, s->mode)) {
		ret = -1;
		goto Done;
	}

	/*
 	 * Set the size of the transfer in the SPI controller
 	 */
	spi_a2f_hw_tfsz_set(s, xfer_ttl);

	/*
 	 * Prepare to traverse the array of transfers.
 	 * We will need to advance separately over 
 	 * transmit and receive data
 	 */
	tx_t = 0;
	tx_l = xfer_arr[tx_t].len;
	tx_i = 0;
	rx_t = 0;
	rx_l = xfer_arr[rx_t].len;
	rx_i = 0;

	/*
 	 * Perform the transfer. Transfer is done when all frames
 	 * have been received (i.e. ri == len). Each time we
 	 * iterate in this loop, we have received a next frame.
 	 */
	for (ti = 0, ri = 0; ri < xfer_ttl; ri++) {

		/* It is important to keep transmit fifo not empty,
		 * while there are frames to be transmitted. If this
		 * is not done, the SPI controller asserts slave
		 * select as soon as transmit fifo has been emptied
		 * regardless of the value in transfer count (which
		 * cancels a transaction at the slave). On the other
		 * hand, it is important to let the code retrieving
		 * incoming data (below) run every so frequenly or
		 * otherwise an RX overflow will happen.
		 */
	        for (i = 0; 
		     i < 2 && ti < xfer_ttl && !spi_a2f_hw_txfifo_full(s);
		     i++) {

			/*
 			 * If the trasmit in the current transfer
 			 * has been finished, go to the next one.
 			 */
			while (tx_i == tx_l) {
				tx_t++;
				tx_l = xfer_arr[tx_t].len;
				tx_i = 0;
			}

			/*
 			 * Put a frame (or a dummy value) to the transmit fifo
 			 */
			spi_a2f_hw_txfifo_put(s, 1, xfer_arr[tx_t].dout, tx_i);
			tx_i++;
			ti++;
		}

		/*
 		 * Wait for a frame to come in (but not indefinitely)
 		 * but check for error conditions first
 		 */
		if (spi_a2f_hw_rxfifo_error(s)) {
			/*
			 * If the receive fifo overflown, this transfer
			 * needs to be finished with an error.
			 */
			spi_a2f_hw_rxfifo_purge(s);
			ret = -1;
			goto Done;
		}
		for (i = 0; i < 100 && spi_a2f_hw_rxfifo_empty(s); i++);

		/*
		 * Process as many incoming frames as there is in the fifo
		 */
		while (!spi_a2f_hw_rxfifo_empty(s)) {

			/*
 			 * If the receive in the current transfer
 	 	 	 * has been finished, go to the next one.
 		 	 */
			while (rx_i == rx_l) {

				/*
 			 	 * Advance to the next transfer
 			 	 */
				rx_t++;
				rx_l = xfer_arr[rx_t].len;
				rx_i = 0;
			}

			/* 
 		 	 * Read in the frame (or a dummy frame).
 		 	 */
			spi_a2f_hw_rxfifo_get(s, 1, xfer_arr[rx_t].din, rx_i);
			rx_i++;
		}
	}

#if defined(SPI_A2F_DEBUG)
	for (i = 0; i <= xfer_len; i++) {
		d_printk(4, "%d:%x,%x\n", i,
			xfer_arr[i].dout ? 
				((unsigned char *) xfer_arr[i].dout)[0] : -1, 
			xfer_arr[i].din ? 
				((unsigned char *) xfer_arr[i].din)[0] : -1);
	}
#endif

Done:
	d_printk(3, "slv=%p,bl=%d,fl=0x%lx\n", slv, bl,fl);
	return ret;
}

#if defined(CONFIG_CMD_A2F_SPI_TEST)

void a2f_spi_test(unsigned int bus, unsigned char cmd)
{
	struct spi_slave *s;
	unsigned char rsp[32];
	int len;

	s = spi_setup_slave(bus, 0, 1000000, SPI_MODE_3);
	if (!s) {
		printf("%s: failed to setup slave\n", __func__);
		goto Done_do_nothing;
	}
	if (spi_claim_bus(s)) {
		printf("%s: failed to claim bus\n", __func__);
		goto Done_free_slave;
	}
	if (spi_xfer(s, 8, &cmd, NULL, SPI_XFER_BEGIN)) {
		printf("%s: failed to send command: 0%x\n",
			 __func__, cmd);
		goto Done_free_slave;
	}
	len = 4;
	if (spi_xfer(s, 8 * len, NULL, rsp, SPI_XFER_END)) {
		printf("%s: failed to receive responce\n", __func__);
		goto Done_free_slave;
	}

	spi_release_bus(s);
Done_free_slave:
	spi_free_slave(s);
Done_do_nothing:
	;
}

#endif /* CONFIG_CMD_A2F_SPI_TEST */
