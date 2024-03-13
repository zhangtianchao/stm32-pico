/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "console.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ToO_Pin GPIO_PIN_0
#define ToO_GPIO_Port GPIOA
#define PHY_RST_N_Pin GPIO_PIN_10
#define PHY_RST_N_GPIO_Port GPIOB
#define WS2812B_DI_Pin GPIO_PIN_14
#define WS2812B_DI_GPIO_Port GPIOB
#define MCU_LED_Pin GPIO_PIN_15
#define MCU_LED_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */
#define FLASH_START_ADDRESS 0x08000000
#define APP_BOOT_ADDRESS_PHY_ADDR 0x08020000
#define APP1_SECTOR_ADDRESS 0x08040000
#define APP2_SECTOR_ADDRESS 0x08100000
#define DATA_SECTOR_ADDRESS 0x081C0000
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
