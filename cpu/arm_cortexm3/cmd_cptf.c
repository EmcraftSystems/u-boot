
#include <common.h>
#include <command.h>
#include <string.h>
#include "CMSIS/a2fxxxm3.h"
#include "nvm.h"

#define A2F_RAM_BUFFER_BASE	0x20004000
#define A2F_RAM_BUFFER_SIZE	0x8000

/*
 * The in-RAM copy of the processor reset function.
 * We need it in RAM so as to be able to reset the CPU
 * after we have upgraded U-boot in the internal Flash.
 */
static int __attribute__((section(".ramcode")))
           __attribute__ ((long_call)) 
  nvm_write_and_reset(ulong dst, ulong src, ulong size, int do_reset)
{
	int ret = 0;

	/*
 	 * Copy the buffer to the destination.
 	 */
	if (nvm_write((uint) dst, (void *) src, (uint) size) != size) {
		ret = -1;
		goto Done;
	}

	/*
 	 * If the user needs a reset, do the reset
 	 */
	if (do_reset) {
		/*
	 	 * Cortex-M3 core reset.
 	 	 * This call is actually all inlined so there are
 	 	 * no function calls in here and no care should be
 	 	 * taken about "long-call" function-calling conventions
 	 	 */
		NVIC_SystemReset();

		/*
	 	 * Should never be here.
	 	 */
	}

	Done:
	return ret;

}

#if 0
int do_cpff(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong flash_buf;
	ulong ram_buf = A2F_RAM_BUFFER_BASE;
	int ret = 0;

	if (argc == 1) {
		printf("%s: Flash offset must be specified\n",
                       (char *) argv[0]);
		goto Done;
	}

	flash_buf = simple_strtoul(argv[1], NULL, 16);

	printf("%s: f=%x r=%x\n",
               (char *) argv[0], (uint) flash_buf, (uint) ram_buf);

	(void) memcpy((void *) ram_buf, (void *) flash_buf,
                       A2F_RAM_BUFFER_SIZE);

	Done:
	return ret;
}

U_BOOT_CMD(
	cpff,	2,		0,	do_cpff,
	"copy internal Flash of the A2F to a RAM buffer",
	"flash_off [ram_buf]"
);
#endif 

 /*
  * cptf: copy content of a buffer (including one in Flash) to Flash.
  */

int do_cptf(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong dst;
	ulong src = A2F_RAM_BUFFER_BASE;
	ulong size = A2F_RAM_BUFFER_SIZE;
	int do_reset = 0;
	int ret = 0;

	/*
	 * Check that at least the destination is specified
	 */
	if (argc == 1) {
		printf("%s: Flash offset must be specified\n",
                       (char *) argv[0]);
		goto Done;
	}

	/*
 	 * Parse the command arguments
 	 */
	dst = simple_strtoul(argv[1], NULL, 16);
	if (argc >= 3) {
		src = simple_strtoul(argv[2], NULL, 16);
	}
	if (argc >= 4) {
		size = simple_strtoul(argv[3], NULL, 16);
	}
	if (argc >= 5) {
		do_reset = simple_strtol(argv[4], NULL, 16);
	}

	printf("%s: dst=%x src=%x sz=%x, do=%d\n",
              (char *) argv[0], (uint) dst, (uint) src, (uint) size, do_reset);

	/*
 	 * Copy the buffer to the destination.
 	 */
	if (nvm_write_and_reset(dst, src, size, do_reset)) {
		printf("%s: nvm_write_and_reset failed\n", (char *) argv[0]);
		goto Done;
	}

	Done:
	return ret;
}

U_BOOT_CMD(
	cptf,	5,		0,	do_cptf,
	"copy memory buffer to internal Flash of the A2F",
	"dst [[src] [[size] [do_reset]]]"
);
