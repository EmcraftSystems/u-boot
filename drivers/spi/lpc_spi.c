#include <common.h>
#include <clock.h>
#include <malloc.h>
#include <spi.h>
#include <asm/arch/lpc18xx_scu.h>
#include <asm/arch/lpc18xx_gpio.h>

/* Control Register bits definition */
#define SPI_CR_BITENABLE	(1 << 2)
#define SPI_CR_CPHA		(1 << 3)
#define SPI_CR_CPOL		(1 << 4)
#define SPI_CR_MSTR		(1 << 5)
#define SPI_CR_LSBF		(1 << 6)
#define SPI_CR_SPIE		(1 << 7)
#define SPI_CR_BITS(x)		((x) << 8)

/* Status Register bits definition */
#define SPI_SR_ABRT		(1 << 3)
#define SPI_SR_MODF		(1 << 4)
#define SPI_SR_ROVR		(1 << 5)
#define SPI_SR_WCOL		(1 << 6)
#define SPI_SR_SPIF		(1 << 7)

/* SPI Clock Counter Register */
#define SPI_CCR_COUNTER(x)	((x) << 0)

/*
 * SPI registers
 */
struct lpc_spi {
	u32 cr;
	u32 sr;
	u32 dr;
	u32 ccr;
	u32 tcr;
	u32 tsr;
	u32 reserved;
	u32 interrupt;
};
#define LPC_SPI_BASE 0x40100000
static volatile struct lpc_spi *lpc_spi = (struct lpc_spi *)LPC_SPI_BASE;

/* SPI pins configuration */
static const struct lpc18xx_pin_config
lpc_spi_pins_config[] = CONFIG_LPC_SPI_PINS;

static const struct lpc18xx_iomux_dsc lpc_cs_gpio = CONFIG_LPC_CS_GPIO;

/*
 * Private data structure for an SPI slave
 */
struct lpc_spi_slave {
	struct spi_slave	slave;		/* Generic slave */
	u32			spccr;		/* SPI bus rate */
	u32			mode;		/* SPI bus mode */
};

/*
 * Handler to get access to the driver specific slave data structure
 * @param c		generic slave
 * @returns		driver specific slave
 */
static inline struct lpc_spi_slave *to_lpc_spi(struct spi_slave *slave)
{
	return container_of(slave, struct lpc_spi_slave, slave);
}

/*
 * Initialization of the entire driver
 */
void spi_init()
{
	/* Configure PINs */
	lpc18xx_pin_config_table(lpc_spi_pins_config,
				 ARRAY_SIZE(lpc_spi_pins_config));

	/* Configure Chip Select GPIO */
	lpc_gpio_dir(lpc_cs_gpio, 1);
	lpc_gpio_set(lpc_cs_gpio);

	/* The SPI operates in Master mode. */
	lpc_spi->cr = SPI_CR_MSTR;
}

/*
 * Prepare to use an SPI slave
 * @param b		SPI controller
 * @param cs		slave Chip Select
 * @param hz		max freq this slave can run at
 * @param m		slave access mode
 * @returns		driver specific slave
 */
struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
				  unsigned int hz, unsigned int mode)
{
	struct lpc_spi_slave *s;
	struct spi_slave *slave = NULL;
	unsigned int spccr;

	if (bus != 0 || cs != 0) {
		goto done;
	}

	spccr = (clock_get(CLOCK_SPI) / hz) & 0xfe;
	if (spccr < 8) {
		goto done;
	}

	s = malloc(sizeof(struct lpc_spi_slave));
	if (!s) {
		goto done;
	}

	s->spccr = spccr;
	s->mode = mode;

	slave = &s->slave;

	slave->bus = bus;
	slave->cs = cs;

 done:
	return slave;
}

/*
 * Done with an an SPI slave
 * @param slv		SPI slave
 */
void spi_free_slave(struct spi_slave *slv)
{
	struct lpc_spi_slave *s = to_lpc_spi(slv);
	free(s);
}

/*
 * Set up the SPI controller
 * @param slv		SPI slave
 * @returns		0->success; !0->failure
 */
int spi_claim_bus(struct spi_slave *slv)
{
	struct lpc_spi_slave *s = to_lpc_spi(slv);

	/* set speed */
	lpc_spi->ccr = s->spccr;

	/* set mode */
	if (s->mode & SPI_CPHA) {
		lpc_spi->cr |= SPI_CR_CPHA;
	} else {
		lpc_spi->cr &= ~SPI_CR_CPHA;
	}
	if (s->mode & SPI_CPOL) {
		lpc_spi->cr |= SPI_CR_CPOL;
	} else {
		lpc_spi->cr &= ~SPI_CR_CPOL;
	}

	return 0;
}

/*
 * Shut down the SPI controller
 * @param slv		SPI slave
 */
void spi_release_bus(struct spi_slave *slv)
{

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
	u8 dummy = 0xff;
	int i;
	u32 sr;
	int ret = 0;
	const u8	*_dout = dout;
	u8		*_din = din;
	int len = bl >> 3;

	if (fl & SPI_XFER_BEGIN) {
		/* Enable chip select */
		lpc_gpio_clear(lpc_cs_gpio);
	}

	for (i = 0; i < len; i++) {
		if (dout == NULL) {
			lpc_spi->dr = dummy;
		} else {
			lpc_spi->dr = _dout[i];
		}

		do {
			sr = lpc_spi->sr;
		}
		while ((sr & SPI_SR_SPIF) == 0);
		sr = lpc_spi->sr;
		if (din == NULL) {
			dummy = lpc_spi->dr;
		} else {
			_din[i] = lpc_spi->dr;
		}
		sr = lpc_spi->tsr;
	}

	if (fl & SPI_XFER_END) {
		/* Disable chip select */
		lpc_gpio_set(lpc_cs_gpio);
	}

	return ret;
}
