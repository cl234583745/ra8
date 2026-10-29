/*
                  Linker File for Renesas FSP
*/

INCLUDE memory_regions.ld

/* For EtherCAT FoE */
STARTUP_AREA_LENGTH = 16K;                      /* Start Up area 8KBx2 */
RESERVED_AREA_LENGTH = STARTUP_AREA_LENGTH+ 4K; /* + Backup memory for SEMI 4KB */
SINGLE_BANK_LENGTH = (FLASH_LENGTH - RESERVED_AREA_LENGTH) / 2; /* */
BANK0_OFFSET = 0K;                              /* BANK0 */
BANK1_OFFSET = SINGLE_BANK_LENGTH;              /* BANK1 */
BANK_OFFSET =  BANK0_OFFSET;                    /* BANK select */

INCLUDE fsp_mram_bank.ld
