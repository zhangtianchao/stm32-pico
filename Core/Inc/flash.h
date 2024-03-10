#ifndef __FLASH_H
#define __FLASH_H

int flash_is_sector_start_address(uint32_t addr);
HAL_StatusTypeDef flash_erase_sector(uint32_t addr);
HAL_StatusTypeDef flash_write(uint8_t *src, uint32_t dest, uint32_t Len);

#endif