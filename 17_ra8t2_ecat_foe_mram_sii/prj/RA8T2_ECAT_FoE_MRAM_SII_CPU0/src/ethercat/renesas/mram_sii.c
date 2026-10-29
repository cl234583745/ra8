/***********************************************************************************************************************
 * File Name    : mram_sii.c
 * Description  : SII EEPROM emulation backed by RA8T2 code MRAM.
 *
 * The ESC SII interface is word based, while RA8T2 MRAM is programmed in 32-byte blocks. A complete 2 KB SII image is
 * kept in SRAM, modified by the SSC callbacks, and written back to MRAM after the ESC completes a write sequence.
 **********************************************************************************************************************/

#include "ecat_eeprom_config.h"

#if (ECAT_EEPROM_USE_MRAM_EMULATION == 1)

#include "mram_sii.h"
#include "applInterface.h"
#include "ecatappl.h"
#include "renesashw.h"
#include "r_fw_up_ra_if.h"
#include "eeprom.h"

#define MRAM_SII_WORD_COUNT              ((UINT32) (ESC_EEPROM_SIZE / 2U))
#define MRAM_SII_BLOCK_SIZE              BSP_FEATURE_MRAM_PROGRAMMING_SIZE_BYTES
#define MRAM_SII_BLOCK_COUNT             ((ESC_EEPROM_SIZE + MRAM_SII_BLOCK_SIZE - 1U) / MRAM_SII_BLOCK_SIZE)
#define MRAM_SII_DIRTY_BYTE_COUNT        ((MRAM_SII_BLOCK_COUNT + 7U) / 8U)

#define MRAM_SII_STATION_ALIAS_WORD      (4U)
#define MRAM_SII_CHECKSUM_WORD           (7U)
#define MRAM_SII_VENDOR_WORD             (8U)
#define MRAM_SII_PRODUCT_WORD            (10U)
#define MRAM_SII_REVISION_WORD           (12U)

#define MRAM_SII_ERR_RANGE               ((UINT16) 1U)
#define MRAM_SII_ERR_NOT_INITIALIZED     ((UINT16) 2U)

typedef char mram_sii_default_size_check[(sizeof(aEepromData) == ESC_EEPROM_SIZE) ? 1 : -1];
typedef char mram_sii_word_size_check[((ESC_EEPROM_SIZE % 2U) == 0U) ? 1 : -1];
typedef char mram_sii_dirty_map_size_check[(MRAM_SII_DIRTY_BYTE_COUNT <= 8U) ? 1 : -1];

static UINT16 aSiiShadow[ESC_EEPROM_SIZE / 2U] __attribute__((aligned(32)));
static uint8_t aDirtyBlocks[MRAM_SII_DIRTY_BYTE_COUNT];
static bool bSiiInitialized;

static bool MRAM_SII_RangeValid(UINT32 wordaddr, UINT16 words)
{
    return (words != 0U) &&
           (wordaddr < MRAM_SII_WORD_COUNT) &&
           ((UINT32) words <= (MRAM_SII_WORD_COUNT - wordaddr));
}

static void MRAM_SII_MarkBlockDirty(UINT32 block)
{
    aDirtyBlocks[block >> 3U] |= (uint8_t) (1U << (block & 7U));
}

static bool MRAM_SII_IsBlockDirty(UINT32 block)
{
    return (aDirtyBlocks[block >> 3U] & (uint8_t) (1U << (block & 7U))) != 0U;
}

static void MRAM_SII_ClearBlockDirty(UINT32 block)
{
    aDirtyBlocks[block >> 3U] &= (uint8_t) ~(1U << (block & 7U));
}

static void MRAM_SII_MarkAllDirty(void)
{
    UINT32 block;

    for (block = 0U; block < MRAM_SII_BLOCK_COUNT; block++)
    {
        MRAM_SII_MarkBlockDirty(block);
    }
}

static uint8_t MRAM_SII_CalculateChecksum(void)
{
    uint8_t checksum = 0xFFU;
    UINT32 byte_index;
    UINT8 bit;

    for (byte_index = 0U; byte_index < (MRAM_SII_CHECKSUM_WORD * 2U); byte_index++)
    {
        checksum ^= (uint8_t) ((aSiiShadow[byte_index >> 1U] >> ((byte_index & 1U) * 8U)) & 0xFFU);

        for (bit = 0U; bit < 8U; bit++)
        {
            if ((checksum & 0x80U) != 0U)
            {
                checksum = (uint8_t) ((checksum << 1U) ^ 0x07U);
            }
            else
            {
                checksum = (uint8_t) (checksum << 1U);
            }
        }
    }

    return checksum;
}

static void MRAM_SII_UpdateChecksum(void)
{
    aSiiShadow[MRAM_SII_CHECKSUM_WORD] = (UINT16) MRAM_SII_CalculateChecksum();
}

static bool MRAM_SII_ChecksumValid(void)
{
    return (uint8_t) (aSiiShadow[MRAM_SII_CHECKSUM_WORD] & 0xFFU) == MRAM_SII_CalculateChecksum();
}

static void MRAM_SII_ReadImage(void)
{
    volatile const uint8_t *p_source = (volatile const uint8_t *) FW_UP_SII_PRM_START_ADDR;
    UINT32 word;

    for (word = 0U; word < MRAM_SII_WORD_COUNT; word++)
    {
        aSiiShadow[word] = (UINT16) p_source[word * 2U] |
                           (UINT16) ((UINT16) p_source[(word * 2U) + 1U] << 8U);
    }
}

static void MRAM_SII_LoadDefaultImage(void)
{
    UINT32 word;

    for (word = 0U; word < MRAM_SII_WORD_COUNT; word++)
    {
        aSiiShadow[word] = (UINT16) aEepromData[word * 2U] |
                           (UINT16) ((UINT16) aEepromData[(word * 2U) + 1U] << 8U);
    }

    aSiiShadow[MRAM_SII_VENDOR_WORD] = (UINT16) VENDOR_ID;
    aSiiShadow[MRAM_SII_VENDOR_WORD + 1U] = (UINT16) ((UINT32) VENDOR_ID >> 16U);
    aSiiShadow[MRAM_SII_PRODUCT_WORD] = (UINT16) PRODUCT_CODE;
    aSiiShadow[MRAM_SII_PRODUCT_WORD + 1U] = (UINT16) ((UINT32) PRODUCT_CODE >> 16U);
    aSiiShadow[MRAM_SII_REVISION_WORD] = (UINT16) REVISION_NUMBER;
    aSiiShadow[MRAM_SII_REVISION_WORD + 1U] = (UINT16) ((UINT32) REVISION_NUMBER >> 16U);
    MRAM_SII_UpdateChecksum();
}

static bool MRAM_SII_ImageIsValid(void)
{
    UINT16 vendor_low = (UINT16) VENDOR_ID;
    UINT16 vendor_high = (UINT16) ((UINT32) VENDOR_ID >> 16U);
    UINT16 product_low = (UINT16) PRODUCT_CODE;
    UINT16 product_high = (UINT16) ((UINT32) PRODUCT_CODE >> 16U);

    return (aSiiShadow[MRAM_SII_VENDOR_WORD] == vendor_low) &&
           (aSiiShadow[MRAM_SII_VENDOR_WORD + 1U] == vendor_high) &&
           (aSiiShadow[MRAM_SII_PRODUCT_WORD] == product_low) &&
           (aSiiShadow[MRAM_SII_PRODUCT_WORD + 1U] == product_high) &&
           (aSiiShadow[MRAM_SII_REVISION_WORD] != 0xFFFFU) &&
           (aSiiShadow[MRAM_SII_REVISION_WORD + 1U] != 0xFFFFU) &&
           MRAM_SII_ChecksumValid();
}

static bool MRAM_SII_SynchronizeRevision(void)
{
    UINT16 revision[2];

    revision[0] = (UINT16) REVISION_NUMBER;
    revision[1] = (UINT16) ((UINT32) REVISION_NUMBER >> 16U);

    if ((aSiiShadow[MRAM_SII_REVISION_WORD] == revision[0]) &&
        (aSiiShadow[MRAM_SII_REVISION_WORD + 1U] == revision[1]))
    {
        return true;
    }

    if (MRAM_SII_WriteWords(MRAM_SII_REVISION_WORD, 2U, revision) != 0U)
    {
        return false;
    }

    MRAM_SII_UpdateChecksum();
    MRAM_SII_MarkBlockDirty((MRAM_SII_CHECKSUM_WORD * 2U) / MRAM_SII_BLOCK_SIZE);
    return MRAM_SII_StoreNow();
}

static bool MRAM_SII_VerifyBlock(UINT32 block)
{
    volatile const uint8_t *p_target = (volatile const uint8_t *) (FW_UP_SII_PRM_START_ADDR +
                                                                    (block * MRAM_SII_BLOCK_SIZE));
    const uint8_t *p_source = (const uint8_t *) &aSiiShadow[(block * MRAM_SII_BLOCK_SIZE) / 2U];
    UINT32 offset;

    for (offset = 0U; offset < MRAM_SII_BLOCK_SIZE; offset++)
    {
        if (p_target[offset] != p_source[offset])
        {
            return false;
        }
    }

    return true;
}

static UINT16 MRAM_SII_ReadCallback(UINT32 wordaddr)
{
    UINT16 data[4];
    UINT16 words = (UINT16) (EepromReadSize >> 1U);
    UINT16 index;

    if ((words != 2U) && (words != 4U))
    {
        return MRAM_SII_ERR_RANGE;
    }

    if (MRAM_SII_ReadWords(wordaddr, words, data) != 0U)
    {
        return MRAM_SII_ERR_RANGE;
    }

    for (index = 0U; index < words; index++)
    {
        HW_EscWriteWord(data[index], ESC_EEPROM_DATA_OFFSET + (index * 2U));
    }

    return 0U;
}

static UINT16 MRAM_SII_WriteCallback(UINT32 wordaddr)
{
    UINT16 data;

    HW_EscReadWord(data, ESC_EEPROM_DATA_OFFSET);
    return MRAM_SII_WriteWords(wordaddr, 1U, &data);
}

static UINT16 MRAM_SII_ReloadCallback(void)
{
    HW_EscWriteWord(aSiiShadow[MRAM_SII_STATION_ALIAS_WORD], ESC_EEPROM_DATA_OFFSET);
    return 0U;
}

static void MRAM_SII_StoreCallback(void)
{
    (void) MRAM_SII_StoreNow();
}

fsp_err_t MRAM_SII_Init(void)
{
    if (bSiiInitialized)
    {
        return FSP_SUCCESS;
    }

    MRAM_SII_ReadImage();
    if (MRAM_SII_ImageIsValid())
    {
        bSiiInitialized = true;
        if (!MRAM_SII_SynchronizeRevision())
        {
            bSiiInitialized = false;
            return FSP_ERR_WRITE_FAILED;
        }

        return FSP_SUCCESS;
    }

    MRAM_SII_LoadDefaultImage();
    MRAM_SII_MarkAllDirty();
    bSiiInitialized = true;

    if (!MRAM_SII_StoreNow())
    {
        bSiiInitialized = false;
        return FSP_ERR_WRITE_FAILED;
    }

    return FSP_SUCCESS;
}

void MRAM_SII_RegisterCallbacks(void)
{
    pAPPL_EEPROM_Read = MRAM_SII_ReadCallback;
    pAPPL_EEPROM_Write = MRAM_SII_WriteCallback;
    pAPPL_EEPROM_Reload = MRAM_SII_ReloadCallback;
    pAPPL_EEPROM_Store = MRAM_SII_StoreCallback;
}

UINT16 MRAM_SII_ReadWords(UINT32 wordaddr, UINT16 words, UINT16 *data)
{
    UINT16 index;

    if (!bSiiInitialized)
    {
        return MRAM_SII_ERR_NOT_INITIALIZED;
    }

    if ((data == NULL) || !MRAM_SII_RangeValid(wordaddr, words))
    {
        return MRAM_SII_ERR_RANGE;
    }

    for (index = 0U; index < words; index++)
    {
        data[index] = aSiiShadow[wordaddr + index];
    }

    return 0U;
}

UINT16 MRAM_SII_WriteWords(UINT32 wordaddr, UINT16 words, const UINT16 *data)
{
    UINT16 index;

    if (!bSiiInitialized)
    {
        return MRAM_SII_ERR_NOT_INITIALIZED;
    }

    if ((data == NULL) || !MRAM_SII_RangeValid(wordaddr, words))
    {
        return MRAM_SII_ERR_RANGE;
    }

    for (index = 0U; index < words; index++)
    {
        UINT32 word = wordaddr + index;

        aSiiShadow[word] = data[index];
        MRAM_SII_MarkBlockDirty((word * 2U) / MRAM_SII_BLOCK_SIZE);
    }

    return 0U;
}

bool MRAM_SII_StoreNow(void)
{
    UINT32 block;
    uint32_t primask;
    bool success = true;

    if (!bSiiInitialized)
    {
        return false;
    }

    primask = __get_PRIMASK();
    __disable_irq();

    for (block = 0U; block < MRAM_SII_BLOCK_COUNT; block++)
    {
        if (!MRAM_SII_IsBlockDirty(block))
        {
            continue;
        }

        if ((R_MRAM_Write(&g_mram0_ctrl,
                          (uint32_t) &aSiiShadow[(block * MRAM_SII_BLOCK_SIZE) / 2U],
                          FW_UP_SII_PRM_START_ADDR + (block * MRAM_SII_BLOCK_SIZE),
                          MRAM_SII_BLOCK_SIZE) != FSP_SUCCESS) ||
            !MRAM_SII_VerifyBlock(block))
        {
            success = false;
            break;
        }

        MRAM_SII_ClearBlockDirty(block);
    }

    __set_PRIMASK(primask);
    return success;
}

#endif /* (ECAT_EEPROM_USE_MRAM_EMULATION == 1) */
