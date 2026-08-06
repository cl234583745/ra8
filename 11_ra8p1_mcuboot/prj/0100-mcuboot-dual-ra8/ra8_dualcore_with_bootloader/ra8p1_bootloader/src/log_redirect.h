#ifndef LOG_REDIRECT_H_
#define LOG_REDIRECT_H_

extern void uart_printf(const char *fmt, ...);

#define MCUBOOT_LOG_ERR(_fmt, ...)   uart_printf("[ERR] " _fmt "\n", ##__VA_ARGS__)
#define MCUBOOT_LOG_WRN(_fmt, ...)   uart_printf("[WRN] " _fmt "\n", ##__VA_ARGS__)
#define MCUBOOT_LOG_INF(_fmt, ...)   uart_printf("[INF] " _fmt "\n", ##__VA_ARGS__)
#define MCUBOOT_LOG_DBG(_fmt, ...)   uart_printf("[DBG] " _fmt "\n", ##__VA_ARGS__)

#endif
