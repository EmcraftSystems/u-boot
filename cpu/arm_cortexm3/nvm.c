#include "16550_regs.h"
#include "CMSIS/a2fxxxm3.h"

/*
 * TO-DO: redefine this
 */
#define FLASH_SIZE			(1024 * 256)
#define FLASH_START			(0x00000000)

#define MSS_NVM_MAX_WAIT_CNT		1000
#define MSS_NVM_WAIT_INTERVAL		100

/*
    MSS NVM definitions
*/
#define MSS_NVM_PAGE_SIZE               128
#define MSS_NVM_PAGE_OFFSET_MASK        (MSS_NVM_PAGE_SIZE - 1)
#define MSS_NVM_PAGE_ADDR_MASK          (~MSS_NVM_PAGE_OFFSET_MASK & 0xFFFFF)

/* Relevant MSS NVM control register bits */
#define MSS_NVM_CONTROL_CC_UNPROTECT    0x02000000
#define MSS_NVM_CONTROL_CC_DISCARD      0x04000000
#define MSS_NVM_CONTROL_CC_PROGRAM      0x10000000

/* Relevant MSS NVM status register bits */
#define MSS_NVM_STATUS_BUSY		((1<<0)|(1<<16))
#define MSS_NVM_STATUS_ERROR_MASK	(0x300|(0x300<<16))

/* Relevant ENVM_CR bits */
#define MSS_NVM_CR_PIPE_BYPASS		(1<<6)
#define MSS_NVM_CR_SIX_CYCLE		(1<<7)

#if 0
/*
    Execute an NVM command and wait for its completion.
    Return 0 on success or -1 otherwise.
*/
static void  __attribute__((section(".ramcode")))
 my_putc(char c)
{
    int wait = 1000;

    /* Wait for TX FIFO RDY */
    while (wait--) {
        if (UART0->LSR & LSR_THRE) {
            break;
        }
    	TIMER->TIM1_LOADVAL = 5 * (g_FrequencyPCLK0 / 1000000);
    	while (TIMER->TIM1_VAL) ;
    }

    UART0->THR = c;
}
#endif 

static int __attribute__((section(".ramcode"))) 
  mss_nvm_exec_cmd(unsigned int addr, unsigned int cmd)
{
    unsigned int status;
    int wait;

    addr &= MSS_NVM_PAGE_ADDR_MASK;

    /* Clear pending status bits */
    ENVM_REGS->STATUS = 0xFFFFFFFF;

    /* Execute the command */
    ENVM_REGS->CONTROL = addr | cmd;

    /*
 	Wait for completion (this is probably unnecessary
	if executing from NVM since instruction fetches
	will be blocked anyway...)
    */
    for (wait = 0; wait < MSS_NVM_MAX_WAIT_CNT; wait++) {
        status = ENVM_REGS->STATUS;
	if (!(status & MSS_NVM_STATUS_BUSY)) {
	    break;
	}

    	TIMER->TIM1_LOADVAL = 
		MSS_NVM_WAIT_INTERVAL * (g_FrequencyPCLK0 / 1000000);
    	while (TIMER->TIM1_VAL) ;

    }

    if (status & MSS_NVM_STATUS_BUSY) {
#if 0
	debug_puts(__func__);
	debug_puts(": page ");
	debug_putdec(addr / MSS_NVM_PAGE_SIZE);
	debug_puts(": NVM busy timeout (status = ");
	debug_puthex(status, 8);
	debug_puts(")\n");
#endif
	return -1;
    }

    if (status & MSS_NVM_STATUS_ERROR_MASK) {
#if 0
	debug_puts(__func__);
	debug_puts(": page ");
	debug_putdec(addr / MSS_NVM_PAGE_SIZE);
	debug_puts(": NVM error (status = ");
	debug_puthex(status, 8);
	debug_puts(")\n");
#endif
	/*
 	 *  This code below is a workaround for an occurance 
 	 * of the write count has exceeded the 10-year retention
 	 * threshold error for some page. 
 	 * Apparently, that error is persistently set for some
 	 * pages on the boards we have here (supposedly due to a power-off
 	 * while programming the board using FlashPro.
 	 * ... Supposedly, FlashPro uses the same workaround.
 	 * The bit value in the code below doesn't correspond to 
 	 * the value in the Actel SmartFusion DataSheet.  
 	 * It has been figured out experimentally (the same page 
 	 * on the board I have has that problem, and the code below
 	 * allows to actually program that page with new content.
 	 */
	if ((status & 0x180) == 0x180)  {
		/*
		 * Assume that the page has been programmed successfully
		 */
		return 0;
	}	
	return -1;
    }

    return 0;
}

/*
    MSS NVM API
*/

/* Write to NVM */
unsigned int __attribute__((section(".ramcode")))
             __attribute__ ((long_call))
  nvm_write(unsigned int offset, void *buf, unsigned int size)
{
    unsigned int addr = ENVM_BASE + offset;
    unsigned char *src = (unsigned char *)buf;
    unsigned int i, written = 0;
    int ret = 0;

    if (offset > FLASH_SIZE) {
#if 0
	debug_puts(__func__);
	debug_puts(": offset is bigger than Flash size (");
	debug_putdec(offset);
	debug_puts(")\n");
#endif
	return 0;
    }

    for (i = 0; i < size; i++) {
	if (i == 0 || (addr & MSS_NVM_PAGE_OFFSET_MASK) == 0) {
	    /* First access: unprotect the page */
	    ret = mss_nvm_exec_cmd(addr, MSS_NVM_CONTROL_CC_UNPROTECT);
	    if (ret < 0) {
		break;
	    }
	}

	/* Store data (will be written to page buffer) */
	*(unsigned char *)addr++ = *src++;

	/* If we just wrote the last byte, commit the page to NVM */
	if (i == size - 1 || (addr & MSS_NVM_PAGE_OFFSET_MASK) == 0) {
	    ret = mss_nvm_exec_cmd(addr - 1, MSS_NVM_CONTROL_CC_PROGRAM);
	    if (ret < 0) {
		break;
	    }
	    written = i + 1;
	}
    }

    return written;
}

#if 0
/* Read from NVM */
unsigned int nvm_read(unsigned int offset, void *buf, unsigned int size)
{
    memcpy(buf, (unsigned char *)(FLASH_START + offset), size);
    return size;
}
#endif

/* Initialize the NVM interface */
void nvm_init(void)
{
    /* Disable all interrupts */
    ENVM_REGS->ENABLE = 0;

    /* Clear CONFIG registers */
    ENVM_REGS->CONFIG_0 = 0;
    ENVM_REGS->CONFIG_1 = 0;

    /* Clear NVM select */
    ENVM_REGS->ENVM_SELECT = 0;

    /* Clear status reg */
    ENVM_REGS->STATUS = 0;
}
