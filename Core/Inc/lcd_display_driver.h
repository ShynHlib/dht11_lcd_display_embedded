/*
 * lcd_display_driver.h
 *
 *  Created on: May 14, 2024
 *      Author: Shyn Hlib
 */

#ifndef INC_LCD_DRIVER_H_
#define INC_LCD_DRIVER_H_

#include "main.h"
#include <stdbool.h>

bool display_init(I2C_HandleTypeDef *pI2cHandle);

void display_setCursor(uint8_t row, uint8_t col);

void display_clear(void);

void display_shiftRight(uint8_t offset);

void display_shiftLeft(uint8_t offset);

void display_printf(const char* str, ...);

#endif /* INC_LCD_DISPLAY_DRIVER_H_ */
