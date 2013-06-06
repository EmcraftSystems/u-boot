/*******************************************************************************
 * (c) Copyright 2008 Actel Corporation.  All rights reserved.
 * 
 * SmartFusion microcontroller subsystem SPI bare metal software driver
 * implementation.
 *
 * SVN $Revision: 4566 $
 * SVN $Date: 2012-08-23 17:12:11 +0100 (Thu, 23 Aug 2012) $
 */
#include <common.h>
#include "mss_spi.h"
/* #include "../../CMSIS/mss_assert.h" */

#ifdef __cplusplus
extern "C" {
#endif 
    
/***************************************************************************//**
  MSS SPI can operate as master or slave.
 */
#define MSS_SPI_MODE_SLAVE      0u
#define MSS_SPI_MODE_MASTER     1u

/***************************************************************************//**
 * Mask of transfer protocol and SPO, SPH bits within control register.
 */
#define PROTOCOL_MODE_MASK  (uint32_t)0x030000C0

/***************************************************************************//**
 * Mask of theframe count bits within the SPI control register.
 */
#define TXRXDFCOUNT_MASK    (uint32_t)0x00FFFF00
#define TXRXDFCOUNT_SHIFT   (uint32_t)8

/***************************************************************************//**
 * SPI hardware FIFO depth.
 */
#define RX_FIFO_SIZE    4u
#define BIG_FIFO_SIZE   32u

/***************************************************************************//**
 * 
 */
#define RX_IRQ_THRESHOLD    (BIG_FIFO_SIZE / 2u)

/***************************************************************************//**
  Marker used to detect that the configuration has not been selected for a
  specific slave when operating as a master.
 */
#define NOT_CONFIGURED  0xFFFFFFFF

/***************************************************************************//**
 * CONTROL register bit masks
 */
#define CTRL_ENABLE_MASK    0x00000001u
#define CTRL_MASTER_MASK    0x00000002u

/***************************************************************************//**
  Registers bit masks
 */
/* CONTROL register. */
#define MASTER_MODE_MASK        0x00000002u
#define CTRL_RX_IRQ_EN_MASK     0x00000010u
#define CTRL_TX_IRQ_EN_MASK     0x00000020u
#define CTRL_REG_RESET_MASK     0x80000000u
#define BIGFIFO_MASK            0x20000000u

/* CONTROL2 register */
#define ENABLE_CMD_IRQ_MASK     0x00000010u
#define ENABLE_SSEND_IRQ_MASK   0x00000020u

/* STATUS register */
#define TX_DONE_MASK            0x00000001u
#define RX_DATA_READY_MASK      0x00000002u
#define RX_OVERFLOW_MASK        0x00000004u
#define RX_FIFO_EMPTY_MASK      0x00000040u
#define TX_FIFO_FULL_MASK       0x00000100u
#define TX_FIFO_EMPTY_MASK      0x00000400u

/* MIS register. */
#define TXDONE_IRQ_MASK         0x00000001u
#define RXDONE_IRQ_MASK         0x00000002u
#define RXOVER_IRQ_MASK         0x00000004u
#define RXUNDER_IRQ_MASK        0x00000008u
#define CMD_IRQ_MASK            0x00000010u
#define SSEND_IRQ_MASK          0x00000020u

/* COMMAND register */
#define AUTOFILL_MASK           0x00000001u
#define RX_FIFO_RESET_MASK      0x00000004u
#define TX_FIFO_RESET_MASK      0x00000008u

/***************************************************************************//**
 * SPI instance data structures for SPI0 and SPI1. A pointer to these data
 * structures must be used as first parameter to any of the SPI driver functions
 * to identify the SPI hardware block that will perform the requested operation.
 */
mss_spi_instance_t g_mss_spi0;
volatile mss_spi_instance_t g_mss_spi1;

/***************************************************************************//**
  SPI0 interrupt service routine
 */
#if defined(__GNUC__)
__attribute__((__interrupt__)) void SPI0_IRQHandler(void);
#else
void SPI0_IRQHandler(void);
#endif

/***************************************************************************//**
  SPI1 interrupt service routine
 */
#if defined(__GNUC__)
__attribute__((__interrupt__)) void SPI1_IRQHandler(void);
#else
void SPI1_IRQHandler(void);
#endif

/***************************************************************************//**
  local functions
 */
static void recover_from_rx_overflow(volatile mss_spi_instance_t * this_spi);
/* static void fill_slave_tx_fifo(mss_spi_instance_t * this_spi); */
/* static void read_slave_rx_fifo(mss_spi_instance_t * this_spi); */

/***************************************************************************//**
 * MSS_SPI_init()
 * See "mss_spi.h" for details of how to use this function.
 */
void MSS_SPI_init
(
    volatile mss_spi_instance_t * this_spi
)
{
    uint16_t i;
    
    ASSERT((this_spi == &g_mss_spi0) || (this_spi == &g_mss_spi1));
    
    if(this_spi == &g_mss_spi0)
    {
        this_spi->hw_reg = ((SPI_REVB_TypeDef *) SPI0_BASE);
        this_spi->irqn = SPI0_IRQn;

        /* reset SPI0 */
        SYSREG->SOFT_RST_CR |= SYSREG_SPI0_SOFTRESET_MASK;
        /* Clear any previously pended SPI0 interrupt */
        /* NVIC_ClearPendingIRQ(SPI0_IRQn); */
        /* Take SPI0 out of reset. */
        SYSREG->SOFT_RST_CR &= ~SYSREG_SPI0_SOFTRESET_MASK;

        this_spi->hw_reg->CONTROL &= ~CTRL_REG_RESET_MASK;
    }
    else
    {
        this_spi->hw_reg = ((SPI_REVB_TypeDef *) SPI1_BASE);
        this_spi->irqn = SPI1_IRQn;
        
        /* reset SPI1 */
        SYSREG->SOFT_RST_CR |= SYSREG_SPI1_SOFTRESET_MASK;
        /* Clear any previously pended SPI1 interrupt */
        /* NVIC_ClearPendingIRQ(SPI1_IRQn); */
        /* Take SPI1 out of reset. */
        SYSREG->SOFT_RST_CR &= ~SYSREG_SPI1_SOFTRESET_MASK;
        
        this_spi->hw_reg->CONTROL &= ~CTRL_REG_RESET_MASK;
    }
    
    this_spi->frame_rx_handler = 0u;
    this_spi->slave_tx_frame = 0u;
    
    this_spi->block_rx_handler = 0u;
    
    this_spi->slave_tx_buffer = 0u;
    this_spi->slave_tx_size = 0u;
    this_spi->slave_tx_idx = 0u;

    this_spi->resp_tx_buffer = 0u;
    this_spi->resp_buff_size = 0u;
    this_spi->resp_buff_tx_idx = 0u;
    
    for(i = 0u; i < (uint16_t)MSS_SPI_MAX_NB_OF_SLAVES; ++i)
    {
        this_spi->slaves_cfg[i].ctrl_reg = NOT_CONFIGURED;
    }
}

/***************************************************************************//**
 *
 *
 */
static void recover_from_rx_overflow
(
 volatile    mss_spi_instance_t * this_spi
)
{
    uint32_t control_reg;
    uint32_t clk_gen;
    uint32_t frame_size;
    
    /*
     * Read current SPI hardware block configuration.
     */
    control_reg = this_spi->hw_reg->CONTROL;
    clk_gen = this_spi->hw_reg->CLK_GEN;
    frame_size = this_spi->hw_reg->TXRXDF_SIZE;
     
    /*
     * Reset the SPI hardware block.
     */
    if(this_spi == &g_mss_spi0)
    {
        this_spi->hw_reg = ((SPI_REVB_TypeDef *) SPI0_BASE);
        this_spi->irqn = SPI0_IRQn;

        /* reset SPI0 */
        SYSREG->SOFT_RST_CR |= SYSREG_SPI0_SOFTRESET_MASK;
        /* Clear any previously pended SPI0 interrupt */
        /* NVIC_ClearPendingIRQ(SPI0_IRQn); */
        /* Take SPI0 out of reset. */
        SYSREG->SOFT_RST_CR &= ~SYSREG_SPI0_SOFTRESET_MASK;

        this_spi->hw_reg->CONTROL &= ~CTRL_REG_RESET_MASK;
    }
    else
    {
        this_spi->hw_reg = ((SPI_REVB_TypeDef *) SPI1_BASE);
        this_spi->irqn = SPI1_IRQn;
        
        /* reset SPI1 */
        SYSREG->SOFT_RST_CR |= SYSREG_SPI1_SOFTRESET_MASK;
        /* Clear any previously pended SPI1 interrupt */
        /* NVIC_ClearPendingIRQ(SPI1_IRQn); */
        /* Take SPI1 out of reset. */
        SYSREG->SOFT_RST_CR &= ~SYSREG_SPI1_SOFTRESET_MASK;
        
        this_spi->hw_reg->CONTROL &= ~CTRL_REG_RESET_MASK;
    }
    
    /*
     * Restore SPI hardware block configuration.
     */
    this_spi->hw_reg->CONTROL &= ~CTRL_ENABLE_MASK;
    this_spi->hw_reg->CONTROL = control_reg;
    this_spi->hw_reg->CLK_GEN = clk_gen;
    this_spi->hw_reg->TXRXDF_SIZE = frame_size;
    this_spi->hw_reg->CONTROL |= CTRL_ENABLE_MASK;
}

/* /\***************************************************************************\//\** */
/*  * MSS_SPI_configure_slave_mode() */
/*  * See "mss_spi.h" for details of how to use this function. */
/*  *\/ */
/* void MSS_SPI_configure_slave_mode */
/* ( */
/*     mss_spi_instance_t * this_spi, */
/*     mss_spi_protocol_mode_t protocol_mode, */
/*     mss_spi_pclk_div_t clk_rate, */
/*     uint8_t frame_bit_length */
/* ) */
/* { */
/*     ASSERT((this_spi == &g_mss_spi0) || (this_spi == &g_mss_spi1)); */
/*     ASSERT(frame_bit_length <= 32); */
    
/*     /\* Set the mode. *\/ */
/*     this_spi->hw_reg->CONTROL &= ~CTRL_MASTER_MASK; */

/*     /\* Set the clock rate. *\/ */
/*     this_spi->hw_reg->CONTROL &= ~CTRL_ENABLE_MASK; */
/*     this_spi->hw_reg->CONTROL = (this_spi->hw_reg->CONTROL & ~PROTOCOL_MODE_MASK) | */
/*                                 (uint32_t)protocol_mode | BIGFIFO_MASK; */
                                
/*     this_spi->hw_reg->CLK_GEN = (uint32_t)clk_rate; */
    
/*     /\* Set default frame size to byte size and number of data frames to 1. *\/ */
/*     this_spi->hw_reg->CONTROL = (this_spi->hw_reg->CONTROL & ~TXRXDFCOUNT_MASK) | ((uint32_t)1 << TXRXDFCOUNT_SHIFT); */
/*     this_spi->hw_reg->TXRXDF_SIZE = frame_bit_length; */
/*     this_spi->hw_reg->CONTROL |= CTRL_ENABLE_MASK; */
/* } */

/***************************************************************************//**
 * MSS_SPI_configure_master_mode()
 * See "mss_spi.h" for details of how to use this function.
 */
void MSS_SPI_configure_master_mode
(
    volatile mss_spi_instance_t *    this_spi,
    mss_spi_slave_t         slave,
    mss_spi_protocol_mode_t protocol_mode,
    mss_spi_pclk_div_t      clk_rate,
    uint8_t                 frame_bit_length
)
{
    ASSERT((this_spi == &g_mss_spi0) || (this_spi == &g_mss_spi1));
    ASSERT(slave < MSS_SPI_MAX_NB_OF_SLAVES);
    ASSERT(frame_bit_length <= 32);
    
    /* Set the mode. */
    this_spi->hw_reg->CONTROL &= ~CTRL_ENABLE_MASK;
    this_spi->hw_reg->CONTROL |= CTRL_MASTER_MASK;
    this_spi->hw_reg->CONTROL |= CTRL_ENABLE_MASK;

    /*
     * Keep track of the required register configuration for this slave. These
     * values will be used by the MSS_SPI_set_slave_select() function to configure
     * the master to match the slave being selected.
     */
    if(slave < MSS_SPI_MAX_NB_OF_SLAVES)     
    {
        this_spi->slaves_cfg[slave].ctrl_reg = MASTER_MODE_MASK |
                                               BIGFIFO_MASK |
                                               (uint32_t)protocol_mode | 
                                               ((uint32_t)1 << TXRXDFCOUNT_SHIFT);
        this_spi->slaves_cfg[slave].txrxdf_size_reg = frame_bit_length;
        this_spi->slaves_cfg[slave].clk_gen = (uint8_t)clk_rate;
    }
}

/***************************************************************************//**
 * MSS_SPI_set_slave_select()
 * See "mss_spi.h" for details of how to use this function.
 */
void MSS_SPI_set_slave_select
(
    volatile mss_spi_instance_t * this_spi,
    mss_spi_slave_t slave
)
{
    uint32_t rx_overflow;
    
    ASSERT((this_spi == &g_mss_spi0) || (this_spi == &g_mss_spi1));
    
    /* This function is only intended to be used with an SPI master. */
    ASSERT((this_spi->hw_reg->CONTROL & CTRL_MASTER_MASK) == CTRL_MASTER_MASK);
    
    ASSERT(this_spi->slaves_cfg[slave].ctrl_reg != NOT_CONFIGURED);

    /* Recover from receive overflow. */
    rx_overflow = this_spi->hw_reg->STATUS & RX_OVERFLOW_MASK;
    if(rx_overflow)
    {
         recover_from_rx_overflow(this_spi);
    }
    
    /* Set the clock rate. */
    this_spi->hw_reg->CONTROL &= ~CTRL_ENABLE_MASK;
    this_spi->hw_reg->CONTROL = this_spi->slaves_cfg[slave].ctrl_reg;
    this_spi->hw_reg->CLK_GEN = this_spi->slaves_cfg[slave].clk_gen;
    this_spi->hw_reg->TXRXDF_SIZE = this_spi->slaves_cfg[slave].txrxdf_size_reg;
    this_spi->hw_reg->CONTROL |= CTRL_ENABLE_MASK;
    
    /* Set slave select */
    this_spi->hw_reg->SLAVE_SELECT |= ((uint32_t)1 << (uint32_t)slave);
}

/***************************************************************************//**
 * MSS_SPI_clear_slave_select()
 * See "mss_spi.h" for details of how to use this function.
 */
void MSS_SPI_clear_slave_select
(
    mss_spi_instance_t * this_spi,
    mss_spi_slave_t slave
)
{
    uint32_t rx_overflow;
    
    ASSERT((this_spi == &g_mss_spi0) || (this_spi == &g_mss_spi1));
    
    /* This function is only intended to be used with an SPI master. */
    ASSERT((this_spi->hw_reg->CONTROL & CTRL_MASTER_MASK) == CTRL_MASTER_MASK);

    /* Recover from receive overflow. */
    rx_overflow = this_spi->hw_reg->STATUS & RX_OVERFLOW_MASK;
    if(rx_overflow)
    {
         recover_from_rx_overflow(this_spi);
    }
    
    this_spi->hw_reg->SLAVE_SELECT &= ~((uint32_t)1 << (uint32_t)slave);
}

/***************************************************************************//**
 * MSS_SPI_transfer_frame()
 * See "mss_spi.h" for details of how to use this function.
 */
uint32_t MSS_SPI_transfer_frame
(
	volatile mss_spi_instance_t * this_spi,
    uint32_t tx_bits
)
{
    volatile uint32_t dummy;
    uint32_t rx_ready;
    uint32_t tx_done;
    
    ASSERT((this_spi == &g_mss_spi0) || (this_spi == &g_mss_spi1));
    
    /* This function is only intended to be used with an SPI master. */
    ASSERT((this_spi->hw_reg->CONTROL & CTRL_MASTER_MASK) == CTRL_MASTER_MASK);
    
    /* printf("%s %d %s  %08x\n", __FILE__, __LINE__, __FUNCTION__, this_spi->hw_reg->STATUS); */
    /* Flush Rx FIFO. */
    rx_ready = this_spi->hw_reg->STATUS & RX_DATA_READY_MASK;
    while(rx_ready)
    {
        dummy = this_spi->hw_reg->RX_DATA;
        dummy = dummy;  /* Prevent Lint warning. */
        rx_ready = this_spi->hw_reg->STATUS & RX_DATA_READY_MASK;
    }
    
    /* printf("%s %d %s  %08x\n", __FILE__, __LINE__, __FUNCTION__, this_spi->hw_reg->STATUS); */
    /* Send frame. */
    this_spi->hw_reg->TX_DATA = tx_bits;
    
    /* Wait for frame Tx to complete. */
    tx_done = this_spi->hw_reg->STATUS & TX_DONE_MASK;
    while(!tx_done)
    {
        tx_done = this_spi->hw_reg->STATUS & TX_DONE_MASK;
	/* printf("%s %d %s  %08x\n", __FILE__, __LINE__, __FUNCTION__, this_spi->hw_reg->STATUS); */
    }
    /* printf("%s %d %s  %08x\n", __FILE__, __LINE__, __FUNCTION__, this_spi->hw_reg->STATUS); */
    
    /* Read received frame. */
    /* Wait for Rx complete. */
    rx_ready = this_spi->hw_reg->STATUS & RX_DATA_READY_MASK;
    while(!rx_ready)
    {
        rx_ready = this_spi->hw_reg->STATUS & RX_DATA_READY_MASK;
    }
	/* printf("%s %d %s\n", __FILE__, __LINE__, __FUNCTION__); */
    /* Return Rx data. */
    return( this_spi->hw_reg->RX_DATA );
}


/* /\***************************************************************************\//\** */
/*  * MSS_SPI_transfer_block() */
/*  * See "mss_spi.h" for details of how to use this function. */
/*  *\/ */
/* void MSS_SPI_transfer_block */
/* ( */
/*     mss_spi_instance_t * this_spi, */
/*     const uint8_t * cmd_buffer, */
/*     uint16_t cmd_byte_size, */
/*     uint8_t * rd_buffer, */
/*     uint16_t rd_byte_size */
/* ) */
/* { */
/*     uint16_t transfer_idx = 0u; */
/*     uint16_t tx_idx; */
/*     uint16_t rx_idx; */
/*     uint32_t frame_count; */
/*     volatile uint32_t rx_raw; */
/*     uint16_t transit = 0u; */
/*     uint32_t tx_fifo_full; */
/*     uint32_t rx_overflow; */
/*     uint32_t rx_fifo_empty; */
    
/*     uint16_t transfer_size;     /\* Total number of bytes transfered. *\/ */
    
/*     ASSERT((this_spi == &g_mss_spi0) || (this_spi == &g_mss_spi1)); */
    
/*     /\* This function is only intended to be used with an SPI master. *\/ */
/*     ASSERT((this_spi->hw_reg->CONTROL & CTRL_MASTER_MASK) == CTRL_MASTER_MASK); */
    
/*     /\* Compute number of bytes to transfer. *\/ */
/*     transfer_size = cmd_byte_size + rd_byte_size; */
    
/*     /\* Adjust to 1 byte transfer to cater for DMA transfers. *\/ */
/*     if(transfer_size == 0u) */
/*     { */
/*         frame_count = 1u; */
/*     } */
/*     else */
/*     { */
/*         frame_count = transfer_size; */
/*     } */

/*     /\* Recover from receive overflow. *\/ */
/*     rx_overflow = this_spi->hw_reg->STATUS & RX_OVERFLOW_MASK; */
/*     if(rx_overflow) */
/*     { */
/*          recover_from_rx_overflow(this_spi); */
/*     } */
    
/*     /\* Set frame size to 8 bits and the frame count to the transfer size. *\/ */
/*     this_spi->hw_reg->CONTROL &= ~CTRL_ENABLE_MASK; */
/*     this_spi->hw_reg->CONTROL = (this_spi->hw_reg->CONTROL & ~TXRXDFCOUNT_MASK) | ( (frame_count << TXRXDFCOUNT_SHIFT) & TXRXDFCOUNT_MASK); */
/*     this_spi->hw_reg->TXRXDF_SIZE = 8u; */
/*     this_spi->hw_reg->CONTROL |= CTRL_ENABLE_MASK; */

/*     /\* Flush the receive FIFO. *\/ */
/*     rx_fifo_empty = this_spi->hw_reg->STATUS & RX_FIFO_EMPTY_MASK; */
/*     while(!rx_fifo_empty) */
/*     { */
/*         rx_raw = this_spi->hw_reg->RX_DATA; */
/*         rx_fifo_empty = this_spi->hw_reg->STATUS & RX_FIFO_EMPTY_MASK; */
/*     } */
    
/*     tx_idx = 0u; */
/*     rx_idx = 0u; */
/*     if(tx_idx < cmd_byte_size) */
/*     { */
/*         this_spi->hw_reg->TX_DATA = cmd_buffer[tx_idx]; */
/*         ++tx_idx; */
/*         ++transit; */
/*     } */
/*     else */
/*     { */
/*         if(tx_idx < transfer_size) */
/*         { */
/*             this_spi->hw_reg->TX_DATA = 0x00u; */
/*             ++tx_idx; */
/*             ++transit; */
/*         } */
/*     } */
/*     /\* Perform the remainder of the transfer by sending a byte every time a byte */
/*      * has been received. This should ensure that no Rx overflow can happen in */
/*      * case of an interrupt occurs during this function. *\/ */
/*     while(transfer_idx < transfer_size) */
/*     { */
/*         rx_fifo_empty = this_spi->hw_reg->STATUS & RX_FIFO_EMPTY_MASK; */
/*         if(!rx_fifo_empty) */
/*         { */
/*             /\* Process received byte. *\/ */
/*             rx_raw = this_spi->hw_reg->RX_DATA; */
/*             if(transfer_idx >= cmd_byte_size) */
/*             { */
/*                 if(rx_idx < rd_byte_size) */
/*                 { */
/*                     rd_buffer[rx_idx] = (uint8_t)rx_raw;    */
/*                 } */
/*                 ++rx_idx; */
/*             } */
/*             ++transfer_idx; */
/*             --transit; */
/*         } */

/*         tx_fifo_full = this_spi->hw_reg->STATUS & TX_FIFO_FULL_MASK; */
/*         if(!tx_fifo_full) */
/*         { */
/*             if(transit < RX_FIFO_SIZE) */
/*             { */
/*                 /\* Send another byte. *\/ */
/*                 if(tx_idx < cmd_byte_size) */
/*                 { */
/*                     this_spi->hw_reg->TX_DATA = cmd_buffer[tx_idx]; */
/*                     ++tx_idx; */
/*                     ++transit; */
/*                 } */
/*                 else */
/*                 { */
/*                     if(tx_idx < transfer_size) */
/*                     { */
/*                         this_spi->hw_reg->TX_DATA = 0x00u; */
/*                         ++tx_idx; */
/*                         ++transit; */
/*                     } */
/*                 } */
/*             } */
/*         } */
/*     } */
/* } */

/* /\***************************************************************************\//\** */
/*  * MSS_SPI_set_frame_rx_handler() */
/*  * See "mss_spi.h" for details of how to use this function. */
/*  *\/ */
/* void MSS_SPI_set_frame_rx_handler */
/* ( */
/*     mss_spi_instance_t * this_spi, */
/*     mss_spi_frame_rx_handler_t rx_handler */
/* ) */
/* { */
/*     uint32_t tx_fifo_empty; */
    
/*     ASSERT((this_spi == &g_mss_spi0) || (this_spi == &g_mss_spi1)); */
    
/*     /\* This function is only intended to be used with an SPI slave. *\/ */
/*     ASSERT((this_spi->hw_reg->CONTROL & CTRL_MASTER_MASK) != CTRL_MASTER_MASK); */
    
/*     /\* Disable block Rx handler as they are mutually exclusive. *\/ */
/*     this_spi->block_rx_handler = 0u; */
    
/*     /\* Keep a copy of the pointer to the rx hnadler function. *\/ */
/*     this_spi->frame_rx_handler = rx_handler; */
    
/*     /\* Automatically fill the TX FIFO with zeroes if no slave tx frame set.*\/ */
/*     tx_fifo_empty = this_spi->hw_reg->STATUS & TX_FIFO_EMPTY_MASK; */
/*     if(tx_fifo_empty) */
/*     { */
/*         this_spi->hw_reg->COMMAND |= AUTOFILL_MASK; */
/*     } */
    
/*     /\* Enable Rx interrupt. *\/ */
/*     this_spi->hw_reg->CONTROL |= CTRL_RX_IRQ_EN_MASK; */
/*     /\* NVIC_EnableIRQ( this_spi->irqn ); *\/ */
/* } */

/* /\***************************************************************************\//\** */
/*  * MSS_SPI_set_slave_tx_frame() */
/*  * See "mss_spi.h" for details of how to use this function. */
/*  *\/ */
/* void MSS_SPI_set_slave_tx_frame */
/* ( */
/*     mss_spi_instance_t * this_spi, */
/*     uint32_t frame_value */
/* ) */
/* { */
/*     ASSERT((this_spi == &g_mss_spi0) || (this_spi == &g_mss_spi1)); */

/*     /\* This function is only intended to be used with an SPI slave. *\/ */
/*     ASSERT((this_spi->hw_reg->CONTROL & CTRL_MASTER_MASK) != CTRL_MASTER_MASK); */
    
/*     /\* Disable slave block tx buffer as it is mutually exclusive with frame */
/*      * level handling. *\/     */
/*     this_spi->slave_tx_buffer = 0u; */
/*     this_spi->slave_tx_size = 0u; */
/*     this_spi->slave_tx_idx = 0u; */
    
/*     /\* Keep a copy of the slave tx frame value. *\/ */
/*     this_spi->slave_tx_frame = frame_value; */
    
/*     /\* Disable automatic fill of the TX FIFO with zeroes.*\/ */
/*     this_spi->hw_reg->COMMAND &= ~AUTOFILL_MASK; */
/*     this_spi->hw_reg->COMMAND |= TX_FIFO_RESET_MASK; */
    
/*     /\* Load frame into Tx data register. *\/ */
/*     this_spi->hw_reg->TX_DATA = this_spi->slave_tx_frame; */
    
/*     /\* Enable Tx Done interrupt in order to reload the slave Tx frame after each */
/*      * time it has been sent. *\/ */
/*     this_spi->hw_reg->CONTROL |= CTRL_TX_IRQ_EN_MASK; */
/*     /\* NVIC_EnableIRQ( this_spi->irqn ); *\/ */
/* } */

/* /\***************************************************************************\//\** */
/*  * MSS_SPI_set_slave_block_buffers() */
/*  * See "mss_spi.h" for details of how to use this function. */
/*  *\/ */
/* void MSS_SPI_set_slave_block_buffers */
/* ( */
/*     mss_spi_instance_t * this_spi, */
/*     const uint8_t * tx_buffer, */
/*     uint32_t tx_buff_size, */
/*     uint8_t * rx_buffer, */
/*     uint32_t rx_buff_size, */
/*     mss_spi_block_rx_handler_t block_rx_handler */
/* ) */
/* { */
/*     uint32_t frame_count; */
/*     uint32_t tx_fifo_full; */
    
/*     ASSERT((this_spi == &g_mss_spi0) || (this_spi == &g_mss_spi1)); */
    
/*     /\* This function is only intended to be used with an SPI slave. *\/ */
/*     ASSERT((this_spi->hw_reg->CONTROL & CTRL_MASTER_MASK) != CTRL_MASTER_MASK); */
    
/*     /\* Disable Rx frame handler as it is mutually exclusive with block rx handler. *\/ */
/*     this_spi->frame_rx_handler = 0u; */
    
/*     /\* Keep a copy of the pointer to the block rx handler function. *\/ */
/*     this_spi->block_rx_handler = block_rx_handler; */
    
/*     this_spi->slave_rx_buffer = rx_buffer; */
/*     this_spi->slave_rx_size = rx_buff_size; */
/*     this_spi->slave_rx_idx = 0u; */
    
/*     /\* Initialise the transmit state data. *\/ */
/*     this_spi->slave_tx_buffer = tx_buffer; */
/*     this_spi->slave_tx_size = tx_buff_size; */
/*     this_spi->slave_tx_idx = 0u; */

/*     /\* Use the frame counter to control how often receive interrupts are generated. *\/ */
/*     frame_count = RX_IRQ_THRESHOLD; */
    
/*     /\**\/ */
/*     this_spi->hw_reg->CONTROL &= ~CTRL_ENABLE_MASK; */
/*     this_spi->hw_reg->CONTROL = (this_spi->hw_reg->CONTROL & ~TXRXDFCOUNT_MASK) | (frame_count << TXRXDFCOUNT_SHIFT); */
/*     this_spi->hw_reg->TXRXDF_SIZE = 8u; */
/*     this_spi->hw_reg->CONTROL |= CTRL_ENABLE_MASK; */
    
/*     /\* Load the transmit FIFO. *\/ */
/*     tx_fifo_full = this_spi->hw_reg->STATUS & TX_FIFO_FULL_MASK; */
/*     while(!tx_fifo_full && (this_spi->slave_tx_idx < this_spi->slave_tx_size)) */
/*     { */
/*         this_spi->hw_reg->TX_DATA = this_spi->slave_tx_buffer[this_spi->slave_tx_idx]; */
/*         ++this_spi->slave_tx_idx; */
/*         tx_fifo_full = this_spi->hw_reg->STATUS & TX_FIFO_FULL_MASK; */
/*     } */
    
/*     /\* Enable Rx interrupt. *\/ */
/*     this_spi->hw_reg->CONTROL |= CTRL_RX_IRQ_EN_MASK; */
    
    
/*     if(tx_buff_size > 0) */
/*     { */
/*         this_spi->hw_reg->COMMAND &= ~AUTOFILL_MASK; */
/*         this_spi->hw_reg->CONTROL |= CTRL_TX_IRQ_EN_MASK; */
/*     } */
/*     else */
/*     { */
/*         this_spi->hw_reg->COMMAND |= AUTOFILL_MASK; */
/*     } */
    
/*     /\* */
/*      * Enable slave select release interrupt. The SSEND interrupt is used to */
/*      * complete reading of the recieve FIFO and prepare the transmit FIFO for */
/*      * the next transaction. */
/*      *\/ */
/*     this_spi->hw_reg->CONTROL2 |= ENABLE_SSEND_IRQ_MASK; */
    
/*     /\* NVIC_EnableIRQ(this_spi->irqn); *\/ */
/* } */

/* /\***************************************************************************\//\** */
/*  * MSS_SPI_set_cmd_handler() */
/*  * See "mss_spi.h" for details of how to use this function. */
/*  *\/ */
/* void MSS_SPI_set_cmd_handler */
/* ( */
/*     mss_spi_instance_t * this_spi, */
/*     mss_spi_block_rx_handler_t cmd_handler, */
/*     uint32_t cmd_size */
/* ) */
/* { */
/*     this_spi->cmd_handler = cmd_handler; */
/*     this_spi->hw_reg->CMDSIZE = cmd_size; */
/*     this_spi->hw_reg->CONTROL2 |= ENABLE_CMD_IRQ_MASK; */
/* } */

/* /\***************************************************************************\//\** */
/*  * MSS_SPI_set_cmd_response() */
/*  * See "mss_spi.h" for details of how to use this function. */
/*  *\/ */
/* void MSS_SPI_set_cmd_response */
/* ( */
/*     mss_spi_instance_t * this_spi, */
/*     const uint8_t * resp_tx_buffer, */
/*     uint32_t resp_buff_size */
/* ) */
/* { */
/*     this_spi->resp_tx_buffer = resp_tx_buffer; */
/*     this_spi->resp_buff_size = resp_buff_size; */
/*     this_spi->resp_buff_tx_idx = 0u; */
    
/*     fill_slave_tx_fifo(this_spi); */
/* } */

/***************************************************************************//**
 * MSS_SPI_enable()
 * See "mss_spi.h" for details of how to use this function.
 */
void MSS_SPI_enable
(
    volatile mss_spi_instance_t * this_spi
)
{
    this_spi->hw_reg->CONTROL |= CTRL_ENABLE_MASK;
}

/***************************************************************************//**
 * MSS_SPI_disable()
 * See "mss_spi.h" for details of how to use this function.
 */
void MSS_SPI_disable
(
    volatile mss_spi_instance_t * this_spi
)
{
    this_spi->hw_reg->CONTROL &= ~CTRL_ENABLE_MASK;
}

/* /\***************************************************************************\//\** */
/*  * MSS_SPI_set_transfer_byte_count() */
/*  * See "mss_spi.h" for details of how to use this function. */
/*  *\/ */
/* void MSS_SPI_set_transfer_byte_count */
/* ( */
/*     volatile mss_spi_instance_t * this_spi, */
/*     uint16_t byte_count */
/* ) */
/* { */
/*     this_spi->hw_reg->CONTROL = (this_spi->hw_reg->CONTROL & ~TXRXDFCOUNT_MASK) */
/*                                 | ((byte_count << TXRXDFCOUNT_SHIFT) & TXRXDFCOUNT_MASK); */
                                
/*     this_spi->hw_reg->TXRXDF_SIZE = 8u; */
/* } */

/* /\***************************************************************************\//\** */
/*  * MSS_SPI_tx_done() */
/*  * See "mss_spi.h" for details of how to use this function. */
/*  *\/ */
/* uint32_t MSS_SPI_tx_done */
/* ( */
/*     mss_spi_instance_t * this_spi */
/* ) */
/* { */
/*     uint32_t tx_done; */
    
/*     tx_done = this_spi->hw_reg->STATUS & TX_DONE_MASK; */
    
/*     return tx_done; */
/* } */

/* /\***************************************************************************\//\** */
/*  * Fill the transmit FIFO (used for slave block transfers). */
/*  *\/ */
/* static void fill_slave_tx_fifo */
/* ( */
/*     mss_spi_instance_t * this_spi */
/* ) */
/* { */
/*     uint32_t more_data = 1u; */
/*     uint32_t tx_fifo_full; */
    
/*     tx_fifo_full = this_spi->hw_reg->STATUS & TX_FIFO_FULL_MASK; */
    
/*     while(!tx_fifo_full && more_data) */
/*     { */
/*         if(this_spi->slave_tx_idx < this_spi->slave_tx_size) */
/*         { */
/*             this_spi->hw_reg->TX_DATA = this_spi->slave_tx_buffer[this_spi->slave_tx_idx]; */
/*             ++this_spi->slave_tx_idx; */
/*         } */
/*         else if(this_spi->resp_buff_tx_idx < this_spi->resp_buff_size) */
/*         { */
/*             this_spi->hw_reg->TX_DATA = this_spi->resp_tx_buffer[this_spi->resp_buff_tx_idx]; */
/*             ++this_spi->resp_buff_tx_idx; */
/*         } */
/*         else */
/*         { */
/*             more_data = 0u; */
/*         } */
/*         tx_fifo_full = this_spi->hw_reg->STATUS & TX_FIFO_FULL_MASK; */
/*     } */
/* } */

/***************************************************************************//**
 * 
 */
/* static void read_slave_rx_fifo */
/* ( */
/*     mss_spi_instance_t * this_spi */
/* ) */
/* { */
/*     uint32_t rx_frame; */
/*     uint32_t rx_fifo_empty; */
    
/*     if(this_spi->frame_rx_handler != 0u) */
/*     { */
/*         rx_fifo_empty = this_spi->hw_reg->STATUS & RX_FIFO_EMPTY_MASK; */
        
/*         while(!rx_fifo_empty) */
/*         { */
/*             /\* Single frame handling mode. *\/ */
/*             rx_frame = this_spi->hw_reg->RX_DATA; */
/*             this_spi->frame_rx_handler( rx_frame ); */
/*             rx_fifo_empty = this_spi->hw_reg->STATUS & RX_FIFO_EMPTY_MASK; */
/*         } */
/*     } */
/*     else  */
/*     { */
/*         /\* Block handling mode. *\/ */
/*         rx_fifo_empty = this_spi->hw_reg->STATUS & RX_FIFO_EMPTY_MASK; */
        
/*         while((!rx_fifo_empty) && (this_spi->slave_rx_idx < this_spi->slave_rx_size)) */
/*         { */
/*             rx_frame = this_spi->hw_reg->RX_DATA; */
/*             this_spi->slave_rx_buffer[this_spi->slave_rx_idx] = (uint8_t)rx_frame; */
/*             ++this_spi->slave_rx_idx; */
/*             rx_fifo_empty = this_spi->hw_reg->STATUS & RX_FIFO_EMPTY_MASK; */
/*         } */
/*     } */
/* } */

/***************************************************************************//**
 * SPI interrupt service routine.
 */
/* static void mss_spi_isr */
/* ( */
/*     mss_spi_instance_t * this_spi */
/* ) */
/* { */
/*     uint32_t tx_done; */
    
/*     ASSERT((this_spi == &g_mss_spi0) || (this_spi == &g_mss_spi1)); */
  
/*     /\* Handle receive. *\/ */
/*     if(this_spi->hw_reg->MIS & RXDONE_IRQ_MASK) */
/*     { */
/*         read_slave_rx_fifo(this_spi); */
/*         this_spi->hw_reg->IRQ_CLEAR = RXDONE_IRQ_MASK; */
/*     } */
    
/*     /\* Handle transmit. *\/ */
/*     tx_done = this_spi->hw_reg->MIS & TXDONE_IRQ_MASK; */
/*     if(tx_done) */
/*     { */
/*         if(0u == this_spi->slave_tx_buffer) */
/*         { */
/*             /\* Reload slave tx frame into Tx data register. *\/ */
/*             this_spi->hw_reg->TX_DATA = this_spi->slave_tx_frame; */
/*         } */
/*         this_spi->hw_reg->IRQ_CLEAR = TXDONE_IRQ_MASK; */
/*     } */
    
/*     /\* Handle command interrupt. *\/ */
/*     if(this_spi->hw_reg->MIS & CMD_IRQ_MASK) */
/*     { */
/*         read_slave_rx_fifo(this_spi); */
        
/*         /\* */
/*          * Call the command handler if one exists. */
/*          *\/ */
/*         if(this_spi->cmd_handler != 0u) */
/*         { */
/*             (*this_spi->cmd_handler)(this_spi->slave_rx_buffer, this_spi->slave_rx_idx); */
/*         } */
        
/*         this_spi->hw_reg->IRQ_CLEAR = CMD_IRQ_MASK; */
/*     } */
    
/*     /\* Handle slave select becoming de-asserted. *\/ */
/*     if(this_spi->hw_reg->MIS & SSEND_IRQ_MASK) */
/*     { */
/*         uint32_t rx_size; */
        
/*         read_slave_rx_fifo(this_spi); */
/*         rx_size = this_spi->slave_rx_idx; */
        
/*         /\*  */
/*          * Reset the transmit index to 0 to restart transmit at the start of the */
/*          * transmit buffer in the next transaction. This also requires flushing */
/*          * the Tx FIFO and refilling it with the start of Tx data buffer. */
/*          *\/ */
/*         this_spi->slave_tx_idx = 0u; */
/*         this_spi->hw_reg->COMMAND |= TX_FIFO_RESET_MASK; */
/*         fill_slave_tx_fifo(this_spi); */
        
/*         /\* Prepare to receive next packet. *\/ */
/*         this_spi->slave_rx_idx = 0u; */
        
/*         /\* */
/*          * Call the receive handler if one exists. */
/*          *\/ */
/*         if(this_spi->block_rx_handler != 0u) */
/*         { */
/*             (*this_spi->block_rx_handler)(this_spi->slave_rx_buffer, rx_size); */
/*         } */
        
/*         this_spi->hw_reg->IRQ_CLEAR = SSEND_IRQ_MASK; */
/*     } */
/* } */


/***************************************************************************//**
 * SPIO interrupt service routine.
 * Please note that the name of this ISR is defined as part of the SmartFusion
 * CMSIS startup code.
 */
#if defined(__GNUC__)
__attribute__((__interrupt__)) void SPI0_IRQHandler(void)
#else
void SPI0_IRQHandler( void )
#endif
{
    /* mss_spi_isr(&g_mss_spi0); */
    /* NVIC_ClearPendingIRQ(SPI0_IRQn); */
}

/***************************************************************************//**
 * SPI1 interrupt service routine.
 * Please note that the name of this ISR is defined as part of the SmartFusion
 * CMSIS startup code.
 */
#if defined(__GNUC__)
__attribute__((__interrupt__)) void SPI1_IRQHandler(void)
#else
void SPI1_IRQHandler(void)
#endif
{
    /* mss_spi_isr(&g_mss_spi1); */
    /* NVIC_ClearPendingIRQ(SPI1_IRQn); */
}

#ifdef __cplusplus
}
#endif

