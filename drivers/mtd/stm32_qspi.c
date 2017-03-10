/*
 * (C) Copyright 2016
 * Sergei Miroshnichenko, Emcraft Systems, sergeimir@emcraft.com
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <malloc.h>
#include <linux/mtd/compat.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/spi-nor.h>
#include <linux/mtd/stm32_qspi.h>
#include <asm/io.h>
#include <errno.h>
#include <asm/arch/stm32.h>

DECLARE_GLOBAL_DATA_PTR;

struct stm32_qspi_regs {
	u32	cr;
	u32	dcr;
	u32	sr;
	u32	fcr;
	u32	dlr;
	u32	ccr;
	u32	ar;
	u32	abr;
	u32	dr;
	u32	psmkr;
	u32	psmar;
	u32	pir;
	u32	lptr;
};

#define QSPI_CR_EN			(1 << 0)
#define QSPI_CR_ABORT			(1 << 1)
#define QSPI_CR_DMAEN			(1 << 2)
#define QSPI_CR_TCEN			(1 << 3)
#define QSPI_CR_SSHIFT			(1 << 4)
#define QSPI_CR_DFM			(1 << 6)
#define QSPI_CR_FSEL			(1 << 7)
#define QSPI_CR_TEIE			(1 << 16)
#define QSPI_CR_TCIE			(1 << 17)
#define QSPI_CR_FTIE			(1 << 18)
#define QSPI_CR_SMIE			(1 << 19)
#define QSPI_CR_TOIE			(1 << 20)
#define QSPI_CR_APMS			(1 << 22)
#define QSPI_CR_PMM			(1 << 23)
#define QSPI_CR_FTHRES(x)		(((x) & 0x1F) << 8)
#define QSPI_CR_PRESCALER(x)		(((x) & 0xFF) << 24)

#define QSPI_DCR_CK_MODE		(1 << 0)
#define QSPI_DCR_CSHT(x)		(((x) & 0x7) << 8)
#define QSPI_DCR_FSIZE(x)		(((x) & 0x1F) << 16)

#define QSPI_SR_TEF			(1 << 0)
#define QSPI_SR_TCF			(1 << 1)
#define QSPI_SR_FTF			(1 << 2)
#define QSPI_SR_SMF			(1 << 3)
#define QSPI_SR_TOF			(1 << 4)
#define QSPI_SR_BUSY			(1 << 5)
#define QSPI_SR_FLEVEL(x)		(((x) >> 8) & 0x3F)

#define QSPI_FCR_CTEF			(1 << 0)
#define QSPI_FCR_CTCF			(1 << 1)
#define QSPI_FCR_CSMF			(1 << 3)
#define QSPI_FCR_CTOF			(1 << 4)

#define QSPI_CCR_INSTRUCTION(x)		(((x) & 0xFF) << 0)
#define QSPI_CCR_IMODE(x)		(((x) & 0x3) << 8)
#define QSPI_CCR_IMODE_NONE		(QSPI_CCR_IMODE(0))
#define QSPI_CCR_IMODE_SINGLE_LINE	(QSPI_CCR_IMODE(1))
#define QSPI_CCR_IMODE_TWO_LINES	(QSPI_CCR_IMODE(2))
#define QSPI_CCR_IMODE_FOUR_LINES	(QSPI_CCR_IMODE(3))
#define QSPI_CCR_ADMODE(x)		(((x) & 0x3) << 10)
#define QSPI_CCR_ADMODE_NONE		(QSPI_CCR_ADMODE(0))
#define QSPI_CCR_ADMODE_SINGLE_LINE	(QSPI_CCR_ADMODE(1))
#define QSPI_CCR_ADMODE_TWO_LINES	(QSPI_CCR_ADMODE(2))
#define QSPI_CCR_ADMODE_FOUR_LINES	(QSPI_CCR_ADMODE(3))
#define QSPI_CCR_ADSIZE(x)		(((x) & 0x3) << 12)
#define QSPI_CCR_ADSIZE_ONE_BYTE	(QSPI_CCR_ADSIZE(0))
#define QSPI_CCR_ADSIZE_TWO_BYTES	(QSPI_CCR_ADSIZE(1))
#define QSPI_CCR_ADSIZE_THREE_BYTES	(QSPI_CCR_ADSIZE(2))
#define QSPI_CCR_ADSIZE_FOUR_BYTES	(QSPI_CCR_ADSIZE(3))
#define QSPI_CCR_ABMODE(x)		(((x) & 0x3) << 14)
#define QSPI_CCR_ABSIZE(x)		(((x) & 0x3) << 16)
#define QSPI_CCR_DCYC(x)		(((x) & 0x1F) << 18)
#define QSPI_CCR_DMODE(x)		(((x) & 0x3) << 24)
#define QSPI_CCR_DMODE_NONE		(QSPI_CCR_DMODE(0))
#define QSPI_CCR_DMODE_SINGLE_LINE	(QSPI_CCR_DMODE(1))
#define QSPI_CCR_DMODE_TWO_LINES	(QSPI_CCR_DMODE(2))
#define QSPI_CCR_DMODE_FOUR_LINES	(QSPI_CCR_DMODE(3))
#define QSPI_CCR_FMODE(x)		(((x) & 0x3) << 26)
#define QSPI_CCR_FMODE_INDIRECT_WRITE	(QSPI_CCR_FMODE(0))
#define QSPI_CCR_FMODE_INDIRECT_READ	(QSPI_CCR_FMODE(1))
#define QSPI_CCR_FMODE_AUTO_POLL	(QSPI_CCR_FMODE(2))
#define QSPI_CCR_FMODE_MEMORY_MAP	(QSPI_CCR_FMODE(3))
#define QSPI_CCR_SIOO			(1 << 28)
#define QSPI_CCR_DHHC			(1 << 30)
#define QSPI_CCR_DDRM			(1 << 31)

#define STM32_QSPI_BASE			(STM32_AHB3PERIPH_BASE + 0x1000)

#define QSPI_TIMEOUT_MS			1000
#define QSPI_POLLING_INTERVAL		16

struct stm32_qspi_priv {
	struct stm32_qspi_regs *regs;
	size_t size;
	size_t erase_size;
	size_t write_size;
	int fast_read_dummy;
	int support_4bytes;
};

static struct stm32_qspi_priv *stm32_qspi = NULL;

static int stm32_qspi_set_speed(struct stm32_qspi_priv *priv, uint speed)
{
	int div = clock_get(CLOCK_HCLK) / speed;

	if ((div > 256) || (div < 1)) {
		error("%s: unable to provide a rate %d based of AHB freq %lu\n",
		       __func__, speed, clock_get(CLOCK_HCLK));
	}

	clrsetbits_le32(&priv->regs->cr,
			QSPI_CR_PRESCALER(0xFF),
			QSPI_CR_PRESCALER(div - 1));

	return 0;
}

static int wait_for_status(struct stm32_qspi_priv *priv, u32 mask, int active)
{
	int i, timeout = QSPI_TIMEOUT_MS * 1000;
	u32 reg;

	for (i = 0; i < timeout; ++i) {
		u32 status;
		reg = readl(&priv->regs->sr);
		status = reg & mask;

		if ((active && status) || (!active && !status))
			break;

		udelay(1);
	}

	if (i == timeout)
		error("%s: TIMEOUT: mask 0x%x, active %d, status 0x%x\n",
		      __func__, mask, active, reg);

	return (i == timeout) ? -1 : 0;
}

static int wait_while_busy(struct stm32_qspi_priv *priv)
{
	int err = wait_for_status(priv, QSPI_SR_BUSY, 0);
	if (err)
		error("%s: failed: %d\n", __func__, err);
	return err;
}

static int wait_until_complete(struct stm32_qspi_priv *priv)
{
	int err = wait_for_status(priv, QSPI_SR_TCF, 1);
	if (!err)
		setbits_le32(&priv->regs->fcr, QSPI_FCR_CTCF);
	else
		error("%s: failed: %d\n", __func__, err);

	return err;
}

static int wait_until_match(struct stm32_qspi_priv *priv)
{
	int err = wait_for_status(priv, QSPI_SR_SMF, 1);
	if (!err)
		setbits_le32(&priv->regs->fcr, QSPI_FCR_CSMF);
	else
		error("%s: failed: %d\n", __func__, err);
	return err;
}

static int wait_for_fifo(struct stm32_qspi_priv *priv)
{
	int err = wait_for_status(priv, QSPI_SR_FTF, 1);
	if (err)
		error("%s: failed: %d\n", __func__, err);
	return err;
}

static int autopoll(struct stm32_qspi_priv *priv, u32 mask, u32 match)
{
	int err;

	writel(match, &priv->regs->psmar);
	writel(mask, &priv->regs->psmkr);
	writel(0x10, &priv->regs->pir);
	writel(1 - 1, &priv->regs->dlr);

	clrsetbits_le32(&priv->regs->cr,
			QSPI_CR_PMM,
			QSPI_CR_APMS);

	err = wait_while_busy(priv);
	if (err)
		goto fail;

	writel(QSPI_CCR_FMODE_AUTO_POLL
	       | SPINOR_OP_RDSR
	       | QSPI_CCR_IMODE_SINGLE_LINE
	       | QSPI_CCR_ADMODE_NONE
	       | QSPI_CCR_DMODE_SINGLE_LINE
	       | QSPI_CCR_DCYC(0),
	       &priv->regs->ccr);

	err = wait_until_match(priv);
	if (err)
		goto fail;

	clrbits_le32(&priv->regs->cr,
		     QSPI_CR_PMM | QSPI_CR_APMS);

	debug("%s: status 0x%x\n", __func__, readl(&priv->regs->dr));
	return 0;
fail:
	error("%s: failed: %d\n", __func__, err);
	return err;
}

static int wait_while_writing(struct stm32_qspi_priv *priv)
{
	int err;

	err = wait_while_busy(priv);
	if (err)
		goto fail;

	err = autopoll(priv, SR_WIP, 0);
	if (err)
		goto fail;

	return 0;
fail:
	error("%s: failed: %d\n", __func__, err);
	return err;
}

static int wait_until_write_enable(struct stm32_qspi_priv *priv)
{
	int err;

	err = wait_while_busy(priv);
	if (err)
		goto fail;

	err = autopoll(priv, SR_WEL, SR_WEL);
	if (err)
		goto fail;

	return 0;
fail:
	error("%s: failed: %d\n", __func__, err);
	return err;
}

static int write_enable(struct stm32_qspi_priv *priv)
{
	int err;

	err = wait_while_busy(priv);
	if (err)
		goto fail;

	writel(QSPI_CCR_FMODE_INDIRECT_WRITE
	       | SPINOR_OP_WREN
	       | QSPI_CCR_IMODE_SINGLE_LINE
	       | QSPI_CCR_ADMODE_NONE
	       | QSPI_CCR_DMODE_NONE
	       | QSPI_CCR_DCYC(0),
	       &priv->regs->ccr);

	err = wait_until_complete(priv);
	if (err)
		goto fail;

	err = wait_until_write_enable(priv);
	if (err)
		goto fail;

	return 0;
fail:
	error("%s: failed: %d\n", __func__, err);
	return err;
}

static int stm32_qspi_abort(struct stm32_qspi_priv *priv)
{
	int err;

	priv->regs->cr &= ~QSPI_CR_DMAEN;
	priv->regs->cr |= QSPI_CR_ABORT;

	err = wait_until_complete(priv);
	priv->regs->cr |= QSPI_CR_DMAEN;
	if (err)
		goto fail;

	err = wait_while_busy(priv);
	if (err)
		goto fail;

	return 0;
fail:
	error("%s: failed: %d\n", __func__, err);
	return err;
}

static int switch_to_memory_mapped(struct stm32_qspi_priv *priv)
{
	int err;

	setbits_le32(&priv->regs->cr, QSPI_CR_DMAEN);

	err = wait_while_busy(priv);
	if (err)
		goto fail;

	writel(QSPI_CCR_FMODE_MEMORY_MAP
	       | (stm32_qspi->support_4bytes
		  ? SPINOR_OP_FAST_READ_4B
		  : SPINOR_OP_FAST_READ)
	       | QSPI_CCR_IMODE_SINGLE_LINE
	       | QSPI_CCR_ADMODE_FOUR_LINES
	       | (stm32_qspi->support_4bytes
		  ? QSPI_CCR_ADSIZE_FOUR_BYTES
		  : QSPI_CCR_ADSIZE_THREE_BYTES)
	       | QSPI_CCR_DMODE_FOUR_LINES
	       | QSPI_CCR_DCYC(priv->fast_read_dummy),
	       &priv->regs->ccr);

	return 0;
fail:
	error("%s: failed: %d\n", __func__, err);
	return err;
}

static int erase_block(struct stm32_qspi_priv *priv, u32 address)
{
	int err;

	err = wait_while_busy(priv);
	if (err)
		goto fail;

	err = write_enable(priv);
	if (err)
		goto fail;

	err = wait_while_busy(priv);
	if (err)
		goto fail;

	writel(QSPI_CCR_FMODE_INDIRECT_WRITE
	       | SPINOR_OP_SE
	       | QSPI_CCR_IMODE_SINGLE_LINE
	       | QSPI_CCR_ADMODE_SINGLE_LINE
	       | (stm32_qspi->support_4bytes
		  ? QSPI_CCR_ADSIZE_FOUR_BYTES
		  : QSPI_CCR_ADSIZE_THREE_BYTES)
	       | QSPI_CCR_DMODE_NONE
	       | QSPI_CCR_DCYC(0),
		&priv->regs->ccr);

	writel(address, &priv->regs->ar);

	err = wait_until_complete(priv);
	if (err)
		goto fail;

	err = wait_while_writing(priv);
	if (err)
		goto fail;

	return 0;
fail:
	error("%s: failed: %d\n", __func__, err);
	return err;
}

static int erase(struct stm32_qspi_priv *priv, u32 address, size_t size)
{
	if (unlikely((address + size) > priv->size)) {
		error("%s: Erase past end of device\n", __func__);
		return -EINVAL;
	}

	if (size & (stm32_qspi->erase_size-1)) {
		error("%s: non-block erase: len %u\n", __func__, size);
		return -EINVAL;
	}

	if (address & (stm32_qspi->erase_size-1)) {
		error("%s: non-aligned erase: addr 0x%x\n", __func__, (u32)address);
		return -EINVAL;
	}

	stm32_qspi_abort(priv);

	while (size) {
		int err = erase_block(priv, address);
		if (err)
			return err;
		address += stm32_qspi->erase_size;
		size -= stm32_qspi->erase_size;
	}

	return switch_to_memory_mapped(priv);
}

static int write_page(struct stm32_qspi_priv *priv, u32 address, const u8 *buf, size_t size)
{
	int err;

	if (size > priv->write_size) {
		error("%s: size %u >  max write size %u\n",
		      __func__, size, priv->write_size);
		return -EINVAL;
	}

	err = wait_while_busy(priv);
	if (err)
		goto fail;

	err = write_enable(priv);
	if (err)
		goto fail;

	err = wait_while_busy(priv);
	if (err)
		goto fail;

	writel(size - 1, &priv->regs->dlr);

	writel(QSPI_CCR_FMODE_INDIRECT_WRITE
	       | CONFIG_STM32_QSPI_FAST_PROGRAM_CMD
	       | QSPI_CCR_IMODE_SINGLE_LINE
	       | QSPI_CCR_ADMODE_FOUR_LINES
	       | (stm32_qspi->support_4bytes
		  ? QSPI_CCR_ADSIZE_FOUR_BYTES
		  : QSPI_CCR_ADSIZE_THREE_BYTES)
	       | QSPI_CCR_DMODE_FOUR_LINES
	       | QSPI_CCR_DCYC(0),
		&priv->regs->ccr);

	writel(address, &priv->regs->ar);

	clrsetbits_le32(&priv->regs->ccr,
			QSPI_CCR_FMODE(3),
			QSPI_CCR_FMODE_INDIRECT_WRITE);

	while (size) {
		u8 *dr = (u8*)&priv->regs->dr;
		int err = wait_for_fifo(priv);
		if (err) {
			error("%s: write failed (fifo): %d\n", __func__, err);
			goto fail;
		}
		writeb(*buf++, dr);
		--size;
	}

	err = wait_until_complete(priv);
	if (err)
		goto fail;

	err = wait_while_writing(priv);
	if (err)
		goto fail;

	return 0;
fail:
	error("%s: failed: %d\n", __func__, err);
	return err;
}

static int write(struct stm32_qspi_priv *priv, u32 address, const u8 *buf, size_t size)
{
	size_t current_size;

	if (unlikely((address + size) > priv->size)) {
		error("%s: Write past end of device\n", __func__);
		return -EINVAL;
	}

	stm32_qspi_abort(priv);

	current_size =
		(((address & (priv->write_size-1)) + size) > priv->write_size)
		? (priv->write_size - (address & (priv->write_size-1)))
		: size;

	while (size) {
		int err = write_page(priv, address, buf, current_size);
		if (err) {
			error("%s: write failed: %d\n", __func__, err);
			return err;
		}

		size -= current_size;
		address += current_size;
		buf += current_size;
		current_size = (size > priv->write_size) ? priv->write_size : size;
	}

	return switch_to_memory_mapped(priv);
}

static int enter_4_bytes_mode(struct stm32_qspi_priv *priv)
{
	int err;

	err = wait_while_busy(priv);
	if (err)
		goto fail;

	err = write_enable(priv);
	if (err)
		goto fail;

	err = wait_while_busy(priv);
	if (err)
		goto fail;

	writel(QSPI_CCR_FMODE_INDIRECT_WRITE
	       | SPINOR_OP_EN4B
	       | QSPI_CCR_IMODE_SINGLE_LINE
	       | QSPI_CCR_ADMODE_NONE
	       | QSPI_CCR_DMODE_NONE
	       | QSPI_CCR_DCYC(0),
		&priv->regs->ccr);

	err = wait_until_complete(priv);
	if (err)
		goto fail;

	return 0;

fail:
	error("%s: failed: %d\n", __func__, err);
	return err;
}

int stm32_qspi_init(void)
{
	int err;

	if (stm32_qspi)
		return 0;

	stm32_qspi = malloc(sizeof(*stm32_qspi));
	if (!stm32_qspi) {
		error("%s: unable to allocate QSPI\n", __func__);
		return -ENOMEM;
	}

	stm32_qspi->regs = (struct stm32_qspi_regs *)(uintptr_t)STM32_QSPI_BASE;

	stm32_qspi->fast_read_dummy = CONFIG_STM32_QSPI_FAST_READ_DUMMY_CYCLES;
	stm32_qspi->size = 1 << CONFIG_SPI_FLASH_SIZE_OFF;
	stm32_qspi->erase_size = SPINOR_MAX_ERASE_SIZE;
	stm32_qspi->write_size = SPINOR_MAX_WRITE_SIZE;

	writel(QSPI_CR_APMS
	       | QSPI_CR_FTHRES(3)
	       | QSPI_CR_SSHIFT
	       | QSPI_CR_DMAEN,
	       &stm32_qspi->regs->cr);

	stm32_qspi_set_speed(stm32_qspi, CONFIG_STM32_QSPI_FREQ);

	setbits_le32(&stm32_qspi->regs->dcr, QSPI_DCR_CK_MODE);

	/* Number of bytes in Flash memory = 2^(FSIZE+1) */
	clrsetbits_le32(&stm32_qspi->regs->dcr,
			QSPI_DCR_FSIZE(0x1F),
			QSPI_DCR_FSIZE(CONFIG_SPI_FLASH_SIZE_OFF - 1));

	setbits_le32(&stm32_qspi->regs->cr, QSPI_CR_EN);
	err = wait_while_busy(stm32_qspi);
	if (err) {
		error("%s: unable to enable QSPI: %d\n", __func__, err);
		return err;
	}

	/* 4 bytes addressing is only supported for flashes bigger than 16MiB */
	stm32_qspi->support_4bytes = !!(CONFIG_SPI_FLASH_SIZE_OFF > 24);

	if (stm32_qspi->support_4bytes) {
		err = enter_4_bytes_mode(stm32_qspi);
		if (err) {
			error("%s: unable to switch to 4 byte mode addressing: %d\n", __func__, err);
			return err;
		}
	}

	err = switch_to_memory_mapped(stm32_qspi);
	if (err) {
		error("%s: unable to switch to memory mapping: %d\n", __func__, err);
		return err;
	}

	printf("QSPI:  %d MB mapped at 0x%x\n",
		1 << (CONFIG_SPI_FLASH_SIZE_OFF - 20), STM32_QSPI_BANK);

	return 0;
}

static inline int str2long(char *p, ulong *num)
{
	char *endptr;
	*num = simple_strtoul(p, &endptr, 16);
	return (*p != '\0' && *endptr == '\0' && p != endptr);
}

int do_qspi(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char *cmd;
	int err = -1;

	if (argc < 2) {
		cmd_usage(cmdtp);
		return 1;
	}

	cmd = argv[1];

	if (strcmp(cmd, "erase") == 0) {
		if ((argc == 3) && (strcmp(argv[2], "all") == 0)) {
			printf("Erase whole QSPI flash\n");
			err = erase(stm32_qspi, 0, stm32_qspi->size);
			printf("Erase whole QSPI flash: %s\n", err ? "FAIL" : "OK");
		} else if (argc == 4) {
			ulong off, size;
			if (str2long(argv[2], &off)
			    && str2long(argv[3], &size)) {
				if (size & (stm32_qspi->erase_size - 1)) {
					size += stm32_qspi->erase_size - 1;
					size &= ~(stm32_qspi->erase_size - 1);
					printf("Round the erase size up to 0x%lx to be a multiple of the block size 0x%x\n",
					       size, stm32_qspi->erase_size);
				}
				printf("Erase QSPI flash from 0x%lx to 0x%lx, estimated time %lu s\n",
				       off, off + size,
				       (CONFIG_STM32_QSPI_64KB_ERASE_TYP_TIME_MS * (size / 1000)) /
					stm32_qspi->erase_size);
				err = erase(stm32_qspi, off, size);
				printf("Erase QSPI flash: %s\n", err ? "FAIL" : "OK");
			} else {
				cmd_usage(cmdtp);
			}
		} else {
			cmd_usage(cmdtp);
		}
	} else if ((argc == 5) && (strcmp(cmd, "write") == 0)) {
		ulong addr, off, size;

		if (str2long(argv[2], &addr)
		    && str2long(argv[3], &off)
		    && str2long(argv[4], &size)) {
			printf("Write from memory 0x%lx to QSPI 0x%lx, size 0x%lx, estimated time %lu s\n",
			       addr, off, size,
			       (CONFIG_STM32_QSPI_256B_PROGRAM_TYP_TIME_US * (size / 1000)) /
				stm32_qspi->write_size / 1000);
			err = write(stm32_qspi, off, (u8*)addr, size);
			printf("Write from memory to QSPI: %s\n", err ? "FAIL" : "OK");
		} else {
			cmd_usage(cmdtp);
		}
	} else {
		cmd_usage(cmdtp);
	}

	return err;
}

U_BOOT_CMD(
	qspi,   CONFIG_SYS_MAXARGS,   0,  do_qspi,
	"STM32 Quad SPI flash",
	"qspi erase [off size] - erase 'size' bytes starting at offset 'off'\n"
	"qspi erase all - erase whole flash\n"
	"qspi write addr off size\n"
	"    read/write 'size' bytes starting at offset 'off'\n"
	"    to/from memory address 'addr'\n"
);
