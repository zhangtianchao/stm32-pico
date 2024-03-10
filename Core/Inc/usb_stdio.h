#ifndef __USB_STDIO_H
#define __USB_STDIO_H

int usb_get_char();
int usb_put_char(char c);
int usb_put_buf(uint8_t *buf, int len);

#endif