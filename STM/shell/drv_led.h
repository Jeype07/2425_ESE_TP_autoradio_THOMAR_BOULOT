/*
 * drv_led.h
 *
 *  Created on: Dec 3, 2024
 *      Author: paul_
 */

#ifndef DRV_LED_H_
#define DRV_LED_H_

//MCP2317 REGISTER
#define MCP23S17_ADDR_WRITE  0x40
#define MCP23S17_ADDR_READ  0x41

#define IODIRA  0x00
#define IODIRB  0x01

#define MCPGPIOA   0x12
#define MCPGPIOB   0x13

#include <stdint.h>

uint8_t drv_led(uint8_t led_num, uint8_t gpio);
void MCP23S17_Write( uint8_t reg, uint8_t value);

#endif /* DRV_LED_H_ */
