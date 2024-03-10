
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "pico_cli.h"

int pico_process_cmd(const char *cmd)
{
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