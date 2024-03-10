#include <stdint.h>
#include <stdio.h>

#include "tusb.h"

#define usb_stdio_if 0

int usb_get_char()
{
  char c;
  if (tud_ready() && tud_cdc_n_available(usb_stdio_if)) {
    tud_cdc_n_read(usb_stdio_if, &c, 1);
    return (int)c;
  }
  return -1;
}

int usb_put_char(char c)
{
  if (tud_ready()) {
    tud_cdc_n_write_char(usb_stdio_if, c);
    tud_cdc_n_write_flush(usb_stdio_if);
    return 1;
  }
  return -1;
}

int usb_put_buf(uint8_t *buf, int len)
{
  if (tud_ready()) {
    tud_cdc_n_write(usb_stdio_if, buf, len);
    tud_cdc_n_write_flush(usb_stdio_if);
    return len;
  }
  return -1;
}

int __io_putchar(int ch)
{
  usb_put_char(ch);
  return ch;
}

int __io_getchar(void)
{
  return usb_get_char();
}

int _write(int file, char *ptr, int len)
{
  usb_put_buf((uint8_t *)ptr, len);
  return len;
}

int _read(int file, char *ptr, int len)
{
  int n = 0;
  while (n < len) {
    if (tud_ready() && tud_cdc_n_available(usb_stdio_if)) {
      n += tud_cdc_n_read(usb_stdio_if, (uint8_t *)ptr + n, len - n);
    }
  }
  return len;
}