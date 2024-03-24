#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "pico_reg.h"

pico_registers_st pico_reg;

int pico_reg_read(uint32_t addr, uint8_t *buf, uint32_t len)
{
  if (buf == NULL || len == 0) {
    return 0;
  }
  if ((addr + len) > sizeof(pico_registers_st)) {
    return -1;
  }

  memcpy(buf, ((uint8_t *)(&pico_reg)) + addr, len);

  return len;
}

int pico_reg_write(uint32_t addr, const uint8_t *buf, uint32_t len)
{
  if (buf == NULL || len == 0) {
    return 0;
  }
  if ((addr + len) > sizeof(pico_registers_st)) {
    return -1;
  }
  if (addr < ((uint32_t)pico_reg.common_buf - (uint32_t)(&pico_reg))) {
    // 前面的地址空间是只读的
    return -1;
  }

  memcpy(((uint8_t *)(&pico_reg)) + addr, buf, len);

  return len;
}