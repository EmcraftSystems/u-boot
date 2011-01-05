#ifndef __ENVM_H__
#define __ENVM_H__

/*
 * Initialize the eNVM interface
 */
extern void envm_init(void);

/*
 * Write a data buffer to eNVM.
 * Note that we need for this function to reside in RAM since it
 * will be used to self-upgrade U-boot in eNMV.
 */
extern unsigned int
	__attribute__((section(".ramcode")))
	__attribute__ ((long_call))
	envm_write(unsigned int offset, void * buf, unsigned int size);

#endif /* __ENVM_H__ */
