/**
 * @file uart.c
 * @author 
 * @brief 只支持使用一个串口，不支持多串口
 *        必须要使能串口中断
 * @version 0.1
 * @date 2024-01-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "stm32h7xx.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"

#define UART_BUF_LEN 512
#define UART_BUF_WATER_LEN 256

typedef struct __UART_RxHandle {
  UART_HandleTypeDef *huart;
  uint8_t *head;
  uint8_t buf[UART_BUF_LEN];
} UART_Handle;

static UART_Handle uart_handle;

void UART_StartRx(UART_HandleTypeDef *huart);
int UART_GetChar(UART_HandleTypeDef *huart);
void UART_PutBuf(UART_HandleTypeDef *huart, uint8_t *buf, int len);
void UART_PutString(UART_HandleTypeDef *huart, char *str);
void UART_PutChar(UART_HandleTypeDef *huart, char c);
void UART_Printf(UART_HandleTypeDef *huart, const char *format, ...);
void UART_ClearRxBuf(UART_HandleTypeDef *huart);

void uart_init(UART_HandleTypeDef *huart)
{
  uart_handle.huart = huart;
  uart_handle.head = uart_handle.buf;
  UART_StartRx(huart);
}

int uart_get_char()
{
  return UART_GetChar(uart_handle.huart);
}

int uart_put_char(char c)
{
  UART_PutChar(uart_handle.huart, c);
  return 1;
}

int uart_put_buf(uint8_t *buf, int len)
{
  UART_PutBuf(uart_handle.huart, buf, len);
  return len;
}

/**
 * private function
 */


void UART_StartRx(UART_HandleTypeDef *huart)
{
  uart_handle.huart = huart;
  uart_handle.head = uart_handle.buf;

  HAL_UART_Receive_IT(huart, uart_handle.buf, UART_BUF_LEN);
}

int UART_GetChar(UART_HandleTypeDef *huart)
{
  UART_Handle *rxhandle;
  if (huart == uart_handle.huart) {
    rxhandle = &uart_handle;
    if (huart->ErrorCode != 0) {
      UART_StartRx(huart);
    }
  } else {
    return -1;
  }
  if (rxhandle->head == rxhandle->huart->pRxBuffPtr) {
    // no data
    if (rxhandle->huart->RxXferCount < UART_BUF_WATER_LEN) {
      if (rxhandle->huart->RxXferCount != 0) HAL_UART_AbortReceive_IT(rxhandle->huart);
      // rx buffer full
      HAL_UART_Receive_IT(rxhandle->huart, rxhandle->buf, UART_BUF_LEN);
      rxhandle->head = rxhandle->buf;
    }
    return -1;
  }
  uint8_t c = *(rxhandle->head);
  rxhandle->head++;
  return c;
}

void UART_PutBuf(UART_HandleTypeDef *huart, uint8_t *buf, int len)
{
  // HAL_UART_Transmit_IT(huart, buf, len);
  // while (huart->TxXferCount)
  //   ;
  // HAL_Delay(1);
  HAL_UART_Transmit(huart, buf, len, HAL_MAX_DELAY);
}

void UART_PutString(UART_HandleTypeDef *huart, char *str)
{
  // HAL_UART_Transmit_IT(huart, (uint8_t *)str, strlen(str));
  // while (huart->TxXferCount)
  //   ;
  // HAL_Delay(1);
  HAL_UART_Transmit(huart, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
}

void UART_PutChar(UART_HandleTypeDef *huart, char c)
{
  // HAL_UART_Transmit_IT(huart, (uint8_t *)&c, 1);
  // // HAL_UART_Transmit(huart, (uint8_t *)&c, 1, 0);
  // while (huart->TxXferCount)
  //   ;
  // HAL_Delay(1);
  HAL_UART_Transmit(huart, (uint8_t *)&c, 1, HAL_MAX_DELAY);
}

void UART_ClearRxBuf(UART_HandleTypeDef *huart)
{
  while (UART_GetChar(huart) >= 0)
    ;
}
