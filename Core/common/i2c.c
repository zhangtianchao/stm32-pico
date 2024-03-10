#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "console.h"
#include <stm32h7xx_hal_i2c.h>
#include "i2c.h"

int i2c_init(I2C_HandleTypeDef *i2c, int speed_khz)
{
  return HAL_I2C_Init(i2c);
}

int i2c_write_blocking(I2C_HandleTypeDef *i2c, uint8_t addr, const uint8_t *src, size_t len, bool nostop)
{
  // return HAL_I2C_Master_Seq_Transmit_IT(i2c, addr, src, len, nostop ? I2C_FIRST_FRAME : I2C_FIRST_AND_LAST_FRAME);
  uint8_t *buf = (uint8_t *)src;
  return HAL_I2C_Master_Transmit(i2c, addr<<1, buf, len, 100);
}

int i2c_read_blocking(I2C_HandleTypeDef *i2c, uint8_t addr, uint8_t *dst, size_t len, bool nostop)
{
  // return HAL_I2C_Master_Seq_Receive_IT(i2c, addr, dst, len, nostop ? I2C_FIRST_FRAME : I2C_FIRST_AND_LAST_FRAME);
  return HAL_I2C_Master_Receive(i2c, addr<<1, dst, len, 100);
}
