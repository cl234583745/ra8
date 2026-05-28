/*
 * log.h
 *
 *  Created on: 2026年2月12日
 *      Author: Jerry.Chen
 */
#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdbool.h>

#ifndef LOG_USE
#error "Missing LOG_USE. Define LOG_USE before #include \"log.h\""
#endif

/* 使用方式（所有用法都需要 LOG_USE 在 #include 之前）：

   使用全局默认级别：
       #define LOG_USE
       #include "log.h"

   自定义当前文件级别：
       #define CURRENT_LOG_LEVEL LOG_LEVEL_DEBUG
       #define LOG_USE
       #include "log.h"

   错误示例（编译报错，不会静默失效）：
       #include "log.h"                              // LOG_USE 未定义 → #error
       #define LOG_USE                                // 太晚了！
       #define CURRENT_LOG_LEVEL LOG_LEVEL_DEBUG

   LOG_RAW 始终可用，不受级别控制。
*/

/*
 * 打印方式选择: 0=UART, 1=RTT
 * 只能选择一种方式
 */
#define PRINTF_METHOD  0

#if (PRINTF_METHOD == 0)
    #define USE_UART_PRINT      1
    #define USE_RTT_PRINT       0
#elif (PRINTF_METHOD == 1)
    #define USE_UART_PRINT      0
    #define USE_RTT_PRINT       1
#endif


#if USE_RTT_PRINT
#define SEGGER_INDEX (0)
#include "SEGGER_RTT/SEGGER_RTT.h"
    #define printf(...) SEGGER_RTT_printf(SEGGER_INDEX, __VA_ARGS__)
#endif//#if USE_RTT_PRINT


#ifndef PRINTF
#define PRINTF 1
#endif


#if USE_UART_PRINT
/* 0: printf -> newlib -> _write (full format support, needs -u _printf_float)
 * 1: printf -> uart_printf -> vsnprintf -> UART (lightweight, fixed buffer 256) */
#ifndef USE_UART_PRINTF_REDIRECT
#define USE_UART_PRINTF_REDIRECT 1
#endif

#if USE_UART_PRINTF_REDIRECT
#define printf(...) uart_printf(__VA_ARGS__)
#endif

extern void uart_printf(const char *fmt, ...);
#endif//#if USE_UART_PRINT

// ---------- 日志级别定义 ----------
#define LOG_LEVEL_NONE  0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_INFO  3
#define LOG_LEVEL_DEBUG 4
#define LOG_LEVEL_TRACE 5

// ---------- 全局总开关（默认 DEBUG） ----------
#ifndef GLOBAL_LOG_LEVEL
#define GLOBAL_LOG_LEVEL LOG_LEVEL_DEBUG
#endif

// ---------- 当前文件级别（必须在 #include 前定义，未定义时 = GLOBAL） ----------
#ifndef CURRENT_LOG_LEVEL
#define CURRENT_LOG_LEVEL GLOBAL_LOG_LEVEL
#elif CURRENT_LOG_LEVEL > GLOBAL_LOG_LEVEL
#error "CURRENT_LOG_LEVEL cannot exceed GLOBAL_LOG_LEVEL"
#endif

// ---------- 各个日志级别的宏定义（编译时决定） ----------

#if CURRENT_LOG_LEVEL >= LOG_LEVEL_ERROR
    #define LOG_ERROR(...) printf("[ERROR] " __VA_ARGS__ )
#else
    #define LOG_ERROR(...) ((void)0)
#endif

#if CURRENT_LOG_LEVEL >= LOG_LEVEL_WARN
    #define LOG_WARN(...)  printf("[WARN] "  __VA_ARGS__ )
#else
    #define LOG_WARN(...)  ((void)0)
#endif

#if CURRENT_LOG_LEVEL >= LOG_LEVEL_INFO
    #define LOG_INFO(...)  printf("[INFO] "  __VA_ARGS__ )
#else
    #define LOG_INFO(...)  ((void)0)
#endif

#if CURRENT_LOG_LEVEL >= LOG_LEVEL_DEBUG
    #define LOG_DEBUG(...) printf("[DEBUG] " __VA_ARGS__ )
#else
    #define LOG_DEBUG(...) ((void)0)
#endif

#if CURRENT_LOG_LEVEL >= LOG_LEVEL_TRACE
    #define LOG_TRACE(...) printf("[TRACE] " __VA_ARGS__ )
#else
    #define LOG_TRACE(...) ((void)0)
#endif

// ---------- 无前缀原始输出（不受级别控制，始终可用） ----------
#define LOG_RAW(...) printf(__VA_ARGS__)

#undef LOG_USE

#endif // LOG_H

