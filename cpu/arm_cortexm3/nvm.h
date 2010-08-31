/*
    Copyright (c) 2009-2010 Pigeon Point Systems.
    All rights reserved.
    Pigeon Point Systems proprietary and confidential.

    Description:
    	This header file contains declarations of
	generic NVM access functions.
    
    $Revision: 9329 $
*/

#ifndef __NVM_H__
#define __NVM_H__

#define NVM_PAGE_SIZE	128

extern unsigned int
	__attribute__((section(".ramcode")))
	__attribute__ ((long_call))
	nvm_write(unsigned int offset, void *buf, unsigned int size);
#if 0
extern unsigned int nvm_read(unsigned int offset, void *buf,
	unsigned int size);
#endif
extern void nvm_init(void);

#endif /* __NVM_H__ */
