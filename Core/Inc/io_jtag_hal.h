#ifndef __IO_JTAG_HAL_H
#define __IO_JTAG_HAL_H

void hal_io_jtagen_set(int value);
void hal_io_tck_set(int value);
void hal_io_tms_set(int value);
void hal_io_tdi_set(int value);
int hal_io_tdo_get(void);
void hal_io_wait_100ns(int count);

#endif