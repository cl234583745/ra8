/***********************************************************************************************************************
 * File Name    : mram_sii.h
 * Description  : SII EEPROM emulation backed by RA8T2 code MRAM.
 **********************************************************************************************************************/

#ifndef MRAM_SII_H_
#define MRAM_SII_H_

#include <stdbool.h>
#include <stdint.h>
#include "hal_data.h"
#include "ecat_def.h"

#if (ECAT_EEPROM_USE_MRAM_EMULATION == 1)

fsp_err_t MRAM_SII_Init(void);
void      MRAM_SII_RegisterCallbacks(void);

UINT16 MRAM_SII_ReadWords(UINT32 wordaddr, UINT16 words, UINT16 *data);
UINT16 MRAM_SII_WriteWords(UINT32 wordaddr, UINT16 words, const UINT16 *data);

bool MRAM_SII_StoreNow(void);

#endif /* (ECAT_EEPROM_USE_MRAM_EMULATION == 1) */

#endif /* MRAM_SII_H_ */
