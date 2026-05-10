#include "hal_data.h"

#include "usb_config.h"

#define CURRENT_LOG_LEVEL   LOG_LEVEL_DEBUG
#define LOG_USE
#include "log.h"

#if (1 == BSP_MULTICORE_PROJECT) && BSP_TZ_SECURE_BUILD
bsp_ipc_semaphore_handle_t g_core_start_semaphore =
{
    .semaphore_num = 0
};
#endif

extern void cdc_acm_init(uint8_t busid, uintptr_t reg_base);

void hal_entry(void)
{
#if PRINTF
    __enable_irq();

    g_uart8.p_api->open(g_uart8.p_ctrl, g_uart8.p_cfg);

    LOG_INFO("g_uart8.p_api->open\n");
    LOG_INFO("date:%s\ntime:%s\nfile:%s\nfunc:%s,line:%d\nhello world!\n", __DATE__, __TIME__, __FILE__, __FUNCTION__, __LINE__);

    float PI = 3.1415926f;
    LOG_INFO("PI=%f\n", PI);
    LOG_INFO("%s\n", FSP_VERSION_BUILD_STRING);

    R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);
#endif

#if (0 == _RA_CORE) && (1 == BSP_MULTICORE_PROJECT) && !BSP_TZ_NONSECURE_BUILD

#if BSP_TZ_SECURE_BUILD
    R_BSP_IpcSemaphoreTake(&g_core_start_semaphore);
#endif

    R_BSP_SecondaryCoreStart();

#if BSP_TZ_SECURE_BUILD
    while(FSP_ERR_IN_USE == R_BSP_IpcSemaphoreTake(&g_core_start_semaphore))
    {
        ;
    }
#endif
#endif

#if (1 == _RA_CORE) && (1 == BSP_MULTICORE_PROJECT) && BSP_TZ_SECURE_BUILD
    R_BSP_IpcSemaphoreGive(&g_core_start_semaphore);
#endif

#if BSP_TZ_SECURE_BUILD
    R_BSP_NonSecureEnter();
#endif

    static uint32_t cnt = 0;
    // 0 usbfs   1 usbhs
#ifndef CONFIG_USB_HS
    // 0 usbfs
    LOG_INFO("USB full-speed\n");
    R_IOPORT_PinWrite (&IOPORT_CFG_CTRL, BSP_IO_PORT_04_PIN_13, 0);
    cdc_acm_init(0, R_USB_FS0_BASE);

#else
    // 1 usbhs
    LOG_INFO("USB high-speed! %d\n",cnt++);
    R_IOPORT_PinWrite (&IOPORT_CFG_CTRL, BSP_IO_PORT_04_PIN_13, 1);
    cdc_acm_init(1, R_USB_HS0_BASE);

#endif

    while (1)
    {
        LOG_INFO("running! %ld\n",cnt++);
        R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MILLISECONDS);
    }
}

#if BSP_TZ_SECURE_BUILD

FSP_CPP_HEADER
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
FSP_CPP_FOOTER

#endif



