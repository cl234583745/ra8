#include "mram_test.h"
#include "common_utils.h"

#define SEED_ADDR       (0x020FF000U)
#define FW_ADDR         (0x02080000U)
#define FW_SIZE         (512 * 1024)
#define CHUNK_SIZE      (1024)
#define NUM_CHUNKS      (FW_SIZE / CHUNK_SIZE)

#define DWT_DEMCR       (*(volatile uint32_t *)0xE000EDFC)
#define DWT_CONTROL     (*(volatile uint32_t *)0xE0001000)
#define DWT_CYCCNT      (*(volatile uint32_t *)0xE0001004)

static uint32_t crc_tbl[256];
static uint8_t  chunk[CHUNK_SIZE] __attribute__((aligned(32)));

static void build_crc_table(void)
{
    for (uint32_t i = 0; i < 256; i++)
    {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++)
            crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320UL : 0);
        crc_tbl[i] = crc;
    }
}

static uint32_t crc_update(uint32_t crc, const uint8_t *data, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
        crc = crc_tbl[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    return crc;
}

static void fill_const(uint32_t ch, uint8_t val)
{
    (void)ch;
    memset(chunk, val, CHUNK_SIZE);
}

static void fill_inc(uint32_t ch, uint8_t seed)
{
    for (uint32_t i = 0; i < CHUNK_SIZE; i++)
        chunk[i] = (uint8_t)(seed + ch + i);
}

typedef void (*fill_fn_t)(uint32_t ch, uint8_t param);

static void test_pattern(const char *label, fill_fn_t fill_fn, uint8_t param)
{
    APP_PRINT("\r\n--- Pattern: %s ---\r\n", label);

    /* Pre-compute source CRC */
    uint32_t src_crc = 0xFFFFFFFFUL;
    for (uint32_t ch = 0; ch < NUM_CHUNKS; ch++)
    {
        fill_fn(ch, param);
        src_crc = crc_update(src_crc, chunk, CHUNK_SIZE);
    }
    src_crc ^= 0xFFFFFFFFUL;

    /* Write */
    uint32_t sw_cyc = 0, sw_us, sw_kbs;
    uint32_t tw0 = DWT_CYCCNT;
    for (uint32_t ch = 0; ch < NUM_CHUNKS; ch++)
    {
        fill_fn(ch, param);
        uint32_t t0 = DWT_CYCCNT;
        fsp_err_t err = R_MRAM_Write(&g_mram_ctrl, (uint32_t)chunk, FW_ADDR + ch * CHUNK_SIZE, CHUNK_SIZE);
        uint32_t t1 = DWT_CYCCNT;
        if (FSP_SUCCESS != err) { APP_PRINT("  FAIL: write error %ld\r\n", (int32_t)err); return; }
        if (ch == 0) sw_cyc = t1 - t0;
    }
    uint32_t tw_us = (DWT_CYCCNT - tw0) / 1000;
    sw_us  = sw_cyc / 1000;
    sw_kbs = sw_us ? (1024UL * 1000000UL / sw_us / 1024UL) : 0;
    uint32_t tw_kbs = tw_us ? (uint32_t)((uint64_t)FW_SIZE * 1000000ULL / ((uint64_t)tw_us * 1024ULL)) : 0;
    APP_PRINT("  Write 1KB: %lu us = %lu KB/s (%lu.%03lu MB/s)\r\n",
              sw_us, sw_kbs, sw_kbs / 1024, (sw_kbs % 1024) * 1000 / 1024);
    APP_PRINT("  Write all: %lu us = %lu KB/s (%lu.%03lu MB/s)\r\n",
              tw_us, tw_kbs, tw_kbs / 1024, (tw_kbs % 1024) * 1000 / 1024);

    /* Read + CRC */
    uint32_t rd_tot = 0, crc_tot = 0;
    uint32_t rd_crc = 0xFFFFFFFFUL;
    for (uint32_t ch = 0; ch < NUM_CHUNKS; ch++)
    {
        uint32_t t0 = DWT_CYCCNT;
        memcpy(chunk, (uint8_t *)(FW_ADDR + ch * CHUNK_SIZE), CHUNK_SIZE);
        uint32_t t1 = DWT_CYCCNT;
        rd_tot += t1 - t0;

        t0 = DWT_CYCCNT;
        rd_crc = crc_update(rd_crc, chunk, CHUNK_SIZE);
        t1 = DWT_CYCCNT;
        crc_tot += t1 - t0;
    }
    rd_crc ^= 0xFFFFFFFFUL;

    uint32_t rd_us = rd_tot / 1000;
    uint32_t crc_us = crc_tot / 1000;
    uint32_t rd_kbs = rd_us ? (uint32_t)((uint64_t)FW_SIZE * 1000000ULL / ((uint64_t)rd_us * 1024ULL)) : 0;
    APP_PRINT("  Read: %lu us = %lu KB/s (%lu.%03lu MB/s)\r\n",
              rd_us, rd_kbs, rd_kbs / 1024, (rd_kbs % 1024) * 1000 / 1024);
    APP_PRINT("  CRC: %lu us, check: %s\r\n", crc_us,
              (src_crc == rd_crc) ? "PASS" : "FAIL");
}

/* Temporarily disable DCache so stores reach MRAM during P/E mode */
static void mram_disable_dcache(void)
{
    SCB_CleanInvalidateDCache();
    SCB_DisableDCache();
    __DSB();
    __ISB();
}

static void mram_enable_dcache(void)
{
    SCB_EnableDCache();
    __DSB();
    __ISB();
}

void mram_auto_test(void)
{
    fsp_err_t err;
    uint8_t  seed  = *(volatile uint8_t *) SEED_ADDR;

#if BSP_CFG_DCACHE_ENABLED
    mram_disable_dcache();
#endif

    build_crc_table();

    APP_PRINT("\r\n===== MRAM Write/Read Test =====\r\n");
    APP_PRINT("FW: %u KB @ 0x%08lX, Chunk: %u B, Seed: 0x%02X\r\n",
              FW_SIZE / 1024, FW_ADDR, CHUNK_SIZE, seed);

    err = R_MRAM_Open(&g_mram_ctrl, &g_mram_cfg);
    if (FSP_SUCCESS != err) { APP_ERR_TRAP(err); }
    err = R_MRAM_StartUpAreaSelect(&g_mram_ctrl, FLASH_STARTUP_AREA_BLOCK0, true);
    if (FSP_SUCCESS != err) { APP_ERR_TRAP(err); }

    DWT_DEMCR  |= (1UL << 24);
    __DSB();
    DWT_CONTROL |= 1UL;

    /* Three patterns */
    test_pattern("incremental (seed)", fill_inc, seed);
    test_pattern("all-0xFF", fill_const, 0xFF);
    test_pattern("all-0x00", fill_const, 0x00);

    /* Store new seed */
    uint8_t nseed = seed + 1;
    {
        uint8_t sb[32] = {0};
        sb[0] = nseed;
        __disable_irq();
        R_MRAM_Write(&g_mram_ctrl, (uint32_t)sb, SEED_ADDR, 32);
        __enable_irq();
    }

    APP_PRINT("\r\n===== Done =====\r\n");

#if BSP_CFG_DCACHE_ENABLED
    mram_enable_dcache();
#endif

    while (1);
}
