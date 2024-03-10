#ifndef __I2C_H__
#define __I2C_H__

#include <stdbool.h>

int i2c_init(I2C_HandleTypeDef *i2c, int speed_khz);
int i2c_write_blocking(I2C_HandleTypeDef *i2c, uint8_t addr, const uint8_t *src, size_t len, bool nostop);
int i2c_read_blocking(I2C_HandleTypeDef *i2c, uint8_t addr, uint8_t *dst, size_t len, bool nostop);

#endif