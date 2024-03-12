
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"

#include "flash.h"

int flash_is_sector_start_address(uint32_t addr)
{
  if (addr > (FLASH_BASE + FLASH_BANK_SIZE)) {
    return 0;
  } else {
    return (addr % FLASH_SECTOR_SIZE == 0) ? 1 : 0;
  }
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */
/**
 * @brief  Gets the sector of a given address
 * @param  Address Address of the FLASH Memory
 * @retval The sector of a given address
 */
static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;

  if (Address < (FLASH_BASE + FLASH_BANK_SIZE)) {
    sector = (Address - FLASH_BASE) / FLASH_SECTOR_SIZE;
  } else {
    sector = (Address - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_SECTOR_SIZE;
  }

  return sector;
}

/**
 * @brief Erase sector.
 *
 * @param addr Address of sector to be erased.
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef flash_erase_sector(uint32_t addr)
{
  uint32_t startsector = 0, sectorerror = 0;
  /* Variable contains Flash operation status */
  FLASH_EraseInitTypeDef eraseinitstruct;

  HAL_StatusTypeDef st;

  /* Get the number of sector */
  startsector = GetSector(addr);
  eraseinitstruct.TypeErase = FLASH_TYPEERASE_SECTORS;
  eraseinitstruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
  eraseinitstruct.Banks = FLASH_BANK_1;
  eraseinitstruct.Sector = startsector;
  eraseinitstruct.NbSectors = 1;

  __disable_irq();
  HAL_FLASH_Unlock();

  st = HAL_FLASHEx_Erase(&eraseinitstruct, &sectorerror);

  HAL_FLASH_Lock();
  __enable_irq();

  return st;
}

/**
 * 目前算法是OK，但是重复写入后可能会导致写入程序挂起TODO:需要追踪底层HAL_FLASH_Program的bug
 * @brief  Memory write routine.
 * @param  src: Pointer to the source buffer. Address to be written to.
 * @param  dest: Pointer to the destination buffer. 虽然参数可以不对齐32字节，但是写入错误
 * @param  Len: Number of data to be written (in bytes).
 * @retval USBD_OK if operation is successful, MAL_FAIL else.
 */
HAL_StatusTypeDef flash_write(uint8_t *src, uint32_t dest, uint32_t len)
{
  uint32_t i = 0, j = 0;
  uint32_t ilen = 0;
  HAL_StatusTypeDef st;
  static uint8_t ibuf[32];

  uint32_t si = 0;
  for (i = dest; i < dest + len; i += ilen) {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
     * be done by byte */
    memset(ibuf, 0xff, 32);
    ilen = 32 - (i % 32);
    if (ilen > len - si) {
      ilen = len - si;
    }
    memcpy(ibuf + (i % 32), src + si, ilen);

    __disable_irq();
    HAL_FLASH_Unlock();
    st = HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, (uint32_t)(i & 0xffffffe0), (uint32_t)(ibuf));
    HAL_FLASH_Lock();
    __enable_irq();

    if (st == HAL_OK) {
      /* Check the written value */
      for (j = 0; j < ilen; j++) {
        if (*(uint8_t *)(src + si + j) != *(uint8_t *)(i + j)) {
          /* Flash content doesn't match SRAM content */
          return HAL_ERROR;
        }
      }
    } else {
      /* Error occurred while writing data in Flash memory */
      return st;
    }

    si += ilen;
  }

  return (0);
}
