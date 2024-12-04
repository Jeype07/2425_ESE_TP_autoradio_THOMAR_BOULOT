/*
 * sgtl5000.c
 *
 *  Created on: Dec 4, 2024
 *      Author: jeanp
 */

#include "sgtl5000.h"

I2C_HandleTypeDef hi2c2;

int sglt5000_get_CHIP_ID_val(void){
	uint16_t pData[1];
	if (HAL_I2C_Mem_Read(&hi2c2, SGTL5000_ADDR, CHIP_ID,CHIP_ID_REG_SIZE, pData, sizeof(pData), HAL_MAX_Delay)== HAL_OK){
		printf("CHIP_ID value : \r\n",pData[1]);
		return 1;
	}
	else{
		printf("Error");
		return 0;
	}
}
