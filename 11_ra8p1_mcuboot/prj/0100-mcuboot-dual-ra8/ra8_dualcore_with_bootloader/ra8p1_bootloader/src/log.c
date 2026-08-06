#include "hal_data.h"
#define LOG_USE
#include "log.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#if USE_UART_PRINT
#define POLLING_REG     0//使用 fsp中断 或者 轮询寄存器

volatile bool uartTxCompleteFlg = 0;

void USR_SCI_UART_Write (uart_ctrl_t * const p_api_ctrl, uint8_t const * const p_src, uint32_t const bytes);

void uart_printf(const char *fmt, ...)
{
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
#if POLLING_REG
    USR_SCI_UART_Write(&g_uart8_ctrl, (uint8_t *)buf, strlen(buf));
#else
    uartTxCompleteFlg = 0;
    g_uart8.p_api->write(&g_uart8_ctrl, (uint8_t *)buf, strlen(buf));
    while(!uartTxCompleteFlg);
    uartTxCompleteFlg = 0;
#endif
}

void USR_SCI_UART_Write (uart_ctrl_t * const p_api_ctrl, uint8_t const * const p_src, uint32_t const bytes)
{
    uint32_t i;
#if defined(R_SCI_B_UART_CFG_H_)
    sci_b_uart_instance_ctrl_t * p_ctrl = (sci_b_uart_instance_ctrl_t *) p_api_ctrl;
#else
    sci_uart_instance_ctrl_t * p_ctrl = (sci_uart_instance_ctrl_t *) p_api_ctrl;
#endif
    uint8_t *data  = (uint8_t *)p_src;
    for(i = 0; i < bytes; i++)
    {
#if defined(R_SCI_B_UART_CFG_H_)
        p_ctrl->p_reg->TDR_b.TDAT = *data;
        while(p_ctrl->p_reg->CSR_b.TDRE == 0);
#else
#if defined(_RENESAS_RA_)
        p_ctrl->p_reg->TDR_b.TDR = *data;
        while(p_ctrl->p_reg->SSR_b.TDRE == 0);
#elif defined(_RENESAS_RZN_) || defined(_RENESAS_RZT_)
        p_ctrl->p_reg->TDR_b.TDAT = *data;
        while(p_ctrl->p_reg->CSR_b.TDRE == 0);
#endif
#endif
        data++;
    }
}
#if !defined __ICCARM__
int _write(int fd, char *pBuffer, int size);
int _write(int fd, char *pBuffer, int size)
{
    (void)fd;
#if POLLING_REG
    USR_SCI_UART_Write(&g_uart8_ctrl, (uint8_t *)pBuffer, (uint32_t)size);
#else
    uartTxCompleteFlg = 0;
    g_uart8.p_api->write(&g_uart8_ctrl, (uint8_t const *)pBuffer, (uint32_t)size);
    while(!uartTxCompleteFlg);
    uartTxCompleteFlg = 0;
#endif
    return size;
}
#if defined __llvm__ && defined __clang__
static int _uart_putchar(char c, FILE *stream)
{
    (void)stream;
    USR_SCI_UART_Write(&g_uart8_ctrl, (uint8_t *)&c, 1);
    return 0;
}
static FILE __stdio = FDEV_SETUP_STREAM(_uart_putchar, NULL, NULL, _FDEV_SETUP_WRITE);
FILE *const stdout = &__stdio;
FILE *const stdin  = &__stdio;
FILE *const stderr = &__stdio;
#endif
#elif defined __ICCARM__
#include <yfuns.h>
#if __VER__ < 8000000
  _STD_BEGIN
#endif
  #pragma module_name = "?__write"
size_t __write(int handle, const unsigned char * buffer, size_t size)
{
      USR_SCI_UART_Write(&g_uart8_ctrl, (uint8_t *)buffer, (uint32_t)size);
}
#if __VER__ < 8000000
  _STD_END
#endif
#else
int fputc(int ch, FILE *f)
{
    (void)f;
    USR_SCI_UART_Write(&g_uart8_ctrl, (uint8_t *)&ch, 1);
    return ch;
}
#endif

void g_uart8CB(uart_callback_args_t *p_args)
{
    if(p_args->event == UART_EVENT_TX_COMPLETE)
    {
        uartTxCompleteFlg = 1;
    }
}
#endif//#if USE_UART_PRINT
