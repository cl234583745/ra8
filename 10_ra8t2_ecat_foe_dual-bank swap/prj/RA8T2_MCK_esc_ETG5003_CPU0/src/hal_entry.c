/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <stdio.h>
#include "hal_data.h"
#include "applInterface.h"
#include "sampleappl.h"
#include "sio_char.h"

#if defined(BOARD_RA8T2_EK)
#include "i2c_expander.h"
#endif

extern void RelocateVectorTableToRAM(void);

void R_BSP_WarmStart(bsp_warm_start_event_t event);
void handle_error(fsp_err_t err);

/* If Code Flash programming is enabled, then all API functions must execute out of RAM. */
#if defined(__ICCARM__)
 #pragma section=".ram_from_flash"
#endif
#if defined(__ARMCC_VERSION) || defined(__GNUC__)
 #define PLACE_IN_RAM_SECTION    __attribute__((noinline)) BSP_PLACE_IN_SECTION(".ram_from_flash")
#else
 #define PLACE_IN_RAM_SECTION    BSP_PLACE_IN_SECTION(".ram_from_flash")
#endif

/** Use for UART communication */
void user_uart_callback (uart_callback_args_t * p_args);
uint8_t  g_out_of_band_received[TRANSFER_LENGTH];
volatile uint32_t g_transfer_complete = 0;
volatile uint32_t g_receive_complete  = 0;
uint32_t g_out_of_band_index = 0;

/*******************************************************************************************************************//**
 * @brief  EtherCAT Slave Stack example application
 *
 * The EtherCAT Slave Stack Code is provided by SSC tool.
 *
 **********************************************************************************************************************/
PLACE_IN_RAM_SECTION void hal_entry (void)
{
	fsp_err_t err;

#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif

#if (FOE_SUPPORTED == 1)
	/* Relocate Vector Table to SRAM */
	RelocateVectorTableToRAM();
#endif

#if defined(R_SCI_B_UART_H)
	/* Open the transfer instance with initial configuration. */
	err = R_SCI_B_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
	handle_error(err);
#endif

#if (FOE_SUPPORTED == 1)
    /* Open the MRAM instance */
    err = R_MRAM_Open(&g_mram0_ctrl, &g_mram0_cfg);
	handle_error(err);
#endif

#if defined(BOARD_RA8T2_EK)
	/* Initialize I/O expander */
	init_io_expander();
#endif

    /* Initialize EtherCAT SSC Port */
    err = RM_ETHERCAT_SSC_PORT_Open(gp_ethercat_ssc_port->p_ctrl, gp_ethercat_ssc_port->p_cfg);
    handle_error(err);

	/* Print that the EtherCAT Sample starts */
#if defined(BOARD_RA8T2_MCK) | defined(BOARD_RA8T2_EK)
 #if defined(MRAM_BANK0)
    printf("RA8T2 EtherCAT sample program starts on BANK0.\r\n");
 #elif defined(MRAM_BANK1)
    printf("RA8T2 EtherCAT sample program starts on BANK1.\r\n");
 #endif
#endif

	/* Initilize the stack */
	MainInit();
#if (CiA402_SAMPLE_APPLICATION == 1)
	/* Initialize axis structures */
	CiA402_Init();
#endif

	/* Create basic mapping */
	APPL_GenerateMapping(&nPdInputSize,&nPdOutputSize);

	/* Set stack run flag */
	bRunApplication = TRUE;

	/* Execute the stack */
	while(bRunApplication == TRUE)
	{
		MainLoop();
	}

#if (CiA402_SAMPLE_APPLICATION == 1)
	/* Remove all allocated axes resources */
	CiA402_DeallocateAxis();
#endif
	/* Close SSC Port */
	RM_ETHERCAT_SSC_PORT_Close(gp_ethercat_ssc_port->p_ctrl);

	while(1);
}

/* Function Name: handle_error */
/******************************************************************************************************************//**
 * @brief Check if an error occurs

 * @param[in] fsp_err_t err
 *********************************************************************************************************************/
void handle_error(fsp_err_t err)
{
    if (err != FSP_SUCCESS)
    {
		__NOP();
    }

}

/* Function Name: user_uart_callback */
void user_uart_callback (uart_callback_args_t * p_args)
{
    /* Handle the UART event */
    switch (p_args->event)
    {
        /* Received a character */
        case UART_EVENT_RX_CHAR:
        {
            /* Only put the next character in the receive buffer if there is space for it */
            if (sizeof(g_out_of_band_received) > g_out_of_band_index)
            {
                /* Write either the next one or two bytes depending on the receive data size */
                if ((UART_DATA_BITS_7 == g_uart0_cfg.data_bits) || (UART_DATA_BITS_8 == g_uart0_cfg.data_bits))
                {
                    g_out_of_band_received[g_out_of_band_index++] = (uint8_t) p_args->data;
                }
                else
                {
                    uint16_t * p_dest = (uint16_t *) &g_out_of_band_received[g_out_of_band_index];
                    *p_dest              = (uint16_t) p_args->data;
                    g_out_of_band_index += 2;
                }
            }
            break;
        }
        /* Receive complete */
        case UART_EVENT_RX_COMPLETE:
        {
            g_receive_complete = 1;
            break;
        }
        /* Transmit complete */
        case UART_EVENT_TX_COMPLETE:
        {
            g_transfer_complete = 1;
            break;
        }
        default:
        {
        }
    }
}

#if defined(BOARD_RA8T2_MCK)
/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open(&IOPORT_CFG_CTRL, &IOPORT_CFG_NAME);
    }
}
#endif /* defined(BOARD_RA8T2_MCK) */
