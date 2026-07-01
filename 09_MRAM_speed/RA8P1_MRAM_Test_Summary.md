九、RA8P1 MRAM速度测试和各种Flash技术对比
===

[toc]
# 一、背景

- Renesas RA8P1 是首款搭载 Cortex-M85 内核的 MCU，主频可达 1 GHz。其片内集成了 **Code MRAM**（磁阻随机存取存储器）作为非易失存储介质，替代传统的嵌入式闪存（eFlash）。
- 本文档记录了对 RA8P1 Code MRAM 写入/读取性能的实际测试结果、与 DCache 的兼容性问题，以及系统总线数据路径分析。
- 扩展对比eFlash（NOR）、MRAM（STT-MRAM）、RRAM、PCM（PRAM）、FRAM（FeRAM）

# 二、测试环境配置
- 时钟配置

| 时钟域 | 频率 | 来源 |
|--------|------|------|
| CPU（内核） | 1 GHz | PLL1P ÷ 1 |
| MRICLK（MRAM 总线时钟） | 250 MHz | PLL1P ÷ 4 |
| MRPCLK（MRAM 外设时钟） | 125 MHz | PLL1P ÷ 8 |

来源：`ra_gen/bsp_clock_cfg.h`

- MRAM 硬件规格

来源：RA8P1 用户手册 R01UH1064EJ0110 §60

| 参数 | 值 |
|------|-----|
| Code MRAM 容量 | 最大 1 MB（地址 `0x0208_0000`） |
| 编程数据缓冲区 | 32 字节 |
| 写入触发条件 | 缓冲区满（32 B）或 MRCFLR.MRCFL 强制刷新 |
| 总线接口 | MRC0BI（Code MRAM Bus Interface） |
| 额外 MRAM | MRE0BI，只读接口 |
| 最大总线频率 | 500 MHz（MRICLK） |

- 计时方法

DWT 循环计数器（`DWT->CYCCNT`）在主频 1 GHz 下以 CPU 周期计数，1 周期 = 1 ns。计数器为 32 位，约每 4.3 秒回绕一次（2³² ns）。所有测试均在回绕时间内完成，无需处理溢出。

# 三、写入速度测试

## 3.1 时序公式（手册 §70.16.1 Table 70.121）

- Normal 模式：`tPMC(Typ) = 137.8/FMRICLK + 6.452 µs`（每 32 B）
- High-speed 模式：`tPMC(Typ) = 137.8/FMRICLK + 4.452 µs`（每 32 B）

High-speed 模式由 FSP 驱动 `r_mram.c:907` 中设置 `MRPSC.MHSPEN = 1` 启用。

MRICLK = 250 MHz 时理论值：**~5.0 µs/32 B → ~6.4 MB/s**

## 3.2 实测结果

| 数据模式 | 写入速度（MB/s） | 说明 |
|---------|----------------|------|
| 递增（种子） | 6.07 | 混合比特翻转（约 50%），最慢 |
| 全 0xFF（擦除态→写入） | 8.19 | 翻转最少，最快 |
| 全 0x00（上一轮后） | 7.08 | 介于之间 |

速度差异的根本原因：MRAM 写入时间与**比特翻转方向**相关。0→1 翻转最快，1→0 次之，混合方向最慢。手册的 Typ 值（~6.4 MB/s）假定 50% 比特翻转。


## 3.3 读取速度

| 操作 | 512 KB 耗时 | 等效带宽 |
|------|-----------|---------|
| memcpy（MRAM → SRAM） | ~18.9 ms | ~26.4 MB/s |
| CRC-32 查表 | ~35.7 ms | ~14.3 MB/s |

读取速度固定，与数据内容无关。读取期间 DCache 关闭，实测为纯 MRAM 访问速度。

## 3.4 测试代码结构

文件：`src/mram_test.c`

```
hal_entry()
  └─ mram_auto_test()
       ├─ 读取种子（0x020FF000）
       ├─ 可选：关闭 DCache
       ├─ 构建 CRC-32 表
       ├─ test_pattern("incremental", fill_inc, seed)     ← 写 + 读 + CRC
       ├─ test_pattern("all-0xFF",    fill_const, 0xFF)
       ├─ test_pattern("all-0x00",    fill_const, 0x00)
       ├─ 种子 +1 写回 MRAM（关中断保护）
       └─ 可选：恢复 DCache
```

- 计时：DWT 周期计数器（1 GHz 下 1 周期 = 1 ns）
- 块大小：1 KB（512 次写操作 = 512 KB 总写量）
- 验证：CRC-32 查表，写入前计算源 CRC，读取后对比
```
00> ===== MRAM Write/Read Test =====
00> FW: 512 KB @ 0x02080000, Chunk: 1024 B, Seed: 0x23
00> 
00> --- Pattern: incremental (seed) ---
00>   Write 1KB: 158 us = 6329 KB/s (6.180 MB/s)
00>   Write all: 82047 us = 6240 KB/s (6.093 MB/s)
00>   Read: 18947 us = 27022 KB/s (26.388 MB/s)
00>   CRC: 35681 us, check: PASS
00> 
00> --- Pattern: all-0xFF ---
00>   Write 1KB: 115 us = 8695 KB/s (8.491 MB/s)
00>   Write all: 61067 us = 8384 KB/s (8.187 MB/s)
00>   Read: 18947 us = 27022 KB/s (26.388 MB/s)
00>   CRC: 35680 us, check: PASS
00> 
00> --- Pattern: all-0x00 ---
00>   Write 1KB: 135 us = 7407 KB/s (7.233 MB/s)
00>   Write all: 70601 us = 7252 KB/s (7.082 MB/s)
00>   Read: 18947 us = 27022 KB/s (26.388 MB/s)
00>   CRC: 35681 us, check: PASS
00> 
00> ===== Done =====
```

# 四、DCache 兼容性问题

## 4.1 症状

```
R_MRAM_Write() → FSP_ERR_IN_USE（错误码 8）于 r_mram.c:317
```

错误条件为 `R_MRMS->MRCPS & (ABUFFULL | PRGBSYC)` 在上一次写入后未清零。

## 4.2 根因分析

MRAM 的写入机制与传统 Flash 不同：它通过**内存映射 AXI store**（`*dest = *src`）将数据送入 32 字节编程数据缓冲区，然后在 P/E（Program/Erase）模式下写入存储单元。**数据必须在 P/E 模式窗口内通过 AXI 总线到达 MRAM 控制器**。

RA8P1 总线系统包含四级缓存（Figure 15.1, §15）：

```
CPU 核 → [D-Cache] → CPU0MAXIBI → [C-Cache] → [S-Cache] → 总线矩阵 → MRC0BI → MRAM
```

- **D-Cache（ARM L1 Data Cache, 16 KB）**: 在 CPU 核内部，Cortex-M85 标配
- **C-Cache（Code-bus Cache, 16 KB）**: Renesas 自定义 IP，仅缓存代码总线
- **S-Cache（System-bus Cache, 16 KB）**: Renesas 自定义 IP，缓存系统总线

三种状态的对比（§15 Figure 15.1, §2.16, §60.4）：

| DCache 状态 | AXI 写事务 | MRAM 编程结果 |
|------------|-----------|--------------|
| **关闭** | store 直达总线矩阵 → MRC0BI → MRAM 缓冲区 | ✅ 正常写入 |
| **开启 + Write-Back（默认）** | store 被 D-Cache 吸收，不发总线 | ❌ Error 8 |
| **开启 + FORCEWT=1** | store 经 Write-Through 走总线，但 write buffer 可能延迟/合并 AXI 事务 | ⚠️ 可能失败 |

> 注意：`BSP_CFG_DCACHE_FORCE_WRITETHROUGH`（`MEMSYSCTL->MSCR.FORCEWT`，§63.2.4）默认 = 1，强制 D-Cache 使用 Write-Through 策略。但 Write-Through 仍经过 write buffer，连续字节 store 可能被合并延迟，无法保证在 P/E 模式窗口内到达 MRAM 控制器。

## 4.4 驱动的缺失

| 文件 | 是否有 DCache 处理 |
|------|------------------|
| `r_mram.c`（FSP 驱动） | ❌ 无。未调用任何 `CleanDCache` / `InvalidateDCache` |
| `mram_ep.c`（官方示例） | ❌ 无。仅 `__disable_irq()` / `__enable_irq()` |
| `mram_test.c`（本测试） | ✅ 测试前后关闭/恢复 DCache |

来源：FSP v5.9 `ra/fsp/src/r_mram/r_mram.c` 全文检索 "cache" 无匹配。

## 4.5 临时解决

```c
SCB_CleanInvalidateDCache();
SCB_DisableDCache();
// ... MRAM 写入操作 ...
SCB_EnableDCache();
```


# 五、嵌入式非易失存储技术对比

下表对比 MCU 片内嵌入式非易失存储技术。数据来源：RA8P1 手册 R01UH1064EJ0110、Renesas ISSCC 2024 STT-MRAM 论文、英飞凌 TC4xx RRAM 白皮书、意法半导体 Stellar P6 PCM 资料、JEDEC 标准。

| 参数 | eFlash（NOR） | MRAM（STT-MRAM） | RRAM | PCM（PRAM） | FRAM（FeRAM） |
|------|--------------|-----------------|------|------------|--------------|
| **存储原理** | 浮栅电荷存储 | 磁阻隧道结（MTJ） | 电阻态转变（氧空位） | 硫系化合物相变（晶态/非晶态） | 铁电体极化（PZT 膜） |
| **MCU 代表产品** | 传统 MCU（40 nm+） | RA8P1/T2/M2/D2（Renesas 22 nm）<br>S32K5（NXP 16 nm FinFET） | AURIX TC4x（英飞凌 28 nm）<br>nRF54L15（Nordic 22 nm ULL） | Stellar P6（ST 28 nm FD-SOI）<br>STM32V8 | MSP430 FRAM 系列（TI）、Ramtron |
| **写入方式** | 页擦除→页编程（FCU） | AXI store（`*addr=data`） | 位写入（无需擦除） | 位覆写 | 位写入（无需擦除） |
| **典型写速度** | ~0.1 MB/s | ~6 MB/s（实测） | 未公开（注①）<br>Nordic 实测：65 μs/word（单次），22 μs/word（顺序） | 未公开（注①） | **125 ns/word**（16 MB/s @ 8 MHz，注②） |
| **读速度** | 受 Wait State 限制 | ~26 MB/s（实测） | 未公开 | 快于 eFlash | SRAM 同等速度（系统时钟） |
| **耐久度（次）** | 10K~100K | 100K~1M | 100K~1M | 10K~100K（+） | 10^12~10^15（几乎无限） |
| **写入前擦除** | 需要 | **不需要** | **不需要** | **不需要** | **不需要** |
| **比特覆写** | ❌ | ✅ | ✅ | ✅ | ✅ |
| **数据保持** | 20 年 @ 85°C | 10 年 @ 125°C | 10 年 @ 85°C | 10+ 年 @ 125°C | 10+ 年 @ 85°C |
| **最小制程** | 40 nm（受限） | 22 nm / 16 nm FinFET | 28 nm | 28 nm FD-SOI | 130 nm~500 nm（密度受限） |
| **存储密度** | 高 | 中/高 | 中 | 中（单元面积最小，注③） | **低**（通常 ≤16 Mb） |
| **代表产品/官方资料** | — | [Renesas RA8P1](renesas.cn/zh/products/ra8p1)<br>[NXP S32K5](nxp.com.cn/products/S32K5) <br>[16纳米FinFET嵌入式MRAM](nxp.com.cn/company/about-nxp/newsroom/NW-NXP-AND-TSMC-DELIVER-FIRST16NM-FINFET-MRAM)| [Infineon 新闻稿](https://www.infineon.com/technology-news/2022/infatv202211-031)（明确 RRAM）<br>[产品概览 PDF](https://www.infineon.com/assets/row/public/documents/10/156/infineon-tc4x-overview-productpresentation-en.pdf)（"RRAM-NVM"）<br>[Nordic nRF54L15](https://www.nordicsemi.com/Products/nRF54L15)（TSMC 22ULL eReRAM） | [ST Stellar P6](https://www.st.com.cn/zh/automotive-microcontrollers/stellar-p-motion-control.html)<br>[STM32V8](st.com.cn/content/st_com/zh/campaigns/stm32v8-high-performance-cortex-m85-mcu-z11.html) | [MSP430FR2422 数据手册](https://www.ti.com.cn/lit/pdf/SLASEE5)（tWRITE）<br>[SLAA498B 写速度应用报告](https://www.ti.com/lit/an/slaa498b/slaa498b.pdf)（125 ns/word） |
| **是否需要 DCache 特殊处理** | 否（FCU 写） | **是** | 待评估 | 待评估 | 否（CPU 本地总线） |
| **核心优势** | 技术成熟、成本低 | 无需擦除、密度与速度均衡、适合代码存储 | 结构最简单、可扩展至先进制程、位写入 | 单元面积最小（注③）、耐温最高（165°C） | 写入最快（125 ns/word，同 SRAM）、耐久度几乎无限（10^15+） |

> 注①：RRAM（Infineon TC4x）和 PCM（ST Stellar P6/STM32V8）官方文档**未公开明确写入时序**（µs/byte）。Infineon 仅声明"RRAM 使用与 eFlash 相同的软件接口，功能行为一致"；ST 仅描述"fast read/write speeds with single-bit alterability"。两家的详细数据手册需 NDA 或注册访问，这与 Renesas 在 RA8P1 手册中明确给出 `tPMC` 时序公式形成鲜明对比。
> 
> Nordic nRF54L15 的 RRAM 写入速度通过第三方分析（Argenox）获得：单次写入 65 μs/word，顺序地址写入 22 μs/word（比 nRF52 Flash 的 41 μs/word 快约 50%）。Nordic 官方数据手册仅标注"non-volatile memory (RRAM)"，未提供详细时序参数。
>
> 注②：FRAM 写入速度来自 TI 官方应用报告 SLAA498B（Maximizing Write Speed on the MSP430 FRAM）Table 1：**125 ns/word**（16 bit word）。FRAM 控制器在 8 MHz 以下是零等待（1 周期/word），超过 8 MHz 需插入等待周期（NWAITSx）。FRAM 本质是"与 SRAM 速度相同的 NVM"——写入速度受限于 CPU/总线频率，而非 FRAM 存储单元本身。耐久度 10^15 次、无需擦除、位写入。
>
> 注③：ST 官方博客（2026-01-29，STM32V8 发布）声明其 PCM 单元面积为 MRAM 和 RRAM 的一半——"our PCM allows us to halve the memory footprint compared to MRAM and RRAM, thus offering the smallest non-volatile memory cells in an MCU"。
>
> **FRAM 与 TI OptiFlash 的区别**：FRAM 是新型 NVM **介质**（铁电体存储），直接替代 eFlash/RAM 作为 MCU 片内统一存储器。TI OptiFlash 是**外置 NOR Flash + 硬件加速器**的组合方案（包括 RL2 缓存控制器、Flash 线性控制器 FLC、和执行加速器 RAT），并非新型 NVM 介质。两者是不同层面的技术：FRAM 改变存储介质本身，OptiFlash 改变访问架构。



# 六、待解决问题

1. **MPU 方案**：通过 `ARM_MPU_SetRegion()` 将 MRAM 标记为 Non-cacheable（无需关 DCache）
   - 第一次尝试：调用 `ARM_MPU_Disable()` 后触发 HardFault —— 原因：禁用 MPU 时会丢弃所有 BSP 预设的 MPU 区域，导致系统关键内存属性丢失
   - 第二次尝试：不调用 `ARM_MPU_Disable()`，仅执行 `ARM_MPU_SetRegion()` 后仍崩溃 —— 根因可能为：MRAM 的旧 DCache 行缓存了 Write-Back 数据，SetRegion 后属性变更但脏行未失效
   - 待测试修复：`ARM_MPU_SetRegion()` 前增加 `SCB_InvalidateDCache_by_Addr()` 清理 MRAM 范围内的缓存行（代码已写，未验证）

2. **读性能优化**：利用 MRAM Prefetch Buffer（MRCPFB）、Memory-to-Memory DMA 加速读取

3. **S-Cache 影响**：S-Cache（System-bus Cache, 16 KB）默认未启用（R_BSP_FlashCacheEnable() 仅启用 C-Cache）。若启用并设为 Write-Through 模式，需评估其对 MRAM 写入的影响

4. **CPU1（Cortex-M33）**：架构与 CPU0 相同（Figure 15.1），同样存在 D-Cache → CPU0MAXIBI → C-Cache → S-Cache → 总线矩阵 → MRC0BI 路径，预计 D-Cache 对 MRAM 写入的影响与 CPU0 一致，待实测确认

# 七、总结

1. RA8P1 Code MRAM 实测 AXI 写入速度 6.07~8.19 MB/s，符合手册公式理论值（~6.4 MB/s Typ，50% 翻转）；读取速度 ~26.4 MB/s。

2. D-Cache 开启时 MRAM P/E 模式写入会失败（Error 8），根本原因在于 store 被缓存吸收无法在 P/E 窗口内到达 MRAM 控制器。当前可靠方案：写入前关 DCache（`SCB_DisableDCache()`），写入后恢复。FSP 驱动及官方示例均未处理此问题，属于系统集成层面的文档与设计空白。

3. 行业全景——各主要厂商的新型 NVM 路线：
   - **MRAM**：Renesas（RA8P1/T2/M2/D2，22 nm）、NXP（S32K5，16 nm FinFET）
   - **RRAM**：Infineon（AURIX TC4x，28 nm）、Nordic（nRF54L15，TSMC 22ULL eReRAM）
   - **PCM**：ST（Stellar P6 28 nm FD-SOI、STM32V8 18 nm FD-SOI）
   - **FRAM**：TI（MSP430 系列，密度受限，主攻低功耗计量/RFID）
   - **外置 Flash + 加速器**：TI OptiFlash（非新型 NVM 介质）
   - **代工厂支撑**：TSMC 提供 22 nm/16 nm MRAM、28 nm/6 nm RRAM；GlobalFoundries 提供 22FDX eMRAM（Auto Grade 1，150°C，500K 次耐久）
   - **国产动态**：目前国产MCU尚无量产集成新型NVM的产品，但国产RRAM已取得突破——**新忆科技**（清华孵化，55 nm→22 nm RRAM，累计出货超2000万颗，良率>99%）、**昕原半导体**（28 nm RRAM量产线）、**合肥睿科微**（兆易创新合资，ISSCC 2026发表55 nm RRAM存算一体芯片，3D堆叠）。**中芯国际**40 nm RRAM已量产。国产MCU厂商正在与代工厂评估28~16 nm节点的RRAM/MRAM方案。
   新型NVM（无需擦除、按位覆写）正在各细分领域加速替代传统 eFlash。

4. 官方文档写速度对比（按 32 B 归一化）：
   - **FRAM（TI MSP430）**: 2 µs/32 B = **~16 MB/s**（125 ns/16 bit word，@8 MHz，SLAA498B）
   - **MRAM（Renesas RA8P1）**: 4.7 µs/32 B = **~6.4 MB/s**（tPMC high-speed 公式，R01UH1064 §70.16.1）
   - **RRAM（Nordic nRF54L15）**: 176 µs/32 B = **~0.18 MB/s**（22 µs/32 bit word 顺序写入，nRF54L15 v1.0 §11.16.1）
   - **PCM（ST 18 nm ePCM）**: 85 µs/32 B = **~0.38 MB/s**（~3 Mbps，ST 白皮书 "18nm FD-SOI and ePCM"）

