/*
 * sgtl5000.h
 *
 *  Created on: Dec 4, 2024
 *      Author: jeanp
 */

#ifndef INC_SGTL5000_H_
#define INC_SGTL5000_H_

#include <stdio.h>
#include "stm32l4xx_hal.h"

#define CHIP_ID 0x0000
#define SGTL5000_ADDR 0x14 << 1
#define CHIP_ID_REG_SIZE 16

int sglt5000_get_CHIP_ID_val(void);

#endif /* INC_SGTL5000_H_ */
