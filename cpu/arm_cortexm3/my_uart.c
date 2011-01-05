/*
    MSS UART driver
*/

#include "16550_regs.h"
#include "CMSIS/a2fxxxm3.h"

void my_uart_init(int baud_rate)
{
    unsigned short baud_val;

    /* FCR: all FIFOs enabled */
    UART0->FCR = (FCR_EN_RX_TX_FIFO | FCR_CLR_RX_FIFO
        | FCR_CLR_TX_FIFO | FCR_RX_TX_RDY_EN
        | ((0x3 & FCR_RX_TRIG_LEVEL_MASK) << FCR_RX_TRIG_LEVEL_SHIFT));
    /* LCR: 8 bits, no parity, 1 stop bit */
    UART0->LCR = ((0x3 & LCR_WLS_MASK) << LCR_WLS_SHIFT);
    /* MCR: no modem control currently supported */
    UART0->MCR = 0;
    /* IER: interrupts disabled */
    UART0->IER = 0;

    /* Set baud rate */
    baud_val = (((g_FrequencyPCLK0 / 16) + (baud_rate / 2)) / baud_rate);
    /* Enable DLAB */
    UART0->LCR |= LCR_DLAB;
    /* Write divisor value */
    UART0->DLR = baud_val & 0xFF;
    UART0->DMR = baud_val >> 8;
    /* Disable DLAB */
    UART0->LCR &= ~LCR_DLAB;
}
