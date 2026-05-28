#include "hal_data.h"

#define CURRENT_LOG_LEVEL   LOG_LEVEL_DEBUG
#define LOG_USE
#include "log.h"

#define CFSAMON_BLOCK_SIZE_KB   32
#define SFSAMON_BLOCK_SIZE_KB   32
#define SRAMSABAR_BLOCK_SIZE_KB 8
#define TCMSABAR_BLOCK_SIZE_KB  8

#define FLASH_BASE 0x02000000
#define SRAM_BASE  0x22000000
#define NS_OFFSET  0x10000000

#define SAU_CTRL   (*(volatile uint32_t *)0xE000EDD0)
#define SAU_TYPE   (*(volatile uint32_t *)0xE000EDD4)
#define SAU_RNR    (*(volatile uint32_t *)0xE000EDD8)
#define SAU_RBAR   (*(volatile uint32_t *)0xE000EDDC)
#define SAU_RLAR   (*(volatile uint32_t *)0xE000EDE0)

#define SAU_CTRL_ENABLE_Msk   1
#define SAU_CTRL_ALLNS_Msk    2
#define SAU_RBAR_BADDR_Msk    0xFFFFFFE0
#define SAU_RLAR_LADDR_Msk    0xFFFFFFE0
#define SAU_RLAR_NSC_Msk      2
#define SAU_RLAR_ENABLE_Msk   1

static const char *dlm_state_str(uint32_t v)
{
    switch (v)
    {
    case 0:  return "NS_CD (Non-Secure Chip Default)";
    case 1:  return "SD (Secure Debug)";
    case 2:  return "SSD (Secure Software Debug)";
    case 3:  return "OEM_PL0";
    case 4:  return "OEM_PL1";
    case 5:  return "OEM_PL2 (Production)";
    case 6:  return "OEM_PL3";
    case 7:  return "RMA (Return Material Authorization)";
    default: return "Reserved";
    }
}

static void print_cfsamon(const char *name, uint32_t addr, uint32_t val)
{
    uint32_t blocks = (val >> 15) & 0x1FF;
    uint32_t secure_kb = blocks * CFSAMON_BLOCK_SIZE_KB;
    uint32_t boundary = FLASH_BASE + secure_kb * 1024;

    LOG_INFO("%-12s @ 0x%08X = 0x%08X\n", name, addr, val);
    LOG_INFO("  -> Secure blocks: %3u  |  Secure size: %u KB\n", blocks, secure_kb);

    if (blocks >= 0x1FF)
    {
        LOG_INFO("  -> Boundary exceeds flash end -> entire flash is Non-Secure\n");
    }
    else if (blocks > 0)
    {
        LOG_INFO("  -> Secure: 0x%08X - 0x%08X  (%u KB)\n", FLASH_BASE, boundary - 1, secure_kb);
        LOG_INFO("  -> Non-Secure alias: 0x%08X - 0x%08X\n",
              boundary + NS_OFFSET, boundary + NS_OFFSET + 0x100000 - 1);
        LOG_WARN("  *** WARNING: %s has %u KB Secure region active! ***\n", name, secure_kb);
    }
    else
    {
        LOG_INFO("  -> All flash is Non-Secure (no partition)\n");
    }
}

static void print_sramsabar(const char *name, uint32_t addr, uint32_t val)
{
    uint32_t blocks = (val >> 13) & 0xFF;
    uint32_t ns_offset_kb = blocks * SRAMSABAR_BLOCK_SIZE_KB;

    LOG_INFO("%-12s @ 0x%08X = 0x%08X\n", name, addr, val);
    LOG_INFO("  -> NS region starts at block %3u  |  offset: %u KB\n", blocks, ns_offset_kb);

    if (blocks == 0)
    {
        LOG_INFO("  -> All SRAM is Non-Secure\n");
    }
    else if (blocks < 0xFF)
    {
        uint32_t boundary = SRAM_BASE + ns_offset_kb * 1024;
        LOG_INFO("  -> Secure SRAM: 0x%08X - 0x%08X  (%u KB)\n", SRAM_BASE, boundary - 1, ns_offset_kb);
        LOG_WARN("  *** WARNING: SRAM partition active (up to 0x%08X) ***\n", boundary - 1);
    }
    else
    {
        LOG_INFO("  -> Boundary at max -> all SRAM is Non-Secure\n");
    }
}

static void print_tcmsabar(const char *name, uint32_t addr, uint32_t val)
{
    uint32_t blocks = (val >> 13) & 0x3F;
    uint32_t ns_offset_kb = blocks * TCMSABAR_BLOCK_SIZE_KB;

    LOG_INFO("%-12s @ 0x%08X = 0x%08X\n", name, addr, val);
    LOG_INFO("  -> NS region starts at block %3u  |  offset: %u KB\n", blocks, ns_offset_kb);

    if (blocks > 0)
    {
        LOG_WARN("  *** WARNING: TCM partition is active ***\n");
    }
    else
    {
        LOG_INFO("  -> All TCM is Non-Secure\n");
    }
}

static void print_sau_region(uint32_t region)
{
    SAU_RNR = region;
    uint32_t rbar = SAU_RBAR;
    uint32_t rlar = SAU_RLAR;
    uint32_t base = rbar & SAU_RBAR_BADDR_Msk;
    uint32_t limit = (rlar & SAU_RLAR_LADDR_Msk) | 0x1F;
    uint8_t enabled = (rlar & SAU_RLAR_ENABLE_Msk) ? 1 : 0;
    uint8_t nsc    = (rlar & SAU_RLAR_NSC_Msk) ? 1 : 0;

    LOG_INFO("  SAU[%u] %s: 0x%08X - 0x%08X  %s\n",
          region, enabled ? "EN" : "DIS",
          base, limit,
          nsc ? "NSC" : (enabled ? "NS" : ""));
}

void bsp_tz_monitor_print(void)
{
    LOG_INFO("===== TrustZone Security Monitor =====\n");

    uint32_t cfsamona = R_PSCU->CFSAMONA;
    uint32_t sfsamon  = R_PSCU->SFSAMON;
    uint32_t dlmm     = R_PSCU->DLMMON;
    uint32_t sram0    = R_CPSCU->SRAMSABAR0;
    uint32_t sram1    = R_CPSCU->SRAMSABAR1;
    uint32_t sram2    = R_CPSCU->SRAMSABAR2;
    uint32_t sram3    = R_CPSCU->SRAMSABAR3;
    uint32_t tcmc     = R_CPSCU->TCMSABARC;
    uint32_t tcms     = R_CPSCU->TCMSABARS;

    LOG_INFO("DLM  @ 0x%08X = 0x%08X  -> %s\n",
          0x40204038, dlmm, dlm_state_str(dlmm & 0xF));

    LOG_RAW("\n");

    print_cfsamon("CFSAMONA", 0x40204030, cfsamona);
    print_cfsamon("SFSAMON",  0x4020403C, sfsamon);

    LOG_RAW("\n");

    print_sramsabar("SRAMSABAR0", 0x40008400, sram0);
    print_sramsabar("SRAMSABAR1", 0x40008404, sram1);
    print_sramsabar("SRAMSABAR2", 0x40008408, sram2);
    print_sramsabar("SRAMSABAR3", 0x4000840C, sram3);

    LOG_RAW("\n");

    print_tcmsabar("TCMSABARC", 0x40008508, tcmc);
    print_tcmsabar("TCMSABARS", 0x4000850C, tcms);

    LOG_RAW("\n");

    LOG_INFO("----- SAU Regions -----\n");
    uint32_t sau_ctrl = SAU_CTRL;
    LOG_INFO("SAU_CTRL = 0x%08X  %s\n", sau_ctrl,
          (sau_ctrl & SAU_CTRL_ENABLE_Msk) ? "ENABLED" :
          (sau_ctrl & SAU_CTRL_ALLNS_Msk) ? "ALL_NS" : "DISABLED");

    uint32_t sau_type = SAU_TYPE;
    LOG_INFO("SAU_TYPE = 0x%08X  (regions: %u)\n", sau_type, sau_type & 0xFF);

    uint32_t saved_rnr = SAU_RNR;
    for (uint32_t i = 0; i < 5; i++)
    {
        print_sau_region(i);
    }
    SAU_RNR = saved_rnr;

    LOG_RAW("\n");

    uint32_t cfs2 = (cfsamona >> 15) & 0x1FF;
    if (cfs2 > 0 && cfs2 < 0x1FF)
    {
        uint32_t secure_kb = cfs2 * CFSAMON_BLOCK_SIZE_KB;
        uint32_t boundary = FLASH_BASE + secure_kb * 1024;
        LOG_WARN(">>>> WARNING: Hardware TZ partition detected! <<<<\n");
        LOG_WARN(">>>> Code flash: %u KB Secure (0x%08X - 0x%08X) <<<<\n",
              secure_kb, FLASH_BASE, boundary - 1);
        LOG_WARN(">>>> RFP may fail to program flash beyond boundary! <<<<\n");
        LOG_WARN(">>>> Fix methods: <<<<\n");
        LOG_WARN(">>>>  1. RFP: connect -> Initialize Target (erase chip) <<<<\n");
        LOG_WARN(">>>>  2. RFP CLI: -fo boundary 0,0,0,0,0 (reset all) <<<<\n");
        LOG_WARN(">>>>  3. e2studio: Run -> Renesas Device Partition Manager <<<<\n");
        LOG_WARN(">>>>  4. e2studio Debug Config -> setTZBoundaries=false <<<<\n");
    }
    else
    {
        LOG_INFO(">>>> No active TZ partition -> device is fully Non-Secure. <<<<\n");
    }

    LOG_INFO("========================================\n");
}
