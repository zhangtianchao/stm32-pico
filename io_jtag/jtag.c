
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "jtag.h"

#ifndef __time_critical_func
#define __time_critical_func
#endif

// jtag state machine map for jtag state change
static const char *jtag_state_map[16][16] = {
    {"", "0", "01", "010", "0100", "0101", "01010", "010101", "01011", "011", "0110", "01100", "01101", "011010", "0110101", "011011"},
    {"111", "", "1", "10", "100", "101", "1010", "10101", "1011", "11", "110", "1100", "1101", "11010", "110101", "11011"},
    {"11", "110", "", "0", "00", "01", "010", "0101", "011", "1", "10", "100", "101", "1010", "10101", "1011"},
    {"11111", "110", "111", "", "0", "1", "10", "101", "11", "1111", "11110", "111100", "111101", "1111010", "11110101", "1111011"},
    {"11111", "110", "111", "1110", "", "1", "10", "101", "11", "1111", "11110", "111100", "111101", "1111010", "11110101", "1111011"},
    {"1111", "10", "11", "110", "010", "", "0", "01", "1", "111", "1110", "11100", "11101", "111010", "1110101", "111011"},
    {"11111", "110", "111", "1110", "10", "101", "", "1", "11", "1111", "11110", "111100", "111101", "1111010", "11110101", "1111011"},
    {"1111", "10", "11", "110", "0", "01", "010", "", "1", "111", "1110", "11100", "11101", "111010", "1110101", "111011"},
    {"111", "0", "1", "10", "100", "101", "1010", "10101", "", "11", "110", "1100", "1101", "11010", "110101", "11011"},
    {"1", "10", "101", "1010", "10100", "10101", "101010", "1010101", "101011", "", "0", "00", "01", "010", "0101", "011"},
    {"11111", "110", "111", "1110", "11100", "11101", "111010", "1110101", "111011", "1111", "", "0", "1", "10", "101", "11"},
    {"11111", "110", "111", "1110", "11100", "11101", "111010", "1110101", "111011", "1111", "11110", "", "1", "10", "101", "11"},
    {"1111", "10", "11", "110", "1100", "1101", "11010", "110101", "11011", "111", "1110", "010", "", "0", "01", "1"},
    {"11111", "110", "111", "1110", "11100", "11101", "111010", "1110101", "111011", "1111", "11110", "10", "101", "", "1", "11"},
    {"1111", "10", "11", "110", "1100", "1101", "11010", "110101", "11011", "111", "1110", "0", "01", "010", "", "1"},
    {"111", "0", "1", "10", "100", "101", "1010", "10101", "1011", "11", "110", "1100", "1101", "11010", "110101", ""}};

// local jtag state
static jtag_state _io_jtag_current_state = JTAG_UNKNOWN_STATE;

static void _jtagen_set(int en)
{
}

static void _wait_100ns(int count)
{
}

int io_jtag_init(jtag_port *port)
{
  if (port->jtagen == NULL) {
    port->jtagen = _jtagen_set;
  }

  if (port->tck == NULL) {
    return JTAG_ERR;
  }

  if (port->tms == NULL) {
    return JTAG_ERR;
  }

  if (port->tdi == NULL) {
    return JTAG_ERR;
  }

  if (port->tdo == NULL) {
    return JTAG_ERR;
  }

  if (port->wait == NULL) {
    port->wait = _wait_100ns;
  }

  port->jtagen(1);
  // set tck = L
  port->tck(0);
  // set tms = L
  port->tms(0);
  // set tdi = L
  port->tdi(0);
  // set tdo as input
  port->tdo();

  return JTAG_OK;
}

int io_jtag_deinit(jtag_port *port)
{
  port->jtagen(0);
  // set tck = L
  port->tck(0);
  // set tms = L
  port->tms(0);
  // set tdi = L
  port->tdi(0);
  // set tdo as input
  port->tdo();

  return JTAG_OK;
}

/**
 * @brief
 *
 * @param sts 目标状态机位置
 * @param tdi 切换状态机时的TDI数据
 * @param tdo 返回第一个状态机切换时的TDO
 * @return int
 */
int io_jtag_state(jtag_port *port, jtag_state sts, uint8_t tdi, uint8_t *tdo)
{
  if (_io_jtag_current_state == sts && sts != JTAG_RESET)
    return 0;
  // printf("io jtag state: %d\n", sts);
  uint8_t tms_buf[32] = {1};
  uint8_t _tdo;
  uint8_t *ptdo = tdo;
  int wait_count = 10000000 / port->freq_hz / 2;
  int i;
  memset(tms_buf, 1, 32);
  if (ptdo == NULL)
    ptdo = &_tdo;
  int tmslen = 0;
  if (_io_jtag_current_state == JTAG_UNKNOWN_STATE) {
    _io_jtag_current_state = JTAG_RESET;
    tmslen = 8;
  }
  if (sts == JTAG_RESET) {
    tmslen = 8;
  } else {
    const char *tmsstr = jtag_state_map[_io_jtag_current_state][sts];
    int len = strlen(tmsstr);
    for (i = 0; i < len; i++) {
      if (tmsstr[i] == '1') {
        tms_buf[tmslen] = 1;
      } else {
        tms_buf[tmslen] = 0;
      }
      tmslen++;
    }
  }
  // send tms
  for (i = 0; i < tmslen; i++) {
    port->tms(tms_buf[i]);  // gpio_put(jtag_inst.tms_pin, tms_buf[i]);
    port->tdi(tdi & 0x01);  // gpio_put(jtag_inst.tdi_pin, tdi & 0x01);
    port->wait(wait_count); // sleep_us(1);
    port->tck(1);           // gpio_put(jtag_inst.tck_pin, 1);
    if (i == 0)
      *ptdo = port->tdo();  // if (i == 0) *ptdo = gpio_get(jtag_inst.tdo_pin);
    port->wait(wait_count); // sleep_us(1);
    port->tck(0);           // gpio_put(jtag_inst.tck_pin, 0);
  }

  _io_jtag_current_state = sts;
  return 0;
}

/**
 * sts: final jtag state, normal is JTAG_RESET, JTAG_IDLE, JTAG_DRSHIFT, JTAG_DRPAUSE, JTAG_IRSHIFT, JTAG_IRPAUSE
 * @param mode
 *            1: DRSHIFT, auto change state to DRSHIFT
 *            2: IRSHIFT, auto change state to IRSHIFT
 *            3: DUMMYSHIFT, don't change state
 */

/**
 * @brief shift tdi/tdo
 *
 * @param tdi tdi buffer for shift data out
 * @param tdo tdo buffer for shift data in
 * @param nbits bits of shift
 * @param run_st the state shift run
 * @param end_st the state of shift end
 * @return int
 */
int __time_critical_func(io_jtag_shift)(jtag_port *port, uint8_t *tdi, uint8_t *tdo, uint16_t nbits, jtag_state run_st, jtag_state end_st)
{
  // printf("io jtag shift\n");
  if (nbits == 0) {
    // just change state to end state
    return io_jtag_state(port, end_st, 0, NULL);
  }
  if (run_st != JTAG_RESET &&
      run_st != JTAG_IDLE &&
      run_st != JTAG_DRSHIFT &&
      run_st != JTAG_DRPAUSE &&
      run_st != JTAG_IRSHIFT &&
      run_st != JTAG_IRPAUSE)
    return ERR_JTAG_SHIFT_BAD_FINAL_STATE;

  io_jtag_state(port, run_st, 0, NULL);

  int wait_count = 10000000 / port->freq_hz / 2;
  bool need_last_tdo = false;
  uint8_t last_tdi = 0;
  int tdooff = 7;
  int nbs = nbits / 8 + (nbits % 8 ? 1 : 0);
  if (run_st != end_st) {
    need_last_tdo = true;
    if (nbits % 8) {
      tdooff = nbits % 8 - 1;
    }
    last_tdi = tdi[nbs - 1] >> tdooff; // 最高位的tdi
    nbits = nbits - 1;
  }

  // uint8_t itdi = 0;
  // uint8_t itdo = 0;
  uint8_t *ptdi = tdi;
  uint8_t *ptdo = tdo;
  // if (ptdi == NULL)
  //   ptdi = &itdi;
  // if (ptdo == NULL)
  //   ptdo = &itdo;

  if (nbits > 0) {
    int i;
    int ibyte;
    int ibit;
    for (i = 0; i < nbits; i++) {
      ibyte = i / 8; // 当前的byte索引
      ibit = i % 8;  // 当前的bit位
      if (ibit == 0)
        *(ptdo + ibyte) = 0;
      // set tdi
      if (ptdi)
        port->tdi(((*(ptdi + ibyte)) & (1 << ibit)) ? 1 : 0);
      port->wait(wait_count);
      port->tck(1);
      // get tdo
      if (ptdo)
        *(ptdo + ibyte) |= ((port->tdo() & 0x01) << ibit);
      port->wait(wait_count);
      port->tck(0);
    }
  }

  if (need_last_tdo) {
    uint8_t itdo = 0;
    io_jtag_state(port, end_st, last_tdi, &itdo);
    tdo[nbs - 1] = tdo[nbs - 1] >> 1;
    tdo[nbs - 1] |= itdo << tdooff;
  } else {
    io_jtag_state(port, end_st, last_tdi, NULL);
  }

  return JTAG_OK;
}

int io_jtag_scan(jtag_port *port, uint32_t *id, int count)
{
  uint32_t tdi = 0;
  uint32_t tdo = 0;
  int i;
  io_jtag_state(port, JTAG_RESET, 0, NULL);
  for (i = 0; i < count; i++) {
    io_jtag_shift(port, (uint8_t *)&tdi, (uint8_t *)(&tdo), 32, JTAG_DRSHIFT, (i == (count - 1)) ? JTAG_RESET : JTAG_DRSHIFT);
    *(id + i) = tdo;
  }
  io_jtag_state(port, JTAG_RESET, 0, NULL);
  return 0;
}

int io_jtag_runtest(jtag_port *port, jtag_state run_st, jtag_state end_st, uint32_t count)
{
  return io_jtag_shift(port, NULL, NULL, count, run_st, end_st);
}
