#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include "console.h"

// debug Macro is DEBUG
#ifdef DEBUG
#define __DEBUG__ 1
#else
#define __DEBUG__ 0
#endif

static bool echo = true;       // 是否支持回显
#define CMD_LINE_LENGTH (1024) /* Change if you need */
#define HISTORY_LENGHT 4

#define CLK_PROMT "$>"

// unsigned char g_history_cli[CMD_LINE_LENGTH+1] = {0};

// Binary Protocol
// STX + LEN(DATA) + DATA
// LEN = 0, data length is 256

#define STX_TIMEOUT_US (50) // 50ms

#define STX 2  /* ASCII <STX> */
#define ETX 3  /* ASCII <ETX> */
#define EOT 4  /* ASCII <EOT> */
#define ACK 6  /* ASCII <ACK> */
#define NAK 21 /* ASCII <NAK> */

#define In_DELETE 0x7F /* ASCII <DEL> */
#define In_BACK 0x08   /* ASCII <Back Space> */
#define In_EOL '\r'    /* ASCII <CR> 0x0D*/
// #define In_EOL '\n'    /* ASCII <LF> 0x0A*/
#define In_SKIP '\3'   /* ASCII control-C */
#define In_EOF '\x1A'  /* ASCII control-Z */

#define Out_EOL In_EOL

// 1b5b41 up arrow
// 1b5b42 down arrow
// 1b5b44 left arrow
// 1b5b43 right arrow

#define Out_DELETE "\x8 \x8" /* VT100 backspace and clear */
#define Out_SKIP "^C\n"      /* ^C and new line */
#define Out_EOF "^Z"         /* ^Z and return EOF */
#define Out_CLRLINE "\033[K"

#define ESC '\x1b'
#define In_Up "\x1b[A"
#define In_Down "\x1b[B"

static char input_line_buf[CMD_LINE_LENGTH + 1];
static char output_line_buf[CMD_LINE_LENGTH + 1];
static char history_cmd[HISTORY_LENGHT][CMD_LINE_LENGTH + 1];
static int current_cmd_index = 0;
static int history_cmd_index = 0;
static bool history_cmd_inited = false;

#define CTRL_BUF_LEN 8
static char ctrl_buf[CTRL_BUF_LEN + 1];

static f_get_char _getchar = NULL;
static f_put_char _putchar = NULL;
static f_put_buf _putbuf = NULL;
static f_get_tick_ms _get_tick_ms = NULL;
static int _log_verbose = 0;

void console_init(f_get_char get_char, f_put_char put_char, f_put_buf put_buf, f_get_tick_ms get_tick_ms)
{
  _getchar = get_char;
  _putchar = put_char;
  _putbuf = put_buf;
  _get_tick_ms = get_tick_ms;
}

void console_set_log_level(int level)
{
  _log_verbose = level;
}

void cprintf(const char *format, ...)
{
  if (_putbuf) {
    va_list args;
    va_start(args, format);
    int n = vsnprintf(output_line_buf, CMD_LINE_LENGTH, format, args);
    va_end(args);
    _putbuf((uint8_t *)output_line_buf, n);
  }
}

void cinfo(const char *format, ...)
{
  if (_putbuf && _log_verbose >= LOG_INFO) {
    va_list args;
    va_start(args, format);
    int n = vsnprintf(output_line_buf, CMD_LINE_LENGTH, format, args);
    va_end(args);
    _putbuf((uint8_t *)output_line_buf, n);
  }
}

void cdebugf(const char *format, ...)
{
  if (_putbuf && _log_verbose >= LOG_DEBUG) {
    va_list args;
    va_start(args, format);
    int n = vsnprintf(output_line_buf, CMD_LINE_LENGTH, format, args);
    va_end(args);
    _putbuf((uint8_t *)output_line_buf, n);
  }
}

void ctrace(const char *format, ...)
{
  if (_putbuf && _log_verbose >= LOG_TRACE) {
    va_list args;
    va_start(args, format);
    int n = vsnprintf(output_line_buf, CMD_LINE_LENGTH, format, args);
    va_end(args);
    _putbuf((uint8_t *)output_line_buf, n);
  }
}

void reset_history_cmd_ptr();

char *get_history_cmd(bool up)
{
  char *cmd = NULL;

  if (!history_cmd_inited) {
    for (int i = 0; i < HISTORY_LENGHT; i++) {
      history_cmd[i][0] = 0;
    }
    history_cmd_inited = true;
    return NULL;
  }

  if (current_cmd_index == 0)
    return NULL;

  if (up) {
    history_cmd_index--;
    if (history_cmd_index >= 0) {
      cmd = history_cmd[history_cmd_index];
    } else {
      history_cmd_index = -1;
    }
  } else {
    history_cmd_index++;
    if (history_cmd_index < current_cmd_index) {
      cmd = history_cmd[history_cmd_index];
    } else {
      history_cmd_index = current_cmd_index;
    }
  }

  return cmd;
}

void set_history_cmd(char *cmd)
{
  if (!history_cmd_inited) {
    for (int i = 0; i < HISTORY_LENGHT; i++) {
      history_cmd[i][0] = 0;
    }
    history_cmd_inited = true;
  }

  if (current_cmd_index == HISTORY_LENGHT) {
    // move cmd history
    for (int i = 1; i < HISTORY_LENGHT; i++) {
      strcpy(history_cmd[i - 1], history_cmd[i]);
    }
    current_cmd_index--;
  }

  strcpy(history_cmd[current_cmd_index], cmd);
  for (int i = 0; i < CMD_LINE_LENGTH; i++) {
    if (history_cmd[current_cmd_index][i] == '\r' || history_cmd[current_cmd_index][i] == '\n') {
      history_cmd[current_cmd_index][i] = 0;
    }
  }
  current_cmd_index++;
  reset_history_cmd_ptr();
}

void reset_history_cmd_ptr()
{
  history_cmd_index = current_cmd_index;
}

void echo_onoff(int on)
{
  if(on) echo = true;
  else echo = false;
}

char *check_command()
{
  static int cmd_ptr = 0; /* Pointer in buffer */
  static int ctl_ptr = 0;
  static uint64_t start_ms = 0;
  static int len = 0;

  char c = 0;
  int getc = c;

  if (_getchar == NULL)
    return 0;
  if (_putchar == NULL)
    return 0;
  if (_putbuf == NULL)
    return 0;
  if (_get_tick_ms == NULL)
    return 0;

  while (true) {
    // Return a character from stdin if there is one available within a timeout.
    // timeout_us the timeout in microseconds, or 0 to not wait for a character if none available.
    // the character from 0-255 or PICO_ERROR_TIMEOUT if timeout occurs
    getc = _getchar();
    if (getc <= 0 || getc > 255)
      break;
    char c = getc;

    if (c == ESC) {
      start_ms = _get_tick_ms();
      ctrl_buf[ctl_ptr++] = c;
      continue;
    }

    if (ctl_ptr) {
      if (_get_tick_ms() - start_ms > 200) { // 200ms timeout
        ctl_ptr = 0;
      } else {
        ctrl_buf[ctl_ptr++] = c;
        if (ctl_ptr >= CTRL_BUF_LEN) {
          ctl_ptr = 0;
          continue;
        }
        ctrl_buf[ctl_ptr] = 0;
        if (strstr(ctrl_buf, In_Up)) {
          cprintf("\x1b[2K\x1b[80D");
          char *cmd = get_history_cmd(true);
          if (cmd != NULL) {
            cprintf("$>%s", cmd);
            strcpy(input_line_buf, cmd);
            cmd_ptr = strlen(input_line_buf);
          } else {
            cprintf(CLK_PROMT);
            cmd_ptr = 0;
          }
          ctl_ptr = 0;
        }
        if (strstr(ctrl_buf, In_Down)) {
          cprintf("\x1b[2K\x1b[80D");
          char *cmd = get_history_cmd(false);
          if (cmd != NULL) {
            cprintf("$>%s", cmd);
            strcpy(input_line_buf, cmd);
            cmd_ptr = strlen(input_line_buf);
          } else {
            cprintf(CLK_PROMT);
            cmd_ptr = 0;
          }
          ctl_ptr = 0;
        }
        continue;
      }
    }

    if (c == STX) {
      input_line_buf[cmd_ptr++] = c;
      start_ms = _get_tick_ms();
      len = -1;
      while ((_get_tick_ms() - start_ms) < STX_TIMEOUT_US) {
        getc = _getchar();
        if (getc < 0 || getc > 255)
          continue;
        c = getc;
        if (cmd_ptr < CMD_LINE_LENGTH) {
          input_line_buf[cmd_ptr++] = c;
        }
        if (len == -1) {
          len = c;
          if (len == 0)
            len = 256;
          continue;
        }
        len--;
        if (len == 0) {
          input_line_buf[cmd_ptr] = 0;
          cmd_ptr = 0;
          return input_line_buf;
        }
      }
      cmd_ptr = 0;
      return 0;
    }

    if (c == In_EOF && !cmd_ptr) {
      cprintf(Out_EOF);
      continue;
    }

    if (c == In_DELETE || c == In_BACK) {
      if (cmd_ptr) {
        cmd_ptr--;
        cprintf(Out_DELETE);
      }
    } else if (c == In_EOL) {
      _putchar(input_line_buf[cmd_ptr++] = Out_EOL);
      input_line_buf[cmd_ptr] = 0;
      reset_history_cmd_ptr();
      if (cmd_ptr > 1)
        set_history_cmd(input_line_buf);
      cmd_ptr = 0;
      return input_line_buf;
    } else if (cmd_ptr < CMD_LINE_LENGTH) {
      if (c >= ' ') {
        if (echo)
          _putchar(input_line_buf[cmd_ptr++] = c);
        else
          input_line_buf[cmd_ptr++] = c;
      }
    } else {
      if (echo)
        _putchar('\7');
    }
  }

  return 0; /* no command*/
}
