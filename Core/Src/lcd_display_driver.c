/*
 * lcd_display_driver.c
 *
 *  Created on: May 14, 2024
 *      Author: Shyn Hlib
 */

#include "lcd_display_driver.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define LCD_CLEARDISPLAY    0x01
#define LCD_RETURNHOME      0x02
#define LCD_ENTRYMODESET    0x04
#define LCD_DISPLAYCONTROL  0x08
#define LCD_CURSORSHIFT     0x10
#define LCD_FUNCTIONSET     0x20
#define LCD_SETCGRAMADDR    0x40
#define LCD_SETDDRAMADDR    0x80

#define LCD_ENTRY_SH      0x01
#define LCD_ENTRY_ID      0x02

#define LCD_DISPLAY_B     0x01
#define LCD_DISPLAY_C     0x02
#define LCD_DISPLAY_D     0x04

#define LCD_SHIFT_RL      0x04
#define LCD_SHIFT_SC      0x08

#define LCD_FUNCTION_F    0x04
#define LCD_FUNCTION_N    0x08
#define LCD_FUNCTION_DL   0x10


#define LCD_RS        (1 << 0)
#define LCD_RW        (1 << 1)
#define LCD_EN        (1 << 2)
#define LCD_BK_LIGHT  (1 << 3)


static I2C_HandleTypeDef* displayHandle;
static uint8_t LCD_I2C_SLAVE_ADDRESS=0;
#define LCD_I2C_SLAVE_ADDRESS_0  0x4E
#define LCD_I2C_SLAVE_ADDRESS_1  0x7E


static void display_sendCommand(uint8_t command)
{
  const uint8_t command_0_3 = (0xF0 & (command<<4));
  const uint8_t command_4_7 = (0xF0 & command);
  uint8_t i2cData[4] =
  {
      command_4_7 | LCD_EN | LCD_BK_LIGHT,
      command_4_7 | LCD_BK_LIGHT,
      command_0_3 | LCD_EN | LCD_BK_LIGHT,
      command_0_3 | LCD_BK_LIGHT,
  };
  HAL_I2C_Master_Transmit(displayHandle, LCD_I2C_SLAVE_ADDRESS, i2cData, 4, 200);
}

static void display_sendData(uint8_t data)
{
  const uint8_t data_0_3 = (0xF0 & (data<<4));
  const uint8_t data_4_7 = (0xF0 & data);
  uint8_t i2cData[4] =
  {
      data_4_7 | LCD_EN | LCD_BK_LIGHT | LCD_RS,
      data_4_7 | LCD_BK_LIGHT | LCD_RS,
      data_0_3 | LCD_EN | LCD_BK_LIGHT | LCD_RS,
      data_0_3 | LCD_BK_LIGHT | LCD_RS,
  };
  HAL_I2C_Master_Transmit(displayHandle, LCD_I2C_SLAVE_ADDRESS, i2cData, 4, 200);
}

bool display_init(I2C_HandleTypeDef *pI2cHandle)
{
  HAL_Delay(50);
  displayHandle = pI2cHandle;
  if(HAL_I2C_IsDeviceReady(displayHandle, LCD_I2C_SLAVE_ADDRESS_0, 5, 500) != HAL_OK)
  {
    if(HAL_I2C_IsDeviceReady(displayHandle, LCD_I2C_SLAVE_ADDRESS_1, 5, 500) != HAL_OK)
    {
      return false;
    }
    else
    {
      LCD_I2C_SLAVE_ADDRESS = LCD_I2C_SLAVE_ADDRESS_1;
    }
  }
  else
  {
    LCD_I2C_SLAVE_ADDRESS = LCD_I2C_SLAVE_ADDRESS_0;
  }

  HAL_Delay(45);

  display_sendCommand(0x30);
  HAL_Delay(5);
  display_sendCommand(0x30);
  HAL_Delay(1);
  display_sendCommand(0x30);
  HAL_Delay(8);
  display_sendCommand(0x20);
  HAL_Delay(8);

  display_sendCommand(LCD_FUNCTIONSET | LCD_FUNCTION_N);
  HAL_Delay(1);
  display_sendCommand(LCD_DISPLAYCONTROL);
  HAL_Delay(1);
  display_sendCommand(LCD_CLEARDISPLAY);
  HAL_Delay(3);
  display_sendCommand(0x04 | LCD_ENTRY_ID);
  HAL_Delay(1);
  display_sendCommand(LCD_DISPLAYCONTROL | LCD_DISPLAY_D);
  HAL_Delay(3);

  return true;
}

void display_setCursor(uint8_t col, uint8_t row)
{
  uint8_t maskData;
  maskData = (col)&0x0F;
  if(row==0)
  {
    maskData |= (0x80);
    display_sendCommand(maskData);
  }
  else
  {
    maskData |= (0xc0);
    display_sendCommand(maskData);
  }
}

void display_clear(void)
{
  display_sendCommand(LCD_CLEARDISPLAY);
  HAL_Delay(3);
}

void display_shiftRight(uint8_t offset)
{
  for(uint8_t i=0; i<offset;i++)
  {
    display_sendCommand(0x1c);
  }
}

void display_shiftLeft(uint8_t offset)
{
  for(uint8_t i=0; i<offset;i++)
  {
    display_sendCommand(0x18);
  }
}

void display_printf(const char* str, ...)
{
  char stringArray[20];
  va_list args;
  va_start(args, str);
  vsprintf(stringArray, str, args);
  va_end(args);
  for(uint8_t i=0;  i<strlen(stringArray) && i<16; i++)
  {
    display_sendData((uint8_t)stringArray[i]);
  }
}
