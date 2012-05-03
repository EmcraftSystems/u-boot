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

#ifndef _MACH_CCU_H_
#define _MACH_CCU_H_

/*
 * CCU1 (Clock Control Unit 1) register map
 */
struct lpc18xx_ccu1_regs {
	u32 pm;				/* CCU1 power mode register */
	u32 base_stat;			/* CCU1 base clock status register */
	u32 rsv0[62];

	u32 clk_apb3_bus_cfg;		/* CLK_APB3_BUS clock configuration */
	u32 clk_apb3_bus_stat;		/* CLK_APB3_BUS clock status */
	u32 clk_apb3_i2c1_cfg;		/* CLK_APB3_I2C1 configuration */
	u32 clk_apb3_i2c1_stat;		/* CLK_APB3_I2C1 status */
	u32 clk_apb3_dac_cfg;		/* CLK_APB3_DAC configuration */
	u32 clk_apb3_dac_stat;		/* CLK_APB3_DAC status */
	u32 clk_apb3_adc0_cfg;		/* CLK_APB3_ADC0 configuration */
	u32 clk_apb3_adc0_stat;		/* CLK_APB3_ADC0 status */
	u32 clk_apb3_adc1_cfg;		/* CLK_APB3_ADC1 configuration */
	u32 clk_apb3_adc1_stat;		/* CLK_APB3_ADC1 status */
	u32 clk_apb3_can0_cfg;		/* CLK_APB3_CAN0 configuration */
	u32 clk_apb3_can0_stat;		/* CLK_APB3_CAN0 status */
	u32 rsv1[52];

	u32 clk_apb1_bus_cfg;		/* CLK_APB1_BUS configuration */
	u32 clk_apb1_bus_stat;		/* CLK_APB1_BUS status */
	u32 clk_apb1_motocon_cfg;	/* CLK_APB1_MOTOCON configuration */
	u32 clk_apb1_motocon_stat;	/* CLK_APB1_MOTOCON status */
	u32 clk_apb1_i2c0_cfg;		/* CLK_APB1_I2C0 configuration */
	u32 clk_apb1_i2c0_stat;		/* CLK_APB1_I2C0 status */
	u32 clk_apb1_i2s_cfg;		/* CLK_APB1_I2S configuration */
	u32 clk_apb1_i2s_stat;		/* CLK_APB1_I2S status */
	u32 clk_apb1_can1_cfg;		/* CLK_APB3_CAN1 configuration */
	u32 clk_apb1_can1_stat;		/* CLK_APB3_CAN1 status */
	u32 rsv2[54];

	u32 clk_spifi_cfg;		/* CLK_SPIFI configuration */
	u32 clk_spifi_stat;		/* CLK_SPIFI status */
	u32 rsv3[62];

	u32 clk_m4_bus_cfg;		/* CLK_M4_BUS configuration */
	u32 clk_m4_bus_stat;		/* CLK_M4_BUS status */
	u32 clk_m4_spifi_cfg;		/* CLK_M4_SPIFI configuration */
	u32 clk_m4_spifi_stat;		/* CLK_M4_SPIFI status */
	u32 clk_m4_gpio_cfg;		/* CLK_M4_GPIO configuration */
	u32 clk_m4_gpio_stat;		/* CLK_M4_GPIO status */
	u32 clk_m4_lcd_cfg;		/* CLK_M4_LCD configuration */
	u32 clk_m4_lcd_stat;		/* CLK_M4_LCD status */
	u32 clk_m4_ethernet_cfg;	/* CLK_M4_ETHERNET configuration */
	u32 clk_m4_ethernet_stat;	/* CLK_M4_ETHERNET status */
	u32 clk_m4_usb0_cfg;		/* CLK_M4_USB0 configuration */
	u32 clk_m4_usb0_stat;		/* CLK_M4_USB0 status */
	u32 clk_m4_emc_cfg;		/* CLK_M4_EMC configuration */
	u32 clk_m4_emc_stat;		/* CLK_M4_EMC status */
	u32 clk_m4_sdio_cfg;		/* CLK_M4_SDIO configuration */
	u32 clk_m4_sdio_stat;		/* CLK_M4_SDIO status */
	u32 clk_m4_dma_cfg;		/* CLK_M4_DMA configuration */
	u32 clk_m4_dma_stat;		/* CLK_M4_DMA status */
	u32 clk_m4_m4core_cfg;		/* CLK_M4_M4CORE configuration */
	u32 clk_m4_m4core_stat;		/* CLK_M4_M4CORE status */
	u32 rsv4[6];

	u32 clk_m4_sct_cfg;		/* CLK_M4_SCT configuration */
	u32 clk_m4_sct_stat;		/* CLK_M4_SCT status */
	u32 clk_m4_usb1_cfg;		/* CLK_M4_USB1 configuration */
	u32 clk_m4_usb1_stat;		/* CLK_M4_USB1 status */
	u32 clk_m4_emcdiv_cfg;		/* CLK_M4_EMCDIV configuration */
	u32 clk_m4_emcdiv_stat;		/* CLK_M4_EMCDIV status */
	u32 rsv5[4];

	u32 clk_m4_m0app_cfg;		/* CLK_M4_M0_CFG configuration */
	u32 clk_m4_m0app_stat;		/* CLK_M4_M0_STAT status */
	u32 clk_m4_vadc_cfg;		/* CLK_M4_VADC_CFG configuration */
	u32 clk_m4_vadc_stat;		/* CLK_M4_VADC_STAT configuration */
	u32 rsv6[24];

	u32 clk_m4_wwdt_cfg;		/* CLK_M4_WWDT configuration */
	u32 clk_m4_wwdt_stat;		/* CLK_M4_WWDT status */
	u32 clk_m4_usart0_cfg;		/* CLK_M4_UART0 configuration */
	u32 clk_m4_usart0_stat;		/* CLK_M4_UART0 status */
	u32 clk_m4_uart1_cfg;		/* CLK_M4_UART1 configuration */
	u32 clk_m4_uart1_stat;		/* CLK_M4_UART1 status */
	u32 clk_m4_ssp0_cfg;		/* CLK_M4_SSP0 configuration */
	u32 clk_m4_ssp0_stat;		/* CLK_M4_SSP0 status */
	u32 clk_m4_timer0_cfg;		/* CLK_M4_TIMER0 configuration */
	u32 clk_m4_timer0_stat;		/* CLK_M4_TIMER0 status */
	u32 clk_m4_timer1_cfg;		/* CLK_M4_TIMER1 configuration */
	u32 clk_m4_timer1_stat;		/* CLK_M4_TIMER1 status */
	u32 clk_m4_scu_cfg;		/* CLK_M4_SCU configuration */
	u32 clk_m4_scu_stat;		/* CLK_M4_SCU status */
	u32 clk_m4_creg_cfg;		/* CLK_M4_CREG configuration */
	u32 clk_m4_creg_stat;		/* CLK_M4_CREG status */
	u32 rsv7[48];

	u32 clk_m4_ritimer_cfg;		/* CLK_M4_RITIMER configuration */
	u32 clk_m4_ritimer_stat;	/* CLK_M4_RITIMER status */
	u32 clk_m4_usart2_cfg;		/* CLK_M4_UART2 configuration */
	u32 clk_m4_usart2_stat;		/* CLK_M4_UART2 status */
	u32 clk_m4_usart3_cfg;		/* CLK_M4_UART3 configuration */
	u32 clk_m4_usart3_stat;		/* CLK_M4_UART3 status */
	u32 clk_m4_timer2_cfg;		/* CLK_M4_TIMER2 configuration */
	u32 clk_m4_timer2_stat;		/* CLK_M4_TIMER2 status */
	u32 clk_m4_timer3_cfg;		/* CLK_M4_TIMER3 configuration */
	u32 clk_m4_timer3_stat;		/* CLK_M4_TIMER3 status */
	u32 clk_m4_ssp1_cfg;		/* CLK_M4_SSP1 configuration */
	u32 clk_m4_ssp1_stat;		/* CLK_M4_SSP1 status */
	u32 clk_m4_qei_cfg;		/* CLK_M4_QEI configuration */
	u32 clk_m4_qei_stat;		/* CLK_M4_QEI status */
	u32 rsv8[50];

	u32 clk_periph_bus_cfg;		/* CLK_PERIPH_BUS configuration */
	u32 clk_periph_bus_stat;	/* CLK_PERIPH_BUS status */
	u32 rsv9[2];

	u32 clk_periph_core_cfg;	/* CLK_PERIPH_CORE configuration */
	u32 clk_periph_core_stat;	/* CLK_PERIPH_CORE status */
	u32 clk_periph_sgpio_cfg;	/* CLK_PERIPH_SGPIO configuration */
	u32 clk_periph_sgpio_stat;	/* CLK_PERIPH_SGPIO status */
	u32 rsv10[56];

	u32 clk_usb0_cfg;		/* CLK_USB0 configuration */
	u32 clk_usb0_stat;		/* CLK_USB0 status */
	u32 rsv11[62];

	u32 clk_usb1_cfg;		/* CLK_USB1 configuration */
	u32 clk_usb1_stat;		/* CLK_USB1 status */
	u32 rsv12[62];

	u32 clk_spi_cfg;		/* CLK_SPI configuration */
	u32 clk_spi_stat;		/* CLK_SPI status */
	u32 rsv13[62];

	u32 clk_vadc_cfg;		/* CLK_VADC configuration */
	u32 clk_vadc_stat;		/* CLK_VADC status */
};

/*
 * CCU1 registers base
 */
#define LPC18XX_CCU1_BASE		0x40051000
#define LPC18XX_CCU1			((volatile struct lpc18xx_ccu1_regs *) \
					LPC18XX_CCU1_BASE)

/*
 * All clock configuration registers
 */
#define LPC18XX_CCU1_CLK_RUN_MSK	(1 << 0)

/*
 * CLK_M4_EMCDIV_CFG register
 */
/* Divider selector */
#define LPC18XX_CCU1_CLK_EMCDIV_CFG_DIV2	(1 << 5)

#endif /* _MACH_CCU_H_ */
