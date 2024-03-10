#ifndef INC_UART_H_
#define INC_UART_H_


void uart_init(UART_HandleTypeDef *huart);
int uart_get_char();
int uart_put_char(char c);
int uart_put_buf(uint8_t *buf, int len);

#endif /* INC_UART_H_ */
