
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "console.h"
#include "flash.h"
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

  if(strstr(cmd, "rsr")){
    cprintf("RCC->RSR = 0x%08lX\n", RCC->RSR);
  }

  return 0;
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