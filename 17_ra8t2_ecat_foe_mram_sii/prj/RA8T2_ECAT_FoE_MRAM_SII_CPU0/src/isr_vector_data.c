/* generated vector source file - do not edit */
#include "bsp_api.h"
#include "isr_vector_data.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
extern uint32_t __ram_vector_start;
void RelocateVectorTableToRAM(void);

__attribute__((section(".isr_vector")))
BSP_DONT_REMOVE const fsp_vector_t g_isr_vector_table[ISR_VECTOR_DATA_NUM] =
{
    (fsp_vector_t) (&g_main_stack[0] + BSP_CFG_STACK_MAIN_BYTES), /*      Initial Stack Pointer     */
    Reset_Handler,                                             /*      Reset Handler             */
    NMI_Handler,                                               /*      NMI Handler               */
    HardFault_Handler,                                         /*      Hard Fault Handler        */
    MemManage_Handler,                                         /*      MPU Fault Handler         */
    BusFault_Handler,                                          /*      Bus Fault Handler         */
    UsageFault_Handler,                                        /*      Usage Fault Handler       */
    SecureFault_Handler,                                       /*      Secure Fault Handler      */
    0,                                                         /*      Reserved                  */
    0,                                                         /*      Reserved                  */
    0,                                                         /*      Reserved                  */
    SVC_Handler,                                               /*      SVCall Handler            */
    DebugMon_Handler,                                          /*      Debug Monitor Handler     */
    0,                                                         /*      Reserved                  */
    PendSV_Handler,                                            /*      PendSV Handler            */
    SysTick_Handler,                                           /*      SysTick Handler           */
	/* IRQ */ 
	ethercat_ssc_port_isr_esc_cat,                             /* ETHC I (Interrupt) */
	ethercat_ssc_port_isr_esc_sync0,                           /* ETHC SI0 (Sync0 Interrupt) */
	ethercat_ssc_port_isr_esc_sync1,                           /* ETHC SI1 (Sync1 Interrupt) */
	gpt_counter_overflow_isr,                                  /* GPT0 COUNTER OVERFLOW (Overflow) */
	sci_b_uart_rxi_isr, 									   /* SCI0 RXI (Receive data full) */
	sci_b_uart_txi_isr, 									   /* SCI0 TXI (Transmit data empty) */
	sci_b_uart_tei_isr,										   /* SCI0 TEI (Transmit end) */
	sci_b_uart_eri_isr, 									   /* SCI0 ERI (Receive error) */
#if defined(BOARD_RA8T2_EK)
	iic_master_rxi_isr, 									   /* IIC0 RXI (Receive data full) */
	iic_master_txi_isr,										   /* IIC0 TXI (Transmit data empty) */
	iic_master_tei_isr,										   /* IIC0 TEI (Transmit end) */
	iic_master_eri_isr,										   /* IIC0 ERI (Transfer error) */
#endif
};

void RelocateVectorTableToRAM(void)
{

    /* VTOR を RAM ベクタへ切替 */
    SCB->VTOR = (uint32_t)&__ram_vector_start;

    __DSB();   // Data Synchronization Barrier
    __ISB();   // Instruction Synchronization Barrier

}
