/* generated vector header file - do not edit */
#ifndef ISR_VECTOR_DATA_H
#define ISR_VECTOR_DATA_H
#ifdef __cplusplus
        extern "C" {
        #endif
/* Number of interrupts allocated */
#ifndef ISR_VECTOR_DATA_NUM
#if defined(BOARD_RA8T2_EK)
#define ISR_VECTOR_DATA_NUM    (BSP_CORTEX_VECTOR_TABLE_ENTRIES + 12)
#else // (BOARD_RA8T2_MCK)
#define ISR_VECTOR_DATA_NUM    (BSP_CORTEX_VECTOR_TABLE_ENTRIES + 8)
#endif
#endif

extern uint8_t g_main_stack[];

/* ISR prototypes */
extern void Reset_Handler(void);
extern void NMI_Handler(void);
extern void HardFault_Handler(void);
extern void MemManage_Handler(void);
extern void BusFault_Handler(void);
extern void UsageFault_Handler(void);
extern void SecureFault_Handler(void);
extern void SVC_Handler(void);
extern void DebugMon_Handler(void);
extern void PendSV_Handler(void);
extern void SysTick_Handler(void);

extern void ethercat_ssc_port_isr_esc_cat(void);
extern void ethercat_ssc_port_isr_esc_sync0(void);
extern void ethercat_ssc_port_isr_esc_sync1(void);
extern void gpt_counter_overflow_isr(void);
extern void sci_b_uart_rxi_isr(void);
extern void sci_b_uart_txi_isr(void);
extern void sci_b_uart_tei_isr(void);
extern void sci_b_uart_eri_isr(void);

#if defined(BOARD_RA8T2_EK)
extern void iic_master_rxi_isr(void);
extern void iic_master_txi_isr(void);
extern void iic_master_tei_isr(void);
extern void iic_master_eri_isr(void);
#endif

#endif /* ISR_VECTOR_DATA_H */
