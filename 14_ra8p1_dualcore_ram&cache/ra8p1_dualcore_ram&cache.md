十四、RA8P1 双核内存(SRAM TCM)与 Cache 深度分析
===
[toc]

# 一、概述

- 机型：**RA8P1**，型号 **R7KA8P1KFLCAC**（FSP 6.5.0）
- CPU0：Cortex-M85（双核主核）；CPU1：Cortex-M33
- Code MRAM 1MB（每核 512KB）、SRAM 总计 1872KB（FSP 每核分 936KB = 0xEA000）
- **核心观点**：CPU0 与 CPU1 的 SRAM 段划分结构一致、仅物理基址不同，但 **Cache 配置策略截然不同**——CPU0 开 D-Cache 后整个 SRAM 被缓存，DMA/DTC 缓冲必须落 MPU nocache 段；CPU1 的 S-Cache 从不打开，无一致性问题。
- 本文将从 **CPU0 内存与 Cache、CPU1 内存与 Cache、深度对比、DMA/DTC 缓冲选型** 五个维度进行对比，并附上工程实践中的放置建议与注意事项。

# 二、CPU0（Cortex-M85）内存与 Cache 详解

## 2.1 工作原理与定位

CPU0 是双核主核，运行 Cortex-M85@1GHz。它拥有 **ITCM/DTCM 本地零等待内存** 与 **I-Cache/D-Cache 各 16KB**，D-Cache 通过 `BSP_CFG_DCACHE_ENABLED(1)` 打开。



## 2.2 硬件结构特点

- **Code MRAM（FLASH）**：0x02000000，512KB
- **SRAM 映射区**：0x22000000，936KB（0xEA000）
- **TCM**：ITCM 128KB + DTCM 128KB（本核本地，零等待，不缓存）
- **I-Cache / D-Cache**：各 16KB（D-Cache 已打开）
- **MPU**：启动时配置 nocache region，标记 `.ram_noinit_nocache` / `.bss.*_fsp_nocache` / `.ram_nocache` 为 non-cacheable
- **缓存一致性风险**：**有**（D-Cache 缓存 SRAM，DMA/DTC 绕过 D-Cache 直接读 SRAM）

## 2.3 关键特性总结

| 特性 | 说明 |
|------|------|
| 核心定位 | 双核主核，Cortex-M85 @ 1GHz |
| Code MRAM | 0x02000000，512KB |
| SRAM 映射区 | 0x22000000，936KB（0xEA000） |
| TCM | ITCM 128KB + DTCM 128KB（本核私有，零等待，不缓存） |
| I-Cache / D-Cache | 16KB / 16KB（D-Cache 已打开） |
| 缓存一致性风险 | **有**（D-Cache 缓存 SRAM） |
| DMA/DTC 安全区 | 仅 MPU 强制 non-cacheable 的 nocache 段 |

# 三、CPU1（Cortex-M33）内存与 Cache 详解

## 3.1 工作原理与定位

CPU1 是双核从核，运行 Cortex-M33。它拥有 **CTCM/STCM 本地零等待内存** 与 **C-Cache 16KB**，但 **S-Cache 从不打开**（FSP 从不写 SCACTL）。



## 3.2 硬件结构特点

- **Code MRAM（FLASH）**：0x02080000，512KB
- **SRAM 映射区**：0x220EA000，936KB（0xEA000）
- **TCM**：CTCM 64KB + STCM 64KB（本核本地，零等待，不缓存）
- **C-Cache**：16KB（非 TZ 构建下实际打开）
- **S-Cache**：16KB（**未开**，FSP 从不写 SCACTL）
- **MPU**：未配（S-Cache 关，无需 nocache 保护区）
- **缓存一致性风险**：**无**（S-Cache 关）
- **SRAME 注意**：CPU1 SRAM 区上端 208KB 落在 SRAME，仅当 SRAM ECC **关闭**时可用作数据区；本工程未使能 ECC，当前可用。

## 3.3 关键特性总结

| 特性 | 说明 |
|------|------|
| 核心定位 | 双核从核，Cortex-M33 |
| Code MRAM | 0x02080000，512KB |
| SRAM 映射区 | 0x220EA000，936KB（0xEA000） |
| TCM | CTCM 64KB + STCM 64KB（本核私有，零等待，不缓存） |
| C-Cache / S-Cache | 16KB（开）/ 16KB（**关**） |
| 缓存一致性风险 | **无**（S-Cache 关） |
| DMA/DTC 安全区 | 普通 `.bss` 即可（无需 nocache 段） |

# 四、深度对比分析

## 4.1 内存映射对比

| 对比项 | CPU0（Cortex-M85） | CPU1（Cortex-M33） |
|--------|-----|------|
| **Code MRAM 基址** | 0x02000000，512KB | 0x02080000，512KB |
| **SRAM 映射区基址** | 0x22000000，936KB | 0x220EA000，936KB |
| **TCM 容量** | ITCM 128KB + DTCM 128KB | CTCM 64KB + STCM 64KB |
| **段划分结构** | 完全一致 | 完全一致（仅物理基址不同） |

## 4.2 Cache 配置对比

| 对比项 | CPU0（Cortex-M85） | CPU1（Cortex-M33） |
|--------|-----|------|
| **指令缓存** | I-Cache 16KB（开） | C-Cache 16KB（非 TZ 构建下开） |
| **数据缓存** | D-Cache 16KB（**开**） | S-Cache 16KB（**关**） |
| **缓存一致性风险** | **有**（D-Cache 缓存 SRAM） | **无**（S-Cache 关） |
| **DMA/DTC 缓冲要求** | 必须落 MPU nocache 段 | 普通 `.bss` 即可 |

## 4.3 缓存一致性风险对比

- **CPU0（D-Cache 开）**：DTC 是总线主设备直接读 SRAM（绕过 D-Cache），开 D-Cache 后 CPU 写入的向量表/传输信息可能滞留 cache，DTC 读到旧值。DMA 同理。**必须**把缓冲改到 nocache 段，或搬运后 `SCB_CleanDCache()`。
- **CPU1（S-Cache 关）**：SRAM 访问直通，DMA/DTC 与 CPU 视图天然一致，**无一致性问题**。当前 `g_master_tx_buff` / `g_slave_rx_buff` 在普通 `.bss`，无需改。

## 4.4 SRAM 各段用法要点

普通 cacheable 段（`.data`/`.bss`/堆/栈/TLS）不要放 DMA/DTC 缓冲；DMA/DTC 缓冲首选 `__ram_zero_nocache$$` / `__ram_noinit_nocache$$`（MPU non-cacheable）；DTC 向量表启用时把 `DTC_CFG_VECTOR_TABLE_SECTION_NAME` 改为 `.ram_noinit_nocache`。

# 五、DMA/DTC 缓冲选型指南

| 应用场景 | 推荐落点 | 原因 |
|----------|----------|------|
| **CPU0 DMA/DTC 数据缓冲** | `__ram_zero_nocache$$` / `__ram_noinit_nocache$$` | D-Cache 开，只有 nocache 段是 DMA/DTC 安全区 |
| **CPU1 DMA/DTC 数据缓冲** | 普通 `.bss` | S-Cache 关，无一致性问题，无需改 |
| **CPU0 DTC 向量表** | `.ram_noinit_nocache` | 向量表在 SRAM 不在 nocache 保护区，开 D-Cache 后自身有风险 |
| **本核高频访问变量 / ISR 栈** | DTCM（CPU0）/ STCM（CPU1） | TCM 天然不缓存，零等待，本核私有 |
| **TCM 用于 DMA/DTC** | **禁止** | TCM 带 ECC、交叉开关延迟、容量小，DMAC/DTC1 无法访问 |
| **两核共用 DMA 缓冲** | CPU0 的 nocache 段 | CPU0 直通内存、CPU1 无 cache，双向一致 |

# 六、总结

| 对比项 | CPU0（Cortex-M85） | CPU1（Cortex-M33） |
|--------|-----|------|
| **核心定位** | 双核主核 @ 1GHz | 双核从核 |
| **Code MRAM** | 0x02000000，512KB | 0x02080000，512KB |
| **SRAM 映射区** | 0x22000000，936KB | 0x220EA000，936KB |
| **TCM** | ITCM 128KB + DTCM 128KB | CTCM 64KB + STCM 64KB |
| **指令缓存** | I-Cache 16KB（开） | C-Cache 16KB（非 TZ 开） |
| **数据缓存** | D-Cache 16KB（**开**） | S-Cache 16KB（**关**） |
| **一致性风险** | **有** | **无** |
| **DMA/DTC 安全区** | nocache 段 | 普通 `.bss` |
| **一句话理解** | **开 D-Cache 后整个 SRAM 被缓存，DMA/DTC 必须躲进 MPU nocache 段** | **S-Cache 从不打开，SRAM 直通，DMA/DTC 随便放普通 bss** |

**最终决策要点：**

- **看核**：CPU0 开 D-Cache → DMA/DTC 缓冲进 nocache 段；CPU1 S-Cache 关 → 普通 `.bss` 即可
- **看风险**：CPU0 当前 nocache 段全为 0 字节，`bsp_init_mpu()` 跳过所有 0 尺寸 region → 实际**没有 non-cacheable 保护区**，一旦用 DMA/DTC 必须改到 nocache 段
- **看 TCM**：TCM 只留给本核高频访问变量 / ISR 栈 / RAM 内执行代码，**绝不用于 DMA/DTC**（ECC 64 位写、交叉开关延迟、容量小）
- **看 DTC 双核**：DTC 硬件只有 1 颗，非 TZ 双核下两核都写 `DTCVBR_SEC` 会互踩 → 更简单的分工是 DTC 只给一个核，另一个核用 DMAC

> 参考文件：CPU0 D-Cache/MPU 使能 `system.c`（524-528、676-736）；CPU1 C-Cache 使能条件 `bsp_clocks.c:2265`；CPU1 DMA 缓冲 `spi_ep.c:31/34`；DTC 向量表位置手册 18.3.1。
