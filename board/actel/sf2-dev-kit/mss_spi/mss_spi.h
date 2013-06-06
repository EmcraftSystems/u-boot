/***************************************************************************//**
 * (c) Copyright 2008 Actel Corporation.  All rights reserved.
 * 
 * SmartFusion microcontroller subsystem SPI bare metal software driver public API.
 *
 * The microcontroller subsystem SPI driver provides functions for implementing
 * SPI master or SPI slave operations. These operations can be one of two
 * classes: SPI frame operation or block transfer operations.
 * Frame operations allow transferring SPI frames from 4 to 32 bits long. Block
 * operations allow transferring blocks of data organized as 8 bit bytes. 
 *
 * SVN $Revision: 4566 $
 * SVN $Date: 2012-08-23 17:12:11 +0100 (Thu, 23 Aug 2012) $
 */
#define SPI0_BASE               0x40001000u
#define SPI1_BASE               0x40011000u
#define SYSREG_BASE             0x40038000u

#define SPI0_IRQn 0
#define SPI1_IRQn 0


#define SYSREG                  ((SYSREG_TypeDef *) SYSREG_BASE)

/*=========================================================================*//**
  @mainpage SmartFusion MSS SPI Bare Metal Driver.

  @section intro_sec Introduction
  The SmartFusion™ microcontroller subsystem (MSS) includes two serial
  peripheral interface SPI peripherals for serial communication. This driver
  provides a set of functions for controlling the MSS SPIs as part of a bare
  metal system where no operating system is available. These drivers can be
  adapted for use as part of an operating system, but the implementation of the
  adaptation layer between this driver and the operating system's driver model
  is outside the scope of this driver.
    
  @section hw_dependencies Hardware Flow Dependencies
  The configuration of all features of the MSS SPIs is covered by this driver
  with the exception of the SmartFusion IOMUX configuration. SmartFusion allows
  multiple non-concurrent uses of some external pins through IOMUX configuration.
  This feature allows optimization of external pin usage by assigning external
  pins for use by either the microcontroller subsystem or the FPGA fabric. The
  MSS SPIs serial signals are routed through IOMUXes to the SmartFusion device
  external pins. These IOMUXes are automatically configured correctly by the MSS
  configurator tool in the hardware flow when the MSS SPIs are enabled in that
  tool. You must ensure that the MSS SPIs are enabled by the MSS configurator
  tool in the hardware flow; otherwise the serial inputs and outputs will not be
  connected to the chip's external pins. For more information on IOMUX, refer to
  the IOMUX section of the SmartFusion Datasheet.
  The base address, register addresses and interrupt number assignment for the
  MSS SPI blocks are defined as constants in the SmartFusion CMSIS-PAL. You must
  ensure that the SmartFusion CMSIS-PAL is either included in the software tool
  chain used to build your project or is included in your project.
  
  @section theory_op Theory of Operation
  The MSS SPI driver functions are grouped into the following categories:
    • Initialization
    • Configuration for either master or slave operations
    • SPI master frame transfer control
    • SPI master block transfer control
    • SPI slave frame transfer control
    • SPI slave block transfer control
    • DMA block transfer
  Frame transfers allow the MSS SPI to write or read up to 32 bits of data in a
  SPI transaction. For example, a frame transfer of 12 bits might be used to
  read the result of an ADC conversion from a SPI analog to digital converter.
  Block transfers allow the MSS SPI to write or read a number of bytes in a SPI
  transaction. Block transfer transactions allow data transfers in multiples of
  8 bits (8, 16, 24, 32, 40…). Block transfers are typically used with byte
  oriented devices like SPI FLASH devices.

  Initialization 
    The MSS SPI driver is initialized through a call to the MSS_SPI_init()
    function. The MSS_SPI_init() function takes only one parameter, a pointer
    to one of two global data structures used by the driver to store state
    information for each MSS SPI. A pointer to these data structures is also
    used as first parameter to any of the driver functions to identify which MSS
    SPI will be used by the called function. The names of these two data
    structures are g_mss_spi0 and g_mss_spi1. Therefore any call to an MSS SPI
    driver function should be of the form MSS_SPI_function_name( &g_mss_spi0, ... )
    or MSS_SPI_function_name( &g_mss_spi1, ... ).
    The MSS_SPI_init() function resets the specified MSS SPI hardware block and
    clears any pending interrupts from that MSS SPI in the Cortex-M3 NVIC.
    The MSS_SPI_init() function must be called before any other MSS SPI driver
    functions can be called.

  Configuration
    A MSS SPI block can operate either as a master or slave SPI device. There
    are two distinct functions for configuring a MSS SPI block for master or
    slave operations.

    Master configuration
    The MSS_SPI_configure_master_mode() function configures the specified MSS
    SPI block for operations as a SPI master. It must be called once for each
    remote SPI slave device the MSS SPI block will communicate with. It is used
    to provide the following information about each SPI slave’s communication
    characteristics:
        • The SPI protocol mode
        • The SPI clock speed
        • The frame bit length
    This information is held by the driver and will be used to alter the
    configuration of the MSS SPI block each time a slave is selected through a
    call to MSS_SPI_set_slave_select(). The SPI protocol mode defines the
    initial state of the clock signal at the start of a transaction and which
    clock edge will be used to sample the data signal, or it defines whether the
    SPI block will operate in TI synchronous serial mode or in NSC MICROWIRE mode.

    Slave configuration
    The MSS_SPI_configure_slave_mode() function configures the specified MSS SPI
    block for operations as a SPI slave. It configures the following SPI
    communication characteristics:
        • The SPI protocol mode 
        • The SPI clock speed
        • The frame bit length
    The SPI protocol mode defines the initial state of the clock signal at the
    start of a transaction and which clock edge will be used to sample the data
    signal, or it defines whether the SPI block will operate in TI synchronous
    serial mode or in NSC MICROWIRE mode.

  SPI master frame transfer control
    The following functions are used as part of SPI master frame transfers:
        • MSS_SPI_set_slave_select()
        • MSS_SPI_transfer_frame()
        • MSS_SPI_clear_slave_select()
    The master must first select the target slave through a call to
    MSS_SPI_set_slave_select(). This causes the relevant slave select line to
    become asserted while data is clocked out onto the SPI data line.
    A call to is then made to function MSS_SPI_transfer_frame() specifying and
    the value of the data frame to be sent.
    The function MSS_SPI_clear_slave_select() can be used after the transfer is
    complete to prevent this slave select line from being asserted during
    subsequent SPI transactions. A call to this function is only required if the
    master is communicating with multiple slave devices.

  SPI master block transfer control
    The following functions are used as part of SPI master block transfers:
        • MSS_SPI_set_slave_select()
        • MSS_SPI_clear_slave_select()
        • MSS_SPI_transfer_block()
    The master must first select the target slave through a call to
    MSS_SPI_set_slave_select(). This causes the relevant slave select line to
    become asserted while data is clocked out onto the SPI data line.
    Alternatively a GPIO can be used to control the state of the target slave
    device’s chip select signal.
    A call to is then made to function MSS_SPI_transfer_block (). The
    parameters of this function specify:
        • the number of bytes to be transmitted
        • a pointer to the buffer containing the data to be transmitted
        • the number of bytes to be received
        • a pointer to the buffer where received data will be stored
    The number of bytes to be transmitted can be set to zero to indicate that
    the transfer is purely a block read transfer. The number of bytes to be
    received can be set to zero to specify that the transfer is purely a block
    write transfer.
    The function MSS_SPI_clear_slave_select() can be used after the transfer is
    complete to prevent this slave select line from being asserted during
    subsequent SPI transactions. A call to this function is only required if the
    master is communicating with multiple slave devices.
 
  SPI slave frame transfer control
    The following functions are used as part of SPI slave frame transfers:
        • MSS_SPI_set_slave_tx_frame()
        • MSS_SPI_set_frame_rx_handler()
    The MSS_SPI_set_slave_tx_frame() function specifies the frame data that will
    be returned to the SPI master. The frame data specified through this
    function is the value that will be read over the SPI bus by the remote SPI
    master when it initiates a transaction. A call to MSS_SPI_set_slave_tx_frame()
    is only required if the MSS SPI slave is the target of SPI  read transactions,
    i.e. if data is meant to be read from the SmartFusion device over SPI.
    The MSS_SPI_set_frame_rx_handler() function specifies the receive handler
    function that will called when a frame of data has been received by the MSS
    SPI when it is configured as a slave. The receive handler function specified
    through this call will process the frame data written, over the SPI bus, to
    the MSS SPI slave by the remote SPI master. The receive handler function must
    be implemented as part of the application. It is only required if the MSS SPI
    slave is the target of SPI frame write transactions.

  SPI slave block transfer control
    The following functions are used as part of SPI slave block transfers:
        • MSS_SPI_set_slave_block_buffers()
    The MSS_SPI_set_slave_block_buffers() function is used to configure a MSS SPI
    slave for block transfer operations. It specifies:
        • The buffer containing the data that will be returned to the remote SPI master
        • The buffer where data received from the remote SPI master will be stored
        • The handler function that will be called after the receive buffer is filled

  DMA block transfer control
    The following functions are used as part of MSS SPI DMA transfers:
        • MSS_SPI_disable()
        • MSS_SPI_set_transfer_byte_count()
        • MSS_SPI_enable()
        • MSS_SPI_tx_done()
    The MSS SPI must first be disabled through a call to function MSS_SPI_disable().
    The number of bytes to be transferred is then set through a call to function
    MSS_SPI_set_transfer_byte_count(). The DMA transfer is then initiated by a call
    to the MSS_PDMA_start() function provided by the MSS PDMA driver. The actual
    DMA transfer will only start once the MSS SPI block has been re-enabled through
    a call to  MSS_SPI_enable(). The completion of the DMA driven SPI transfer can
    be detected through a call to MSS_SPI_tx_done(). The direction of the SPI
    transfer, write or read, depends on the DMA channel configuration. A SPI write
    transfer occurs when the DMA channel is configured to write data to the MSS SPI
    block. A SPI read transfer occurs when the DMA channel is configured to read data
    from the MSS SPI block.

 *//*=========================================================================*/
#ifndef MSS_SPI_H_
#define MSS_SPI_H_

/* #include "../../CMSIS/m2sxxx.h" */

#ifdef __cplusplus
extern "C" {
#endif 

/***************************************************************************//**
  This defines the function prototype that must be followed by MSS SPI slave
  frame receive handler functions. These functions are registered with the MSS
  SPI driver through the MSS_SPI_set_frame_rx_handler () function.
  
  Declaring and Implementing Slave Frame Receive Handler Functions:
    Slave frame receive handler functions should follow the following prototype:
        void slave_frame_receive_handler ( uint32_t rx_frame );
    The actual name of the receive handler is unimportant. You can use any name
    of your choice for the receive frame handler. The rx_frame parameter will
    contain the value of the received frame.
 */
typedef void (*mss_spi_frame_rx_handler_t)( uint32_t rx_frame );

/***************************************************************************//**
  This defines the function prototype that must be followed by MSS SPI slave
  block receive handler functions. These functions are registered with the MSS
  SPI driver through the MSS_SPI_set_slave_block_buffers() function.
  
  Declaring and Implementing Slave Block Receive Handler Functions
    Slave block receive handler functions should follow the following prototype:
        void mss_spi_block_rx_handler ( uint8_t * rx_buff, uint16_t rx_size );
    The actual name of the receive handler is unimportant. You can use any name
    of your choice for the receive frame handler. The rx_buff parameter will
    contain a pointer to the start of the received block. The rx_size parameter
    will contain the number of bytes of the received block.

 */
typedef void (*mss_spi_block_rx_handler_t)( uint8_t * rx_buff, uint32_t rx_size );

/***************************************************************************//**
  This enumeration is used to define the settings for the SPI protocol mode
  bits, CPHA and CPOL. It is used as a parameter to the MSS_SPI_configure_master_mode()
  and MSS_SPI_configure_slave_mode() functions.
  
  - MSS_SPI_MODE0:
        Clock starts low, data read on clock's rising edge, data changes on
        falling edge.
        
  - MSS_SPI_MODE1:
        Clock starts low, data read on clock's falling edge, data changes on
        rising edge.
        
  - MSS_SPI_MODE2:
        Clock starts high, data read on clock's falling edge, data changes on
        rising edge.
        
  - MSS_SPI_MODE3:
        Clock starts high, data read on clock's rising edge, data changes on
        falling edge.
        
  - MSS_TI_MODE:  
        TI syncronous serial mode. Slave select is pulsed at start of transfer.
        
  - MSS_NSC_MODE:
        NSC Microwire mode.
 */
typedef enum __mss_spi_protocol_mode_t
{
    MSS_SPI_MODE0    = 0x00000000,
    MSS_SPI_TI_MODE  = 0x01000004,
    MSS_SPI_NSC_MODE = 0x00000008,
    MSS_SPI_MODE2    = 0x01000000,
    MSS_SPI_MODE1    = 0x02000000,
    MSS_SPI_MODE3    = 0x03000000
} mss_spi_protocol_mode_t;

/***************************************************************************//**
 This enumeration specifies the divider to be applied to the the APB bus clock
 in order to generate the SPI clock. It is used as parameter to the
 MSS_SPI_configure_master_mode() and MSS_SPI_configure_slave_mode()functions.
 */
 typedef enum __mss_spi_pclk_div_t
 {
    MSS_SPI_PCLK_DIV_2      = 0,
    MSS_SPI_PCLK_DIV_4      = 1,
    MSS_SPI_PCLK_DIV_8      = 2,
    MSS_SPI_PCLK_DIV_16     = 3,
    MSS_SPI_PCLK_DIV_32     = 4,
    MSS_SPI_PCLK_DIV_64     = 5,
    MSS_SPI_PCLK_DIV_128    = 6,
    MSS_SPI_PCLK_DIV_256    = 7
} mss_spi_pclk_div_t;

/***************************************************************************//**
 This enumeration is used to select a specific SPI slave device (0 to 7). It is
 used as a parameter to the MSS_SPI_configure_master_mode(),
 MSS_SPI_set_slave_select() and MSS_SPI_clear_slave_select () functions.
 */
 typedef enum __mss_spi_slave_t
 {
    MSS_SPI_SLAVE_0             = 0,
    MSS_SPI_SLAVE_1             = 1,
    MSS_SPI_SLAVE_2             = 2,
    MSS_SPI_SLAVE_3             = 3,
    MSS_SPI_SLAVE_4             = 4,
    MSS_SPI_SLAVE_5             = 5,
    MSS_SPI_SLAVE_6             = 6,
    MSS_SPI_SLAVE_7             = 7,
    MSS_SPI_MAX_NB_OF_SLAVES    = 8
} mss_spi_slave_t;

/***************************************************************************//**
  This constant defines a frame size of 8 bits when configuring an MSS SPI to
  perform block transfer data transactions.
  It must be used as the value for the frame_bit_length parameter of function
  MSS_SPI_configure_master_mode() when performing block transfers between the
  MSS SPI master and the target SPI slave.
  It must also be used as the value for the frame_bit_length parameter of
  function MSS_SPI_configure_slave_mode() when performing block transfers
  between the MSS SPI slave and the remote SPI master.
 */
#define MSS_SPI_BLOCK_TRANSFER_FRAME_SIZE   8

/***************************************************************************//**
  The mss_spi_slave_cfg_t holds the MSS SPI configuration that must be used to
  communicate with a specific SPI slave.
 */
typedef struct __mss_spi_slave_cfg_t
{
    uint32_t ctrl_reg;
    uint8_t txrxdf_size_reg;
    uint8_t clk_gen;
} mss_spi_slave_cfg_t;

#define __IO
#define __I
#define __O

/***************************************************************************//**
  
 */
typedef struct
{
    __IO uint32_t CONTROL;
    __IO uint32_t TXRXDF_SIZE;
    __I  uint32_t STATUS;
    __O  uint32_t IRQ_CLEAR;
    __I  uint32_t RX_DATA;
    __O  uint32_t TX_DATA;
    __IO uint32_t CLK_GEN;
    __IO uint32_t SLAVE_SELECT;
    __I  uint32_t MIS;
    __I  uint32_t RIS;
    __IO uint32_t CONTROL2;
    __IO uint32_t COMMAND;
    __IO uint32_t _PKTSIZE;
    __IO uint32_t CMDSIZE;
    __IO uint32_t HWSTATUS;
    __IO uint32_t STAT8;
    __IO uint32_t CTRL0;
    __IO uint32_t CTRL1;
    __IO uint32_t CTRL2;
    __IO uint32_t CTRL3;
} SPI_REVB_TypeDef;


typedef struct
{
    __IO     uint32_t    ESRAM_CR;                                 /*0X0   */
    __IO     uint32_t    ESRAM_MAX_LAT_CR;                         /*0X4   */
    __IO     uint32_t    DDR_CR;                                   /*0X8   */
    __IO     uint32_t    ENVM_CR;                                  /*0XC   */
    __IO     uint32_t    ENVM_REMAP_BASE_CR;                       /*0X10  */
    __IO     uint32_t    ENVM_REMAP_FAB_CR;                        /*0X14  */
    __IO     uint32_t    CC_CR;                                    /*0X18  */
    __IO     uint32_t    CC_REGION_CR;                             /*0X1C  */
    __IO     uint32_t    CC_LOCK_BASE_ADDR_CR;                     /*0X20  */
    __IO     uint32_t    CC_FLUSH_INDX_CR;                         /*0X24  */
    __IO     uint32_t    DDRB_BUF_TIMER_CR;                        /*0X28  */
    __IO     uint32_t    DDRB_NB_ADDR_CR;                          /*0X2C  */
    __IO     uint32_t    DDRB_NB_SIZE_CR;                          /*0X30  */
    __IO     uint32_t    DDRB_CR;                                  /*0X34  */
    __IO     uint32_t    EDAC_CR;                                  /*0X38  */
    __IO     uint32_t    MASTER_WEIGHT0_CR;                        /*0X3C  */
    __IO     uint32_t    MASTER_WEIGHT1_CR;                        /*0X40  */
    __IO     uint32_t    SOFT_IRQ_CR;                              /*0X44  */
    __IO     uint32_t    SOFT_RST_CR;                              /*0X48  */
    __IO     uint32_t    M3_CR;                                    /*0X4C  */
    __IO     uint32_t    FAB_IF_CR;                                /*0X50  */
    __IO     uint32_t    LOOPBACK_CR;                              /*0X54  */
    __IO     uint32_t    GPIO_SYSRESET_SEL_CR;                     /*0X58  */
    __IO     uint32_t    GPIN_SRC_SEL_CR;                          /*0X5C  */
    __IO     uint32_t    MDDR_CR;                                  /*0X60  */
    __IO     uint32_t    USB_IO_INPUT_SEL_CR;                      /*0X64  */
    __IO     uint32_t    PERIPH_CLK_MUX_SEL_CR;                    /*0X68  */
    __IO     uint32_t    WDOG_CR;                                  /*0X6C  */
    __IO     uint32_t    MDDR_IO_CALIB_CR;                         /*0X70  */
    __IO     uint32_t    SPARE_OUT_CR;                             /*0X74  */
    __IO     uint32_t    EDAC_IRQ_ENABLE_CR;                       /*0X78  */
    __IO     uint32_t    USB_CR;                                   /*0X7C  */
    __IO     uint32_t    ESRAM_PIPELINE_CR;                        /*0X80  */
    __IO     uint32_t    MSS_IRQ_ENABLE_CR;                        /*0X84  */
    __IO     uint32_t    RTC_WAKEUP_CR;                            /*0X88  */
    __IO     uint32_t    MAC_CR;                                   /*0X8C  */
    __IO     uint32_t    MSSDDR_PLL_STATUS_LOW_CR;                 /*0X90  */
    __IO     uint32_t    MSSDDR_PLL_STATUS_HIGH_CR;                /*0X94  */
    __IO     uint32_t    MSSDDR_FACC1_CR;                          /*0X98  */
    __IO     uint32_t    MSSDDR_FACC2_CR;                          /*0X9C  */
    __IO     uint32_t    PLL_LOCK_EN_CR;                           /*0XA0  */
    __IO     uint32_t    MSSDDR_CLK_CALIB_CR;                      /*0XA4  */
    __IO     uint32_t    PLL_DELAY_LINE_SEL_CR;                    /*0XA8  */
    __IO     uint32_t    MAC_STAT_CLRONRD_CR;                      /*0XAC  */
    __IO     uint32_t    RESET_SOURCE_CR;                          /*0XB0  */
    __I      uint32_t    CC_DC_ERR_ADDR_SR;                        /*0XB4  */
    __I      uint32_t    CC_IC_ERR_ADDR_SR;                        /*0XB8  */
    __I      uint32_t    CC_SB_ERR_ADDR_SR;                        /*0XBC  */
    __I      uint32_t    CC_DECC_ERR_ADDR_SR;                      /*0XC0  */
    __I      uint32_t    CC_IC_MISS_CNT_SR;                        /*0XC4  */
    __I      uint32_t    CC_IC_HIT_CNT_SR;                         /*0XC8  */
    __I      uint32_t    CC_DC_MISS_CNT_SR;                        /*0XCC  */
    __I      uint32_t    CC_DC_HIT_CNT_SR;                         /*0XD0  */
    __I      uint32_t    CC_IC_TRANS_CNT_SR;                       /*0XD4  */
    __I      uint32_t    CC_DC_TRANS_CNT_SR;                       /*0XD8  */
    __I      uint32_t    DDRB_DS_ERR_ADR_SR;                       /*0XDC  */
    __I      uint32_t    DDRB_HPD_ERR_ADR_SR;                      /*0XE0  */
    __I      uint32_t    DDRB_SW_ERR_ADR_SR;                       /*0XE4  */
    __I      uint32_t    DDRB_BUF_EMPTY_SR;                        /*0XE8  */
    __I      uint32_t    DDRB_DSBL_DN_SR;                          /*0XEC  */
    __I      uint32_t    ESRAM0_EDAC_CNT;                          /*0XF0  */
    __I      uint32_t    ESRAM1_EDAC_CNT;                          /*0XF4  */
    __I      uint32_t    CC_EDAC_CNT;                              /*0XF8  */
    __I      uint32_t    MAC_EDAC_TX_CNT;                          /*0XFC  */
    __I      uint32_t    MAC_EDAC_RX_CNT;                          /*0X100 */
    __I      uint32_t    USB_EDAC_CNT;                             /*0X104 */
    __I      uint32_t    CAN_EDAC_CNT;                             /*0X108 */
    __I      uint32_t    ESRAM0_EDAC_ADR;                          /*0X10C */
    __I      uint32_t    ESRAM1_EDAC_ADR;                          /*0X110 */
    __I      uint32_t    MAC_EDAC_RX_ADR;                          /*0X114 */
    __I      uint32_t    MAC_EDAC_TX_ADR;                          /*0X118 */
    __I      uint32_t    CAN_EDAC_ADR;                             /*0X11C */
    __I      uint32_t    USB_EDAC_ADR;                             /*0X120 */
    __I      uint32_t    MM0_1_2_SECURITY;                         /*0X124 */
    __I      uint32_t    MM4_5_FIC64_SECURITY;                     /*0X128 */
    __I      uint32_t    MM3_6_7_8_SECURITY;                       /*0X12C */
    __I      uint32_t    MM9_SECURITY;                             /*0X130 */
    __I      uint32_t    M3_SR;                                    /*0X134 */
    __I      uint32_t    ETM_COUNT_LOW;                            /*0X138 */
    __I      uint32_t    ETM_COUNT_HIGH;                           /*0X13C */
    __I      uint32_t    DEVICE_SR;                                /*0X140 */
    __I      uint32_t    ENVM_PROTECT_USER;                        /*0X144 */
    __I      uint32_t    G4C_ENVM_STATUS;                          /*0X148 */
    __I      uint32_t    DEVICE_VERSION;                           /*0X14C */
    __I      uint32_t    MSSDDR_PLL_STATUS;                        /*0X150 */
    __I      uint32_t    USB_SR;                                   /*0X154 */
    __I      uint32_t    ENVM_SR;                                  /*0X158 */
    __I      uint32_t    SPARE_IN;                                 /*0X15C */
    __I      uint32_t    DDRB_STATUS;                              /*0X160 */
    __I      uint32_t    MDDR_IO_CALIB_STATUS;                     /*0X164 */
    __I      uint32_t    MSSDDR_CLK_CALIB_STATUS;                  /*0X168 */
    __I      uint32_t    WDOGLOAD;                                 /*0X16C */
    __I      uint32_t    WDOGMVRP;                                 /*0X170 */
    __I      uint32_t    USERCONFIG0;                              /*0X174 */
    __I      uint32_t    USERCONFIG1;                              /*0X178 */
    __I      uint32_t    USERCONFIG2;                              /*0X17C */
    __I      uint32_t    USERCONFIG3;                              /*0X180 */
    __I      uint32_t    FAB_PROT_SIZE;                            /*0X184 */
    __I      uint32_t    FAB_PROT_BASE;                            /*0X188 */
    __I      uint32_t    MSS_GPIO_DEF;                             /*0X18C */
    __IO     uint32_t    EDAC_SR;                                  /*0X190 */
    __IO     uint32_t    MSS_INTERNAL_SR;                          /*0X194 */
    __IO     uint32_t    MSS_EXTERNAL_SR;                          /*0X198 */
    __IO     uint32_t    WDOGTIMEOUTEVENT;                         /*0X19C */
    __IO     uint32_t    CLR_MSS_COUNTERS;                         /*0X1A0 */
    __IO     uint32_t    CLR_EDAC_COUNTERS;                        /*0X1A4 */
    __IO     uint32_t    FLUSH_CR;                                 /*0X1A8 */
    __IO     uint32_t    MAC_STAT_CLR_CR;                          /*0X1AC */
    __IO     uint32_t    IOMUXCELL_CONFIG[57];                     /*0X1B0 */
    __I      uint32_t    NVM_PROTECT_FACTORY;                      /*0X294 */
    __I      uint32_t    DEVICE_STATUS_FIXED;                      /*0X298 */
    __I      uint32_t    MBIST_ES0;                                /*0X29C */
    __I      uint32_t    MBIST_ES1;                                /*0X2A0 */
    __IO     uint32_t    MSDDR_PLL_STAUS_1;                        /*0X2A4 */
    __I      uint32_t    REDUNDANCY_ESRAM0;                        /*0X2A8 */
    __I      uint32_t    REDUNDANCY_ESRAM1;                        /*0X2AC */
    __I      uint32_t    SERDESIF;                                 /*0X2B0 */

} SYSREG_TypeDef;

#define SYSREG_SPI0_SOFTRESET_MASK          ( (uint32_t)( 0x01 << 9) )
#define SYSREG_SPI1_SOFTRESET_MASK          ( (uint32_t)( 0x01 << 10) )


typedef int IRQn_Type;
#define ASSERT(x)

/***************************************************************************//**
  There is one instance of this structure for each of the microcontroller
  subsystem's SPIs. Instances of this structure are used to identify a specific
  SPI. A pointer to an instance of the mss_spi_instance_t structure is passed as
  the first parameter to MSS SPI driver functions to identify which SPI should
  perform the requested operation.
 */
typedef struct __mss_spi_instance_t
{
    /* CMSIS related defines identifying the SPI hardware. */
    SPI_REVB_TypeDef *          hw_reg;     /*!< Pointer to SPI registers. */
    IRQn_Type               irqn;           /*!< SPI's Cortex-M3 NVIC interrupt number. */
    
    /* Internal transmit state: */
    const uint8_t * slave_tx_buffer;    /*!< Pointer to slave transmit buffer. */
    uint32_t slave_tx_size;             /*!< Size of slave transmit buffer. */
    uint32_t slave_tx_idx;              /*!< Current index into slave transmit buffer. */
    
    /* Slave command response buffer: */
    const uint8_t * resp_tx_buffer;
    uint32_t resp_buff_size;
    uint32_t resp_buff_tx_idx;
    mss_spi_block_rx_handler_t cmd_handler;
    
    /* Internal receive state: */
    uint8_t * slave_rx_buffer;          /*!< Pointer to buffer where data received by a slave will be stored. */
    uint32_t slave_rx_size;             /*!< Slave receive buffer size. */
    uint32_t slave_rx_idx;              /*!< Current index into slave receive buffer. */
    
    /* Configuration for each target slave. */
    mss_spi_slave_cfg_t slaves_cfg[MSS_SPI_MAX_NB_OF_SLAVES];
    
    /** Slave received frame handler: */
    mss_spi_frame_rx_handler_t frame_rx_handler;    /*!< Pointer to function that will be called when a frame is received when the SPI block is configured as slave. */
    
    uint32_t slave_tx_frame;                /*!< Value of the data frame that will be transmited when the SPI block is configured as slave. */
    
    /* Slave block rx handler: */
    mss_spi_block_rx_handler_t block_rx_handler;    /*!< Pointer to the function that will be called when a data block has been received. */

} mss_spi_instance_t;

/***************************************************************************//**
  This instance of mss_spi_instance_t holds all data related to the operations
  performed by MSS SPI 0. A pointer to g_mss_spi0 is passed as the first
  parameter to MSS SPI driver functions to indicate that MSS SPI 0 should
  perform the requested operation.
 */
extern mss_spi_instance_t g_mss_spi0;

/***************************************************************************//**
  This instance of mss_spi_instance_t holds all data related to the operations
  performed by MSS SPI 1. A pointer to g_mss_spi1 is passed as the first
  parameter to MSS SPI driver functions to indicate that MSS SPI 1 should
  perform the requested operation.
 */
extern volatile mss_spi_instance_t g_mss_spi1;

/***************************************************************************//**
  The MSS_SPI_init() function initializes and hardware and data structures of
  one of the SmartFusion MSS SPIs. The MSS_SPI_init() function must be called
  before any other MSS SPI driver functions can be called.
  
  @param this_spi
    The this_spi parameter is a pointer to an mss_spi_instance_t structure
    identifying the MSS SPI hardware block to be initialized. There are two such
    data structures, g_mss_spi0 and g_mss_spi1, associated with MSS SPI 0 and
    MSS SPI 1 respectively. This parameter must point to either the g_mss_spi0
    or g_mss_spi1 global data structure defined within the SPI driver.
    
  Example:
  @code
  MSS_SPI_init( &g_mss_spi0 );
  @endcode
 */
void MSS_SPI_init
(
  volatile  mss_spi_instance_t * this_spi
);

/***************************************************************************//**
  The MSS_SPI_configure_slave_mode() function configure a MSS SPI block for
  operations as a slave SPI device. It configures the SPI hardware with the
  selected SPI protocol mode and clock speed.
    
  @param this_spi
    The this_spi parameter is a pointer to an mss_spi_instance_t structure
    identifying the MSS SPI hardware block to be initialized. There are two such
    data structures, g_mss_spi0 and g_mss_spi1, associated with MSS SPI 0 and
    MSS SPI 1 respectively. This parameter must point to either the g_mss_spi0
    or g_mss_spi1 global data structure defined within the SPI driver.
    
  @param protocol_mode
    Serial peripheral interface operating mode. Allowed values are:
        - MSS_SPI_MODE0
        - MSS_SPI_MODE1
        - MSS_SPI_MODE2
        - MSS_SPI_MODE3
        - MSS_TI_MODE
        - MSS_NSC_MODE
 
  @param clk_rate
    Divider value used to generate serial interface clock signal from PCLK.
    Allowed values are:
        - MSS_SPI_PCLK_DIV_2
        - MSS_SPI_PCLK_DIV_4
        - MSS_SPI_PCLK_DIV_8
        - MSS_SPI_PCLK_DIV_16
        - MSS_SPI_PCLK_DIV_32
        - MSS_SPI_PCLK_DIV_64
        - MSS_SPI_PCLK_DIV_128
        - MSS_SPI_PCLK_DIV_256
       
  @param frame_bit_length
    Number of bits making up the frame. The maximum frame length is 32 bits. You
    must use the MSS_SPI_BLOCK_TRANSFER_FRAME_SIZE constant as the value for
    frame_bit_length when configuring the MSS SPI master for block transfer
    transactions with the target SPI slave.
  
  Example:
  @code
  MSS_SPI_init( &g_mss_spi0 );
  MSS_SPI_configure_slave_mode
    (
        &g_mss_spi0,
        MSS_SPI_MODE2,
        MSS_SPI_PCLK_DIV_64,
        MSS_SPI_BLOCK_TRANSFER_FRAME_SIZE
    );
  @endcode
  
 */ 
void MSS_SPI_configure_slave_mode
(
    mss_spi_instance_t * this_spi,
    mss_spi_protocol_mode_t protocol_mode,
    mss_spi_pclk_div_t clk_rate,
    uint8_t frame_bit_length
);

/***************************************************************************//**
  The MSS_SPI_configure_master_mode() function configures the protocol mode,
  serial clock speed and frame size for a specific target SPI slave device. It
  is used when the MSS SPI hardware block is used as a SPI master. This function
  must be called once for each target SPI slave the SPI master is going to
  communicate with. The SPI master hardware will be configured with the
  configuration specified by this function during calls to
  MSS_SPI_set_slave_select().
  
  @param this_spi
    The this_spi parameter is a pointer to an mss_spi_instance_t structure
    identifying the MSS SPI hardware block to be initialized. There are two such
    data structures, g_mss_spi0 and g_mss_spi1, associated with MSS SPI 0 and
    MSS SPI 1 respectively. This parameter must point to either the g_mss_spi0
    or g_mss_spi1 global data structure defined within the SPI driver.
    
    
  @param slave
    The slave parameter is used to identify a target SPI slave. The driver will
    hold the MSS SPI master configuration required to communicate with this
    slave, as specified by the other function parameters. Allowed values are:
        • MSS_SPI_SLAVE_0
        • MSS_SPI_SLAVE_1
        • MSS_SPI_SLAVE_2
        • MSS_SPI_SLAVE_3
        • MSS_SPI_SLAVE_4
        • MSS_SPI_SLAVE_5
        • MSS_SPI_SLAVE_6
        • MSS_SPI_SLAVE_7
    
  @param protocol_mode
    Serial peripheral interface operating mode. Allowed values are:
        • MSS_SPI_MODE0
        • MSS_SPI_MODE1
        • MSS_SPI_MODE2
        • MSS_SPI_MODE3
        • MSS_SPI_TI_MODE
        • MSS_SPI_NSC_MODE
 
  @param clk_rate
    Divider value used to generate serial interface clock signal from PCLK.
    Allowed values are:
        • MSS_SPI_PCLK_DIV_2
        • MSS_SPI_PCLK_DIV_4
        • MSS_SPI_PCLK_DIV_8
        • MSS_SPI_PCLK_DIV_16
        • MSS_SPI_PCLK_DIV_32
        • MSS_SPI_PCLK_DIV_64
        • MSS_SPI_PCLK_DIV_128
        • MSS_SPI_PCLK_DIV_256
        
  @param frame_bit_length
    Number of bits making up the frame. The maximum frame length is 32 bits. You
    must use the MSS_SPI_BLOCK_TRANSFER_FRAME_SIZE constant as the value for
    frame_bit_length when configuring the MSS SPI master for block transfer
    transactions with the target SPI slave.
    
  Example:
  @code
  MSS_SPI_init( &g_mss_spi0 );

  MSS_SPI_configure_master_mode
    (
        &g_mss_spi0,
        MSS_SPI_SLAVE_0,
        MSS_SPI_MODE2,
        MSS_SPI_PCLK_DIV_64,
        12
     );

  MSS_SPI_configure_master_mode
    (
        &g_mss_spi0,
        MSS_SPI_SLAVE_1,
        MSS_SPI_TI_MODE,
        MSS_SPI_PCLK_DIV_128,
        MSS_SPI_BLOCK_TRANSFER_FRAME_SIZE
     );
  @endcode
 */ 
void MSS_SPI_configure_master_mode
(
  volatile  mss_spi_instance_t *    this_spi,
    mss_spi_slave_t         slave,
    mss_spi_protocol_mode_t protocol_mode,
    mss_spi_pclk_div_t      clk_rate,
    uint8_t                 frame_bit_length
);

/*==============================================================================
 * Master functions
 *============================================================================*/

/***************************************************************************//**
  The MSS_SPI_slave_select() function is used by a MSS SPI master to select a
  specific slave. This function causes the relevant slave select signal to be
  asserted.
 
  @param this_spi
    The this_spi parameter is a pointer to an mss_spi_instance_t structure
    identifying the MSS SPI hardware block to be initialized. There are two such
    data structures, g_mss_spi0 and g_mss_spi1, associated with MSS SPI 0 and
    MSS SPI 1 respectively. This parameter must point to either the g_mss_spi0
    or g_mss_spi1 global data structure defined within the SPI driver.
    
 
  @param slave
    The slave parameter is one of mss_spi_slave_t enumerated constants
    identifying a slave.
  
  Example:
  @code
  const uint8_t frame_size = 25;
  const uint32_t master_tx_frame = 0x0100A0E1;

  MSS_SPI_init( &g_mss_spi0 );
  MSS_SPI_configure_master_mode
    (
        &g_mss_spi0,
        MSS_SPI_SLAVE_0,
        MSS_SPI_MODE1,
        MSS_SPI_PCLK_DIV_256,
        frame_size
     );

  MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
  MSS_SPI_transfer_frame( &g_mss_spi0, master_tx_frame );
  MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
  @endcode
 */
void MSS_SPI_set_slave_select
(
 volatile   mss_spi_instance_t * this_spi,
    mss_spi_slave_t slave
);

/***************************************************************************//**
  The MSS_SPI_clear_slave_select() function is used by a MSS SPI Master to
  deselect a specific slave. This function causes the relevant slave select
  signal to be de-asserted.
 
  @param this_spi
    The this_spi parameter is a pointer to an mss_spi_instance_t structure
    identifying the MSS SPI hardware block to be initialized. There are two such
    data structures, g_mss_spi0 and g_mss_spi1, associated with MSS SPI 0 and
    MSS SPI 1 respectively. This parameter must point to either the g_mss_spi0
    or g_mss_spi1 global data structure defined within the SPI driver.
    
 
  @param slave
    The slave parameter is one of mss_spi_slave_t enumerated constants
    identifying a slave.
  
  Example:
  @code
  const uint8_t frame_size = 25;
  const uint32_t master_tx_frame = 0x0100A0E1;

  MSS_SPI_init( &g_mss_spi0 );
  MSS_SPI_configure_master_mode
    (
        &g_mss_spi0,
        MSS_SPI_SLAVE_0,
        MSS_SPI_MODE1,
        MSS_SPI_PCLK_DIV_256,
        frame_size
     );
  MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
  MSS_SPI_transfer_frame( &g_mss_spi0, master_tx_frame );
  MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
  @endcode
 */
void MSS_SPI_clear_slave_select
(
    mss_spi_instance_t * this_spi,
    mss_spi_slave_t slave
);

/***************************************************************************//**
  The MSS_SPI_disable() function is used to temporarily disable a MSS SPI
  hardware block. This function is typically used in conjunction with the
  SPI_set_transfer_byte_count() function to setup a DMA controlled SPI transmit
  transaction as the SPI_set_transfer_byte_count() function must only be used
  when the MSS SPI hardware is disabled.
 
  @param this_spi
    The this_spi parameter is a pointer to an mss_spi_instance_t structure
    identifying the MSS SPI hardware block to be initialized. There are two such
    data structures, g_mss_spi0 and g_mss_spi1, associated with MSS SPI 0 and
    MSS SPI 1 respectively. This parameter must point to either the g_mss_spi0
    or g_mss_spi1 global data structure defined within the SPI driver.
  
  Example:
  @code
  uint32_t transfer_size;
  uint8_t tx_buffer[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
  
  transfer_size = sizeof(tx_buffer);
  
  MSS_SPI_disable( &g_mss_spi0 );
  MSS_SPI_set_transfer_byte_count( &g_mss_spi0, transfer_size );
  PDMA_start
    (
        PDMA_CHANNEL_0,
        (uint32_t)tx_buffer,
        PDMA_SPI1_TX_REGISTER,
        transfer_size
    );
  MSS_SPI_enable( &g_mss_spi0 );
  
  while ( !MSS_SPI_tx_done( &g_mss_spi0 ) )
  {
      ;
  }
  @endcode
 */
void MSS_SPI_disable
(
  volatile    mss_spi_instance_t * this_spi
);

/***************************************************************************//**
  The MSS_SPI_enable() function is used to re-enable a MSS SPI hardware block
  after it was disabled using the SPI_disable() function.
 
  @param this_spi
    The this_spi parameter is a pointer to an mss_spi_instance_t structure
    identifying the MSS SPI hardware block to be initialized. There are two such
    data structures, g_mss_spi0 and g_mss_spi1, associated with MSS SPI 0 and
    MSS SPI 1 respectively. This parameter must point to either the g_mss_spi0
    or g_mss_spi1 global data structure defined within the SPI driver.
    
  Example:
  @code
  uint32_t transfer_size;
  uint8_t tx_buffer[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
  
  transfer_size = sizeof(tx_buffer);
  
  MSS_SPI_disable( &g_mss_spi0 );
  MSS_SPI_set_transfer_byte_count( &g_mss_spi0, transfer_size );
  PDMA_start
    (
        PDMA_CHANNEL_0,
        (uint32_t)tx_buffer,
        PDMA_SPI1_TX_REGISTER,
        transfer_size
    );
  MSS_SPI_enable( &g_mss_spi0 );
  
  while ( !MSS_SPI_tx_done( &g_mss_spi0 ) )
  {
      ;
  }
  @endcode
 */
void MSS_SPI_enable
(
 volatile    mss_spi_instance_t * this_spi
);

/***************************************************************************//**
  The MSS_SPI_set_transfer_byte_count() function is used as part of setting up
  a SPI transfer using DMA. It specifies the number of bytes that must be
  transferred before MSS_SPI_tx_done() indicates that the transfer is complete.
 
  @param this_spi
    The this_spi parameter is a pointer to an mss_spi_instance_t structure
    identifying the MSS SPI hardware block to be initialized. There are two such
    data structures, g_mss_spi0 and g_mss_spi1, associated with MSS SPI 0 and
    MSS SPI 1 respectively. This parameter must point to either the g_mss_spi0
    or g_mss_spi1 global data structure defined within the SPI driver.
    
 
  @param byte_count
    The byte_count parameter specifies the number of bytes that must be
    transferred by the SPI hardware block considering that a transaction has
    been completed.
  
  Example:
  @code
  uint32_t transfer_size;
  uint8_t tx_buffer[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
  
  transfer_size = sizeof(tx_buffer);
  
  MSS_SPI_disable( &g_mss_spi0 );
  
  MSS_SPI_set_transfer_byte_count( &g_mss_spi0, transfer_size );
  
  PDMA_start( PDMA_CHANNEL_0, (uint32_t)tx_buffer, 0x40011014, transfer_size );
  
  MSS_SPI_enable( &g_mss_spi0 );
  
  while ( !MSS_SPI_tx_done( &g_mss_spi0) )
  {
      ;
  }
  @endcode
 */
void MSS_SPI_set_transfer_byte_count
(
    mss_spi_instance_t * this_spi,
    uint16_t byte_count
);

/***************************************************************************//**
  The MSS_SPI_tx_done() function is used to find out if a DMA controlled transfer
  has completed.
 
  @param this_spi
    The this_spi parameter is a pointer to an mss_spi_instance_t structure
    identifying the MSS SPI hardware block to be initialized. There are two such
    data structures, g_mss_spi0 and g_mss_spi1, associated with MSS SPI 0 and
    MSS SPI 1 respectively. This parameter must point to either the g_mss_spi0
    or g_mss_spi1 global data structure defined within the SPI driver.
  
  Example:
  @code
      uint32_t transfer_size;
      uint8_t tx_buffer[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
      
      transfer_size = sizeof(tx_buffer);
      
      MSS_SPI_disable( &g_mss_spi0 );
      
      MSS_SPI_set_transfer_byte_count( &g_mss_spi0, transfer_size );
      
      PDMA_start
        (
            PDMA_CHANNEL_0,
            (uint32_t)tx_buffer,
            PDMA_SPI1_TX_REGISTER,
            transfer_size
        );
      
      MSS_SPI_enable( &g_mss_spi0 );
      
      while ( !MSS_SPI_tx_done(&g_mss_spi0) )
      {
          ;
      }
  @endcode
 */
uint32_t MSS_SPI_tx_done
(
    mss_spi_instance_t * this_spi
);

/***************************************************************************//**
  The MSS_SPI_transfer_frame() function is used by a MSS SPI master to transmit
  and receive a frame up to 32 bits long. This function is typically used for
  transactions with a SPI slave where the number of transmit and receive bits is
  not divisible by 8.
 
  @param this_spi
    The this_spi parameter is a pointer to an mss_spi_instance_t structure
    identifying the MSS SPI hardware block to be initialized. There are two such
    data structures, g_mss_spi0 and g_mss_spi1, associated with MSS SPI 0 and
    MSS SPI 1 respectively. This parameter must point to either the g_mss_spi0
    or g_mss_spi1 global data structure defined within the SPI driver.
    
 
  @param tx_bits
    The tx_bits parameter is a 32 bits word containing the value that will be
    transmitted.
    Note:   The bit length of the value to be transmitted to the slave must be
            specified as the frame_bit_length parameter in a previous call to
            the MSS_SPI_configure_master() function.

  @return
    This function returns a 32 bits word containing the value that is received
    from the slave.
 
  Example:
  @code
      const uint8_t frame_size = 25;
      const uint32_t master_tx_frame = 0x0100A0E1;
      uint32_t master_rx;
      
      MSS_SPI_init( &g_mss_spi0 );
      MSS_SPI_configure_master_mode
        (
            &g_mss_spi0,
            MSS_SPI_SLAVE_0,
            MSS_SPI_MODE1,
            MSS_SPI_PCLK_DIV_256,
            frame_size
         );
     
      MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
      master_rx = MSS_SPI_transfer_frame( &g_mss_spi0, master_tx_frame );
      MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
  @endcode
 */
uint32_t MSS_SPI_transfer_frame
(
 volatile mss_spi_instance_t * this_spi,
    uint32_t tx_bits
);

/***************************************************************************//**
  The MSS_SPI_transfer_block() function is used by MSS SPI masters to transmit
  and receive blocks of data organized as a specified number of bytes. It can be
  used for:
    • Writing a data block to a slave
    • Reading a data block from a slave
    • Sending a command to a slave followed by reading the outcome of the
      command in a single SPI transaction. This function can be used alongside
      Peripheral DMA functions to perform the actual moving to and from the SPI
      hardware block using Peripheral DMA.
 
  @param this_spi
    The this_spi parameter is a pointer to an mss_spi_instance_t structure
    identifying the MSS SPI hardware block to be initialized. There are two such
    data structures, g_mss_spi0 and g_mss_spi1, associated with MSS SPI 0 and
    MSS SPI 1 respectively. This parameter must point to either the g_mss_spi0
    or g_mss_spi1 global data structure defined within the SPI driver.
    
 
  @param cmd_buffer
    The cmd_buffer parameter is a pointer to the buffer containing the data that
    will be sent by the master from the beginning of the transfer. This pointer
    can be null (0) if the master does not need to send a command before reading
    data or if the command part of the transfer is written to the SPI hardware
    block using DMA.
 
  @param cmd_byte_size
    The cmd_byte_size parameter specifies the number of bytes contained in
    cmd_buffer that will be sent. A value of 0 indicates that no data needs to
    be sent to the slave. A non-zero value while the cmd_buffer pointer is 0 is
    used to indicate that the command data will be written to the SPI hardware
    block using DMA.
 
  @param rd_buffer
    The rd_buffer parameter is a pointer to the buffer where the data received
    from the slave after the command has been sent will be stored.
 
  @param rd_byte_size
    The rd_byte_size parameter specifies the number of bytes to be received from
    the slave and stored in the rd_buffer. A value of 0 indicates that no data
    is to be read from the slave. A non-zero value while the rd_buffer pointer
    is null (0) is used to specify the receive size when using DMA to read from
    the slave. 
    Note:   All bytes received from the slave, including the bytes received
            while the command is sent, will be read through DMA.
  
  Polled write transfer example:
  @code
      uint8_t master_tx_buffer[MASTER_TX_BUFFER] = 
      {
          0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A
      };
      MSS_SPI_init( &g_mss_spi0 );
      MSS_SPI_configure_master_mode
        (
            &g_mss_spi0,
            MSS_SPI_SLAVE_0,
            MSS_SPI_MODE1,
            MSS_SPI_PCLK_DIV_256,
            MSS_SPI_BLOCK_TRANSFER_FRAME_SIZE
         );
     
      MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
      MSS_SPI_transfer_block
        (
            &g_mss_spi0,
            master_tx_buffer,
            sizeof(master_tx_buffer),
            0,
            0
        );
      MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
  @endcode
 
  DMA transfer example:
  In this example the transmit and receive buffers are not specified as part of
  the call to MSS_SPI_transfer_block(). MSS_SPI_transfer_block() will only
  prepare the MSS SPI hardware for a transfer. The MSS SPI transmit hardware
  FIFO is filled using one DMA channel and a second DMA channel is used to read
  the content of the MSS SPI receive hardware FIFO. The transmit and receive
  buffers are specified by two separate calls to PDMA_start() to initiate DMA
  transfers on the channel used for transmit data and the channel used for
  receive data. 
  @code
      uint8_t master_tx_buffer[MASTER_RX_BUFFER] =
      {
          0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA
      };
      uint8_t slave_rx_buffer[MASTER_RX_BUFFER] = 
      {
          0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A
      };
      MSS_SPI_init( &g_mss_spi0 );

      MSS_SPI_configure_master_mode
          (
            &g_mss_spi0,
            MSS_SPI_SLAVE_0,
            MSS_SPI_MODE1,
            MSS_SPI_PCLK_DIV_256,
            MSS_SPI_BLOCK_TRANSFER_FRAME_SIZE
         );
      MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
      MSS_SPI_transfer_block( &g_mss_spi0, 0, 0, 0, 0 );
      PDMA_start
        (
            PDMA_CHANNEL_1,
            PDMA_SPI0_RX_REGISTER,
            (uint32_t)master_rx_buffer,
            sizeof(master_rx_buffer)
        ); 
      PDMA_start
        (
            PDMA_CHANNEL_2,
            (uint32_t)master_tx_buffer,
            PDMA_SPI0_TX_REGISTER,
            sizeof(master_tx_buffer)
        ); 
      while( PDMA_status(PDMA_CHANNEL_1) == 0 )
      {
          ;
      }
      MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
  @endcode
 */
void MSS_SPI_transfer_block
(
    mss_spi_instance_t * this_spi,
    const uint8_t * cmd_buffer,
    uint16_t cmd_byte_size,
    uint8_t * rd_buffer,
    uint16_t rd_byte_size
);

/*==============================================================================
 * Slave functions
 *============================================================================*/

/***************************************************************************//**
  The MSS_SPI_set_frame_rx_handler() function is used by MSS SPI slaves to
  specify the receive handler function that will be called by the MSS SPI driver
  interrupt handler when a a frame of data is received by the MSS SPI slave.
  
  @param this_spi
    The this_spi parameter is a pointer to an mss_spi_instance_t structure
    identifying the MSS SPI hardware block to be initialized. There are two such
    data structures, g_mss_spi0 and g_mss_spi1, associated with MSS SPI 0 and
    MSS SPI 1 respectively. This parameter must point to either the g_mss_spi0
    or g_mss_spi1 global data structure defined within the SPI driver.
    
 
  @param rx_handler
    The rx_handler parameter is a pointer to the frame receive handler that must
    be called when a frame is received by the MSS SPI slave.
 
  Example:
  @code
      uint32_t g_slave_rx_frame = 0;

      void slave_frame_handler( uint32_t rx_frame )
      {
          g_slave_rx_frame = rx_frame;
      }

      int setup_slave( void )
      {
          const uint16_t frame_size = 25;
          MSS_SPI_init( &g_mss_spi1 );
          MSS_SPI_configure_slave_mode
            (
                &g_mss_spi0,
                MSS_SPI_MODE2,
                MSS_SPI_PCLK_DIV_64,
                frame_size
            );
          MSS_SPI_set_frame_rx_handler( &g_mss_spi1, slave_frame_handler );
      }
  @endcode
 */
void MSS_SPI_set_frame_rx_handler
(
    mss_spi_instance_t * this_spi,
    mss_spi_frame_rx_handler_t rx_handler
);

/***************************************************************************//**
  The MSS_SPI_set_slave_tx_frame() function is used by MSS SPI slaves to specify
  the frame that will be transmitted when a transaction is initiated by the SPI
  master.
  
  @param this_spi
    The this_spi parameter is a pointer to an mss_spi_instance_t structure
    identifying the MSS SPI hardware block to be initialized. There are two such
    data structures, g_mss_spi0 and g_mss_spi1, associated with MSS SPI 0 and
    MSS SPI 1 respectively. This parameter must point to either the g_mss_spi0
    or g_mss_spi1 global data structure defined within the SPI driver.
    
 
  @param frame_value
    The frame_value parameter contains the value of the frame to be sent to the
    master.
    Note:   The bit length of the value to be transmitted to the master must be
            specified as the frame_bit_length parameter in a previous call to
            the MSS_SPI_configure_slave() function.

  Example:
  @code
      const uint16_t frame_size = 25;
      const uint32_t slave_tx_frame = 0x0110F761;
      uint32_t master_rx;

      MSS_SPI_init( &g_mss_spi1 );
      MSS_SPI_configure_slave_mode
        (
            &g_mss_spi0,
            MSS_SPI_MODE2,
            MSS_SPI_PCLK_DIV_64,
            frame_size
        );
      MSS_SPI_set_slave_tx_frame( &g_mss_spi1, slave_tx_frame );
  @endcode
 */
void MSS_SPI_set_slave_tx_frame
(
    mss_spi_instance_t * this_spi,
    uint32_t frame_value
);

/***************************************************************************//**
  The MSS_SPI_set_slave_block_buffers() function is used to configure an MSS
  SPI slave for block transfer operations. It specifies one or more of the
  following:
    - The data that will be transmitted when accessed by a master.
    - The buffer where data received from a master will be stored.
    - The handler function that must be called after the receive buffer has been
      filled.
    - The number of bytes that must be received from the master before the receive
      handler function is called.
  These parameters allow the following use cases:
    - Slave performing an action after receiving a block of data from a master
      containing a command. The action will be performed by the receive handler
      based on the content of the receive data buffer.
    - Slave returning a block of data to the master. The type of information is
      always the same but the actual values change over time. For example,
      returning the voltage of a predefined set of analog inputs.
    - Slave returning data based on a command contained in the first part of the
      SPI transaction. For example, reading the voltage of the analog input
      specified by the first data byte by the master. This is achieved by using
      the MSS_SPI_set_slave_block_buffers() function in conjunction with
      functions MSS_SPI_set_cmd_handler() and MSS_SPI_set_cmd_response(). Please
      refer to the MSS_SPI_set_cmd_handler() function description for details of
      this use case.
  
  @param this_spi
    The this_spi parameter is a pointer to an mss_spi_instance_t structure
    identifying the MSS SPI hardware block to be initialized. There are two such
    data structures, g_mss_spi0 and g_mss_spi1, associated with MSS SPI 0 and
    MSS SPI 1 respectively. This parameter must point to either the g_mss_spi0
    or g_mss_spi1 global data structure defined within the SPI driver.
    
 
  @param tx_buffer
    The tx_buffer parameter is a pointer to a buffer containing the data that
    will be sent to the master. This parameter can be set to 0 if the MSS SPI
    slave is not intended to be the target of SPI read transactions.
 
  @param tx_buff_size
    The tx_buff_size parameter specifies the number of bytes contained in the
    tx_buffer. This parameter can be set to 0 if the MSS SPI slave is not
    intended to be the target of SPI read transactions.
 
  @param rx_buffer
    The rx_buffer parameter is a pointer to the buffer where data received from
    the master will be stored. This parameter can be set to 0 if the MSS SPI
    slave is not intended to be the target of SPI write or write-read
    transactions.
 
  @param rx_buff_size
    The rx_buff_size parameter specifies the size of the receive buffer. It is
    also the number of bytes that must be received before the receive handler
    is called, if a receive handler is specified using the block_rx_handler
    parameter. This parameter can be set to 0 if the MSS SPI slave is not
    intended to be the target of SPI write or write-read transactions.
 
  @param block_rx_handler
    The block_rx_handler parameter is a pointer to a function that will be
    called when the receive buffer has been filled. This parameter can be set
    to 0 if the MSS SPI slave is not intended to be the target of SPI write or
    write-read transactions.
 
  Slave performing operation based on master command:
  In this example the SPI slave is configured to receive 10 bytes of data or
  command from the SPI slave and process the data received from the master.
  @code
     uint32_t nb_of_rx_handler_calls = 0;
     
     void spi1_block_rx_handler_b
     (
         uint8_t * rx_buff,
         uint16_t rx_size
     )
     {
         ++nb_of_rx_handler_calls;
     }
     
     void setup_slave( void )
     {
         uint8_t slave_rx_buffer[10] = 
         {
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
         };
         
         MSS_SPI_init( &g_mss_spi1 );
         MSS_SPI_configure_slave_mode
            ( 
                &g_mss_spi0,
                MSS_SPI_MODE2,
                MSS_SPI_PCLK_DIV_64,
                MSS_SPI_BLOCK_TRANSFER_FRAME_SIZE
            );
         
         MSS_SPI_set_slave_block_buffers
             (
                 &g_mss_spi1,
                 0,
                 0,
                 slave_rx_buffer,
                 sizeof(master_tx_buffer),
                 spi1_block_rx_handler_b
             );
     }
  @endcode
  
  Slave responding to command example:
  In this example the slave will return data based on a command sent by the
  master. The first part of the transaction is handled using polled mode where
  each byte returned to the master is written as part of the interrupt service
  routine. The second part of the transaction, where the slave returns data
  based on the command value, is sent using a DMA transfer initiated by the
  receive handler.
  The MSS_SPI_set_slave_block_buffers() function specifies that five bytes of
  preamble data will be transmitted before the actual response is sent by the
  slave. These five bytes will be transmitted while the one byte command,
  followed by four turnaround bytes, is received. The turnaround bytes give
  time to the slave to call the command handler and setup the PDMA transfer
  that will return the response data.
 
  @code
     #define SLAVE_NB_OF_COMMANDS    4
     #define SLAVE_PACKET_LENGTH     8

     const uint8_t g_slave_data_array[SLAVE_NB_OF_COMMANDS][SLAVE_PACKET_LENGTH] =
     {
         {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08},
         {0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18},
         {0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28},
         {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38}
     };
     
     void spi1_rx_handler_d
     (
         uint8_t * rx_buff,
         uint32_t rx_size
     )
     {
         uint8_t index;
         const uint8_t * p_response;
        
         index = rx_buff[0];
        
         if(index < 4)
         {
             p_response = &g_slave_data_array[index][0];
         }
         else
         {
             p_response = &g_slave_data_array[0][0];
         }
        
         PDMA_start
         (
             PDMA_CHANNEL_0,
             (uint32_t)p_response,
             PDMA_SPI1_TX_REGISTER,
             SLAVE_PACKET_LENGTH
         );
     }
     
     void setup_slave( void )
     {
         uint8_t slave_tx_buffer[16] = { 0x12, 0x34, 0x56, 0x78, 0x9A };

         PDMA_init();
         PDMA->RATIO_HIGH_LOW = 0xFF;
         PDMA_configure
             (
                 PDMA_CHANNEL_0,
                 PDMA_TO_SPI_1,
                 PDMA_LOW_PRIORITY | PDMA_BYTE_TRANSFER | PDMA_INC_SRC_ONE_BYTE,
                 PDMA_DEFAULT_WRITE_ADJ
             );
    
         MSS_SPI_init( &g_mss_spi1 );

         MSS_SPI_configure_slave_mode
             (
                 &g_mss_spi1,
                 MSS_SPI_MODE1,
                 MSS_SPI_PCLK_DIV_256,
                 MSS_SPI_BLOCK_TRANSFER_FRAME_SIZE
             );
        
         MSS_SPI_set_slave_block_buffers
             (
                 &g_mss_spi1,
                 slave_tx_buffer,
                 5,
                 slave_rx_buffer,
                 sizeof(slave_rx_buffer),
                 spi1_rx_handler_d
             );
        
         MSS_SPI_set_cmd_handler
             (
                 &g_mss_spi1,
                 spi1_slave_cmd_handler,
                 1
             );
     }
  @endcode
 */
void MSS_SPI_set_slave_block_buffers
(
    mss_spi_instance_t * this_spi,
    const uint8_t * tx_buffer,
    uint32_t tx_buff_size,
    uint8_t * rx_buffer,
    uint32_t rx_buff_size,
    mss_spi_block_rx_handler_t block_rx_handler
);


/***************************************************************************//**
  The MSS_SPI_set_cmd_handler() function specifies a command handler function
  that will be called when the number of bytes received reaches the command size
  specified as parameter.
  This function is used by SPI slaves performing block transfers. Its purpose is
  to allow a SPI slave to decide the data that will be returned to the master
  while a SPI transaction is taking place. Typically, one of more command bytes
  are sent by the master to request some specific data. The slave interprets the
  command byte(s) while one or more turn-around bytes are transmitted. The
  slave adjusts its transmit data buffer based on the command during the
  turnaround time.
  The diagram below provides an example of the use of this function where the
  SPI slave returns data bytes D0 to D6 based on the value of a command. The
  3 bytes long command is made up of a command opcode byte followed by an address
  byte followed by a size byte. The cmd_handler() function specified through an
  earlier call to MSS_SPI_set_cmd_handler() is called by the SPI driver once the
  third byte is received. The cmd_handler() function interprets the command bytes
  and calls MSS_SPI_set_cmd_response() to set the SPI slave's response transmit
  buffer with the data to be transmitted after the turnaround bytes (T0 to T3).
  The number of turnaround bytes must be sufficient to give enough time for the
  cmd_handler() to execute. The number of turnaround bytes is specified by the
  protocol used on top of the SPI transport layer so that master and slave agree
  on the number of turn around bytes.

    t0          t1       t2         t3                                 t4
    |           |        |          |                                  |
    |------------------------------------------------------------------|
    |  COMMAND  |    TURN-AROUND    |               DATA               |
    |------------------------------------------------------------------|
    | C | A | S | T0 | T1 | T2 | T4 | D0 | D1 | D2 | D3 | D4 | D5 | D6 |
    |------------------------------------------------------------------|
                |
                |
                --> cmd_handler() called here.
                         |
                         |                          
                         --> MSS_SPI_set_cmd_response() called here by
                             implementation of cmd_handler() to set the data
                             that will be transmitted by the SPI slave.

  @param this_spi
    The this_spi parameter is a pointer to an mss_spi_instance_t structure
    identifying the MSS SPI hardware block used. There are two such data
    structures, g_mss_spi0 and g_mss_spi1, associated with MSS SPI 0 and MSS SPI
    1 respectively. This parameter must point to either the g_mss_spi0 or
    g_mss_spi1 global data structure defined within the SPI driver.

  @param cmd_handler
    The cmd_handler parameter is a pointer to a function with prototype:
        void cmd_handler(uint8_t * rx_buff, uint32_t rx_size);
    It specifies the function that will be called when the number of bytes
    specified by parameter cmd_size has been received.

  @param cmd_size
    The cmd_size parameter specifies the number of bytes that must be received
    before the command handler function specified by cmd_handler is called.
  
  The example below demonstrates how to configure SPI1 to implement the protocol
  given as example in the diagram above.
  The configure_slave() function configures SPI1. It sets the receive and transmit
  buffers. The transmit buffer specified through the call to
  MSS_SPI_set_slave_block_buffers() specifies the data that will be returned to
  the master in bytes between t0 and t3. These are the bytes that will be sent
  to the master while the master transmits the command and dummy bytes.
  The spi1_slave_cmd_handler() function will be called by the driver at time t1
  after the 3 command bytes have been received.
  The spi1_block_rx_handler() function will be called by the driver at time t4
  when the transaction completes when the slave select signal becomes de-asserted.
  
  @code
    #define COMMAND_SIZE            3
    #define NB_OF_DUMMY_BYTES       4
    #define MAX_TRANSACTION_SIZE    16
    
    uint8_t slave_tx_buffer[COMMAND_SIZE + NB_OF_DUMMY_BYTES];
    uint8_t slave_rx_buffer[MAX_TRANSACTION_SIZE];
    
    void configure_slave(void)
    {
        MSS_SPI_init( &g_mss_spi1 );

        MSS_SPI_configure_slave_mode
          (
            &g_mss_spi1,
            MSS_SPI_MODE1,
            MSS_SPI_PCLK_DIV_256,
            MSS_SPI_BLOCK_TRANSFER_FRAME_SIZE
          );
      
        MSS_SPI_set_slave_block_buffers
            (
                &g_mss_spi1,
                slave_tx_buffer,
                COMMAND_SIZE + NB_OF_DUMMY_BYTES,
                slave_rx_buffer,
                sizeof(slave_rx_buffer),
                spi1_block_rx_handler
            );
            
        
        MSS_SPI_set_cmd_handler
            (
                &g_mss_spi1,
                spi1_slave_cmd_handler,
                COMMAND_SIZE
            );
    }   
    
    void spi1_slave_cmd_handler
    (
        uint8_t * rx_buff,
        uint32_t rx_size
    )
    {
        uint8_t command;
        uint8_t address;
        uint8_t size;
        
        uint8_t * p_response;
        uint32_t response_size;
        
        command = rx_buff[0];
        address = rx_buff[1];
        size = rx_buff[2];
        
        p_response = get_response_data(command, address, size, &response_size);
        
        MSS_SPI_set_cmd_response(&g_mss_spi1, p_response, response_size);
    }

    void spi1_block_rx_handler
    (
        uint8_t * rx_buff,
        uint32_t rx_size
    )
    {
        process_rx_data(rx_buff, rx_size);
    }    
  @endcode
 */
void MSS_SPI_set_cmd_handler
(
    mss_spi_instance_t * this_spi,
    mss_spi_block_rx_handler_t cmd_handler,
    uint32_t cmd_size
);

/***************************************************************************//**
 The MSS_SPI_set_cmd_response() function specifies the data that will be
 returned to the master. See the description of MSS_SPI_set_cmd_handler() for
 details.
 
  @param this_spi
    The this_spi parameter is a pointer to an mss_spi_instance_t structure
    identifying the MSS SPI hardware block used. There are two such data
    structures, g_mss_spi0 and g_mss_spi1, associated with MSS SPI 0 and MSS SPI
    1 respectively. This parameter must point to either the g_mss_spi0 or
    g_mss_spi1 global data structure defined within the SPI driver.

  @param resp_tx_buffer
    The resp_tx_buffer parameter is a pointer to the buffer containing the data
    that must be returned to the host in the data phase of a SPI transaction.
  
  @param resp_buff_size
    The resp_buff_size parameter specifies the size of the buffer pointed to by
    the resp_tx_buffer parameter.
 */
void MSS_SPI_set_cmd_response
(
    mss_spi_instance_t * this_spi,
    const uint8_t * resp_tx_buffer,
    uint32_t resp_buff_size
);


#ifdef __cplusplus
}
#endif

#endif /* MSS_SPI_H_*/
