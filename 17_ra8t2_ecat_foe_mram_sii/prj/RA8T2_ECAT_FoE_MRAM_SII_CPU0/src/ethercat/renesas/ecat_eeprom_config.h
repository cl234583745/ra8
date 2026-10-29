/***********************************************************************************************************************
 * File Name    : ecat_eeprom_config.h
 * Description  : Selects the SII EEPROM backend used by the EtherCAT slave stack.
 *
 * Set ECAT_EEPROM_USE_MRAM_EMULATION to:
 *   0 - Keep using the original external EEPROM path.
 *   1 - Emulate the 2 KB SII EEPROM with RA8T2 code MRAM.
 *
 * The value can also be overridden from the compiler command line.
 **********************************************************************************************************************/

#ifndef ECAT_EEPROM_CONFIG_H_
#define ECAT_EEPROM_CONFIG_H_

#ifndef ECAT_EEPROM_USE_MRAM_EMULATION
#define ECAT_EEPROM_USE_MRAM_EMULATION      (1)
#endif

#endif /* ECAT_EEPROM_CONFIG_H_ */
