#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#define LOG_INFO 1
#define LOG_DEBUG 2
#define LOG_TRACE 3

typedef int(*f_get_char)();
typedef int(*f_put_char)(char c);
typedef int(*f_put_buf)(uint8_t *buf, int len);
typedef uint64_t(*f_get_tick_ms)();

/**
 * @brief Initialize console
 * 
 * @param get_char 不能阻塞，如果没有数据则返回-1
 * @param put_char 
 * @param put_buf 
 */
void console_init(f_get_char get_char, f_put_char put_char, f_put_buf put_buf, f_get_tick_ms get_tick_ms);

/**
 * @brief Set log level
 * 
 * @param level 
 *          0: no log
 *          1: info
 *          2: debug
 *          3: trace
 */
void console_set_log_level(int level);
void cprintf(const char *format, ...);
void cinfo(const char *format, ...);
void cdebugf(const char *format, ...);
void ctrace(const char *format, ...);
void echo_onoff(int on);
char *check_command();

#endif