/*----------------------------------------------------------------------------
 *      Name:    serial.h
 *      Purpose: serial port handling
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on NXP Semiconductors LPC microcontroller devices only. Nothing else
 *      gives you the right to use this software.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#define PORT_NUM	0

/*----------------------------------------------------------------------------
 Serial interface related prototypes
 *---------------------------------------------------------------------------*/
extern void  ser_OpenPort  (char portNum);
extern void  ser_ClosePort (char portNum);
extern void  ser_InitPort0  (unsigned long baudrate, unsigned int databits, unsigned int parity, unsigned int stopbits);
extern void  ser_InitPort1  (unsigned long baudrate, unsigned int databits, unsigned int parity, unsigned int stopbits);
extern void  ser_AvailChar (int *availChar);
extern int   ser_Write     (char portNum, const char *buffer, int *length);
extern int   ser_Read      (char *buffer, const int *length);
extern void  ser_LineState (unsigned short *lineState);

