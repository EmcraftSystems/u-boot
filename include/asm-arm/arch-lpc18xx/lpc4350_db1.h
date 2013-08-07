/*******************************************************************************
*
* Project: LPC4350-DB1 Development Board Examples
*
* Description: Definition of LPC4350-DB1 Development Board dependent GPIOs
*
* Copyright by Diolan Ltd. All rights reserved.
*
*******************************************************************************/



#ifndef __LPC4350_DB1_H
#define __LPC4350_DB1_H

/*******************************************************************************
 *                 LEDs
 ******************************************************************************/
#define D2_SCU_PIN 6
#define D2_SCU_PORT 6
#define D2_GPIO_PIN 5
#define D2_GPIO_PORT 0
#define D2_GPIO_MASK (1 << D2_GPIO_PIN)

#define D3_SCU_PIN 14
#define D3_SCU_PORT 0xD
#define D3_GPIO_PIN 28
#define D3_GPIO_PORT 6
#define D3_GPIO_MASK (1 << D3_GPIO_PIN)

#define D4_SCU_PIN 12
#define D4_SCU_PORT 0xD
#define D4_GPIO_PIN 26
#define D4_GPIO_PORT 6
#define D4_GPIO_MASK (1 << D4_GPIO_PIN)

#define D5_SCU_PIN 10
#define D5_SCU_PORT 0xD
#define D5_GPIO_PIN 24
#define D5_GPIO_PORT 6
#define D5_GPIO_MASK (1 << D5_GPIO_PIN)

#define D6_SCU_PIN 11
#define D6_SCU_PORT 0xD
#define D6_GPIO_PIN 25
#define D6_GPIO_PORT 6
#define D6_GPIO_MASK (1 << D6_GPIO_PIN)


#define S1_SCU_PIN 1
#define S1_SCU_PORT 0xD
#define S1_GPIO_PIN 15
#define S1_GPIO_PORT 6
#define S1_GPIO_MASK (1 << S1_GPIO_PIN)

#define S2_SCU_PIN 13
#define S2_SCU_PORT 0xD
#define S2_GPIO_PIN 27
#define S2_GPIO_PORT 6
#define S2_GPIO_MASK (1 << S2_GPIO_PIN)

#define S3_SCU_PIN 15
#define S3_SCU_PORT 0xE
#define S3_GPIO_PIN 15
#define S3_GPIO_PORT 7
#define S3_GPIO_MASK (1 << S3_GPIO_PIN)

#define S4_SCU_PIN 14
#define S4_SCU_PORT 0xE
#define S4_GPIO_PIN 14
#define S4_GPIO_PORT 7
#define S4_GPIO_MASK (1 << S4_GPIO_PIN)


#endif // __LPC4350_DB1_H
