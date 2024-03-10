
#include <stdint.h>
#include <stdio.h>
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

  HAL_FLASH_Lock();

  st = HAL_FLASHEx_Erase(&eraseinitstruct, &sectorerror);

  HAL_FLASH_Unlock();

  return st;
}

/**
 * @brief  Memory write routine.
 * @param  src: Pointer to the source buffer. Address to be written to.
 * @param  dest: Pointer to the destination buffer.
 * @param  Len: Number of data to be written (in bytes).
 * @retval USBD_OK if operation is successful, MAL_FAIL else.
 */
HAL_StatusTypeDef flash_write(uint8_t *src, uint32_t dest, uint32_t Len)
{
  uint32_t i = 0;
  HAL_StatusTypeDef st;

  HAL_FLASH_Lock();
  for (i = 0; i < Len; i += 32) {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
     * be done by byte */

    st = HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, (uint32_t)(dest + i), (uint32_t)(src + i));

    if (st == HAL_OK) {
      /* Check the written value */
      if (*(uint64_t *)(src + i) != *(uint64_t *)(dest + i)) {
        /* Flash content doesn't match SRAM content */
        HAL_FLASH_Unlock();
        return HAL_ERROR;
      }
    } else {
      /* Error occurred while writing data in Flash memory */
      HAL_FLASH_Unlock();
      return st;
    }
  }
  HAL_FLASH_Unlock();

  return (0);
}
