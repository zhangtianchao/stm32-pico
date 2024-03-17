
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"

#define CPU_FREQ 400000000

// TDI = PA3
// TDO = PA0
// TCK = PA4
// TMS = PA5
// JTAGEN = PA6
#define TDO_Pin GPIO_PIN_0
#define TDO_GPIO_Port GPIOA
#define TDI_Pin GPIO_PIN_3
#define TDI_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_4
#define TCK_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_5
#define TMS_GPIO_Port GPIOA
#define JTAGEN_Pin GPIO_PIN_6
#define JTAGEN_GPIO_Port GPIOA

void hal_io_jtagen_set(int value)
{
  HAL_GPIO_WritePin(JTAGEN_GPIO_Port, JTAGEN_Pin, value);
}

void hal_io_tck_set(int value)
{
  HAL_GPIO_WritePin(TCK_GPIO_Port, TCK_Pin, value);
}

void hal_io_tms_set(int value)
{
  HAL_GPIO_WritePin(TMS_GPIO_Port, TMS_Pin, value);
}

void hal_io_tdi_set(int value)
{
  HAL_GPIO_WritePin(TDI_GPIO_Port, TDI_Pin, value);
}

int hal_io_tdo_get(void)
{
  return HAL_GPIO_ReadPin(TDO_GPIO_Port, TDO_Pin);
}

void hal_io_wait_100ns(int count)
{
#if CPU_FREQ == 400000000

  count -= 3;
  if (count < 0) return;

  cpu_delay(CPU_FREQ / 10000000 * count);
  return;
#endif

  uint32_t cpufreq = HAL_RCC_GetSysClockFreq();
  cpu_delay(cpufreq / 10000000 * count);
}