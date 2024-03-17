
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "main.h"
#include "console.h"
#include "flash.h"
#include "io_jtag_hal.h"
#include "jtag.h"
#include "pico_cli.h"

extern void msc_dump(int sector);

int flash_cmd(const char *cmd)
{
  char *pcmd = 0;
  uint32_t addr = 0;
  uint32_t i, len = 0;
  uint8_t buf[128];
  int ret;

  if ((pcmd = strstr(cmd, "-h"))) {
    cprintf("flash -> flash command\n");
    cprintf("  -h :show this help message\n");
    cprintf("  erase : erase flash sector, arg is flash address\n");
    cprintf("  read : read flash, arg is flash address\n");
    cprintf("  write : write flash, arg is flash address\n");
    cprintf("  len : read data length\n");
    cprintf("  data : data to write\n");
    return 0;
  }

  if ((pcmd = strstr(cmd, "len"))) {
    len = strtoul(pcmd + 4, NULL, 0);
  }

  if ((pcmd = strstr(cmd, "data"))) {
    pcmd = strtok(pcmd + 4, ",");
    while (pcmd != NULL) {
      buf[len++] = strtoul(pcmd, NULL, 0);
      if (len >= 128) {
        cprintf("data length should be < 128\n");
        return -1;
      }
      pcmd = strtok(NULL, ",");
    }
  }

  if ((pcmd = strstr(cmd, "erase"))) {
    addr = strtoul(pcmd + 5, NULL, 0);
    ret = flash_erase_sector(addr);
    cprintf("erase flash @%08lX ret = %lu\n", addr, ret);
    return ret;
  }

  if ((pcmd = strstr(cmd, "read"))) {
    addr = strtoul(pcmd + 4, NULL, 0);
    for (i = 0; i < len; i++) {
      cprintf("%02x ", *(uint8_t *)(addr + i));
    }
    cprintf("\n");
    return 0;
  }

  if ((pcmd = strstr(cmd, "write"))) {
    addr = strtoul(pcmd + 5, NULL, 0);
    cprintf("write flash @%08lX\n", addr);
    ret = flash_write(buf, addr, len);
    return ret;
  }

  return 0;
}

int jtag_cmd(const char *cmd)
{
  char *pcmd = 0;
  uint8_t buf[128];

  jtag_port port;
  port.jtagen = hal_io_jtagen_set;
  port.tck = hal_io_tck_set;
  port.tms = hal_io_tms_set;
  port.tdi = hal_io_tdi_set;
  port.tdo = hal_io_tdo_get;
  port.wait = hal_io_wait_100ns;
  port.freq_hz = 1000000;

  if ((pcmd = strstr(cmd, "speed"))) {
    port.freq_hz = strtoul(pcmd + 6, NULL, 0);
  }

  if ((pcmd = strstr(cmd, "test"))) {
    printf("run jtag test :\n");
    cprintf("init: %d\n", io_jtag_init(&port));
    printf("state to JTAG_RESET: %d\n", io_jtag_state(&port, JTAG_RESET, 0, NULL));
    printf("state to JTAG_DRSHIFT: %d\n", io_jtag_state(&port, JTAG_DRSHIFT, 0, NULL));
    printf("state to JTAG_IRSHIFT: %d\n", io_jtag_state(&port, JTAG_IRSHIFT, 0, NULL));
    printf("state to JTAG_DRPAUSE: %d\n", io_jtag_state(&port, JTAG_DRPAUSE, 0, NULL));
    printf("state to JTAG_IRPAUSE: %d\n", io_jtag_state(&port, JTAG_IRPAUSE, 0, NULL));
    printf("state to JTAG_IDLE: %d\n", io_jtag_state(&port, JTAG_IDLE, 0, NULL));
    sleep_ms(1);
    uint8_t tdi_buf[64];
    uint8_t tdo_buf[64];
    memset(tdi_buf, 0x55, 16);
    memset(tdi_buf + 16, 0x11, 16);
    memset(tdi_buf + 32, 0xaa, 16);
    memset(tdi_buf + 48, 0x22, 16);
    printf("shift DR 7bit, final JTAG_DRPAUSE : %d\n", io_jtag_shift(&port, tdi_buf, tdo_buf, 7, JTAG_DRSHIFT, JTAG_DRPAUSE));
    printf("shift DR 7bit, final JTAG_DRSHIFT : %d\n", io_jtag_shift(&port, tdi_buf, tdo_buf, 7, JTAG_DRSHIFT, JTAG_DRSHIFT));
    printf("state to JTAG_IDLE: %d\n", io_jtag_state(&port, JTAG_IDLE, 0, NULL));
    printf("shift DR 7bit, final JTAG_DRSHIFT : %d\n", io_jtag_shift(&port, tdi_buf, tdo_buf, 7, JTAG_DRSHIFT, JTAG_DRSHIFT));
    printf("shift DR 7bit, final JTAG_DRPAUSE : %d\n", io_jtag_shift(&port, tdi_buf, tdo_buf, 7, JTAG_DRSHIFT, JTAG_DRPAUSE));
    printf("shift DR 250bit, final JTAG_DRPAUSE : %d\n", io_jtag_shift(&port, tdi_buf, tdo_buf, 250, JTAG_DRSHIFT, JTAG_DRPAUSE));
    printf("shift DR 420bit, final JTAG_DRPAUSE : %d\n", io_jtag_shift(&port, tdi_buf, tdo_buf, 420, JTAG_DRSHIFT, JTAG_DRPAUSE));
    printf("shift DR 7bit, final JTAG_IRPAUSE : %d\n", io_jtag_shift(&port, tdi_buf, tdo_buf, 7, JTAG_IRSHIFT, JTAG_IRPAUSE));
    printf("shift DR 7bit, final JTAG_IRSHIFT : %d\n", io_jtag_shift(&port, tdi_buf, tdo_buf, 7, JTAG_IRSHIFT, JTAG_IRSHIFT));
    printf("state to JTAG_IDLE: %d\n", io_jtag_state(&port, JTAG_IDLE, 0, NULL));
    printf("shift DR 250bit, final JTAG_IRPAUSE : %d\n", io_jtag_shift(&port, tdi_buf, tdo_buf, 250, JTAG_IRSHIFT, JTAG_IRPAUSE));
    printf("shift DR 420bit, final JTAG_IRPAUSE : %d\n", io_jtag_shift(&port, tdi_buf, tdo_buf, 420, JTAG_IRSHIFT, JTAG_IRPAUSE));
    printf("done\n");
    return 0;
  }

  if ((pcmd = strstr(cmd, "scan"))) {
    cprintf("init: %d\n", io_jtag_init(&port));
    uint32_t *id = (uint32_t *)buf;
    io_jtag_scan(&port, id, 8);
    int i;
    for (i = 0; i < 8; i++) {
      cprintf("%08lX ", *(id + i));
    }
    cprintf("\n");
    return 0;
  }

  return 1;
}

int pico_process_cmd(const char *cmd)
{
  char *pcmd;

  if (strlen(cmd) < 3)
    return 255;

  if (strstr(cmd, "help")) {
    cprintf("command list:\n");
    cprintf("hello\n");
    cprintf("echo on|off\n");
    cprintf("flash read|write|erase\n");
    cprintf("mscdump [sector]\n");
    cprintf("rcc_rsr dump rcc rsr value\n");
    return 0;
  }

  if (strstr(cmd, "echo")) {
    echo_onoff(strstr(cmd, "on") ? 1 : 0);
    return 0;
  }

  if (strstr(cmd, "flash")) {
    return flash_cmd(cmd);
  }

  if ((pcmd = strstr(cmd, "mscdump"))) {
    int sector = strtoul(pcmd + 8, NULL, 0);
    msc_dump(sector);
    cprintf("\n");
    return 0;
  }

  if (strstr(cmd, "rcc_rsr")) {
    cprintf("rcc rsr = 0x%08lX\n", RCC->RSR);
    return 0;
  }

  if (strstr(cmd, "jtag")) {
    return jtag_cmd(cmd);
  }

  return 0;
}

// static EmbeddedCli *cli;

// static void onFlash(EmbeddedCli *cli, char *tokens, void *context);

// void pico_cli_init(EmbeddedCli *ecli)
// {
//   cli = ecli;
//   CliCommandBinding b = {
//       "flash",
//       "flash read|write|erase",
//       true,
//       NULL,
//       onFlash};

//   embeddedCliAddBinding(cli, b);
// }

// static void onFlash(EmbeddedCli *cli, char *tokens, void *context)
// {
//   // UNUSED(context);
//   // PREPARE_IMPL(cli);
//   int i;

//   uint16_t tokenCount = embeddedCliGetTokenCount(tokens);
//   for (i = 0; i < tokenCount; i++) {
//     printf("%s\n", embeddedCliGetToken(tokens, i+1));
//   }

//   return;
// }