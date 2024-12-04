/*
 * drv_led.c
 *
 *  Created on: Dec 3, 2024
 *      Author: paul_
 */
#include "main.h"
#include "drv_led.h"
#include "spi.h"

// Fonction pour écrire dans un registre du MCP23S17
void MCP23S17_Write( uint8_t reg, uint8_t value) {
	uint8_t data[3] = {MCP23S17_ADDR_WRITE, reg, value};
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET); // CS Low
	HAL_SPI_Transmit(&hspi3, data, 3, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);   // CS High
}

uint8_t drv_led(uint8_t led_num, uint8_t gpio)
{
	uint8_t etat_des_LEDs = 0xff;
	if((led_num>=0 && led_num<=7)!=0){
		etat_des_LEDs ^= (1 << led_num);
	}
	// Allumer la LED (GPB0 à HIGH)
	MCP23S17_Write(gpio, etat_des_LEDs);
	return 0;
}

