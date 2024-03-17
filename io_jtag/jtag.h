#ifndef __JTAG_H__
#define __JTAG_H__

#include <stdint.h>

#define JTAG_OK 0
#define JTAG_ERR 1
#define ERR_JTAG_SHIFT_BAD_FINAL_STATE 1

typedef enum jtag_state_e {
  JTAG_RESET = 0,
  JTAG_IDLE = 1,
  JTAG_DRSELECT = 2,
  JTAG_DRCAPTURE = 3,
  JTAG_DRSHIFT = 4,
  JTAG_DREXIT1 = 5,
  JTAG_DRPAUSE = 6,
  JTAG_DREXIT2 = 7,
  JTAG_DRUPDATE = 8,
  JTAG_IRSELECT = 9,
  JTAG_IRCAPTURE = 10,
  JTAG_IRSHIFT = 11,
  JTAG_IREXIT1 = 12,
  JTAG_IRPAUSE = 13,
  JTAG_IREXIT2 = 14,
  JTAG_IRUPDATE = 15,
  JTAG_UNKNOWN_STATE = 255
}jtag_state;

// int (*init)(jtag_tap_t *jtag, int param_len, ...);
// void (*deinit)(jtag_tap_t *jtag); // free malloc resource
// int (*shift)(jtag_tap_t *jtag, uint8_t *tdi, uint8_t *tdo, uint32_t nbits);
// int (*state)(jtag_tap_t *jtag, jtag_state sts, uint8_t *tdi, uint8_t *tdo);
// void (*frequency)(jtag_tap_t *jtag, jtag_freq_e hz);

typedef void (*jtagen_set)(int value);
typedef void (*tck_set)(int value);
typedef void (*tms_set)(int value);
typedef void (*tdi_set)(int value);
typedef int (*tdo_get)(void);
typedef void (*wait_100ns)(int count);

typedef struct _jtag_port {
  jtagen_set jtagen;
  tck_set tck;
  tms_set tms;
  tdi_set tdi;
  tdo_get tdo;
  wait_100ns wait;
  int freq_hz; // tck freq
} jtag_port;

int io_jtag_init(jtag_port *port);
int io_jtag_deinit(jtag_port *port);
int io_jtag_state(jtag_port *port, jtag_state sts, uint8_t tdi, uint8_t *tdo);
int io_jtag_runtest(jtag_port *port, jtag_state run_st, jtag_state end_st, uint32_t count);
int io_jtag_shift(jtag_port *port, uint8_t *tdi, uint8_t *tdo, uint16_t nbits, jtag_state run_st, jtag_state end_st);
int io_jtag_scan(jtag_port *port, uint32_t *id, int count);

#endif