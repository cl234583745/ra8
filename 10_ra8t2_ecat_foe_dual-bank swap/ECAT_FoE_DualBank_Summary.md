十、dual-bank swap升级深度对比分析---RA8T2 ECAT FoE
===
[toc]

# 一、背景
- RA8T2 ECAT FoE例程分析与传统dual-bank有明显差异。
  - **硬件平台**：Renesas RA8T2 MCU，Cortex-M85 内核，主频最高 1 GHz
  - **EtherCAT 接口**：片内 ESC（EtherCAT Slave Controller）符合 ETG5003 规范，通过 Beckhoff SSC v5.12 协议栈实现
  - **非易失存储**：片内 Code MRAM，容量 **1 MB**（地址 `0x0200_0000` ~ `0x020F_FFFF`）
  - **需求**：通过 **FoE（File over EtherCAT）** 协议实现在线固件升级（OTA），升级过程需保证异常断电后可恢复（安全升级）
- **传统思路**：传统dual-bank支持硬件级别swap，用户无感使用。
- **当前方案**：基于 Renesas **Startup Area Select（SAS）** 寄存器的双 Bank 固件交换方案，属于软硬件结合方案。

# 二、当前方案实现（SAS 方案）

## 2.1 方案概述

SAS（Startup Area Setting，启动区域设置）是 RA8T2 MCU 提供的一种地址重映射机制。通过配置 SAS 寄存器，可以使 MCU 在复位后从两套物理 Bank 中的某一套启动，从而实现在线升级。

核心思路：**编译两份固件**，分别链接到不同的物理地址，运行中的固件通过 FoE 将新固件写入**非活动 Bank**，然后通过修改 SAS 寄存器并复位，切换到新固件。

## 2.2 编译与构建

项目包含两套并行的构建输出目录：

| 目录 | 编译宏 | 链接脚本 | 链接基址 | 输出固件 |
|------|--------|---------|---------|---------|
| `BANK0/` | `-DMRAM_BANK0` | `script/fsp_bank0.ld` | `0x02005000` | `ECATFW_RA8T2_B00.efw` |
| `BANK1/` | `-DMRAM_BANK1` | `script/fsp_bank1.ld` | `0x02082800` | `ECATFW_RA8T2_B01.efw` |

链接脚本采用分层引用结构（`fsp_bank0.ld` → `fsp_mram_bank.ld`），通过 `BANK_OFFSET` 参数控制 FLASH 区域的起始地址：

```
BANK0: BANK_OFFSET = 0K
BANK1: BANK_OFFSET = SINGLE_BANK_LENGTH  (= 502 KB)
```

`FLASH` 区域原点计算：
`FLASH_ORIGIN = FLASH_START + RESERVED_AREA_LENGTH + BANK_OFFSET`

即 `0x02000000 + 0x5000 + BANK_OFFSET`。

> 注①：`RESERVED_AREA_LENGTH = 16 KB（SUA0 + SUA1）+ 4 KB（Backup）= 20 KB = 0x5000`。`SINGLE_BANK_LENGTH = (1 MB - 20 KB) / 2 = 502 KB = 0x7D800`。

## 2.3 MRAM 固件布局

```
地址范围                     | 内容         | 大小
0x0200_0000 ~ 0x0200_1FFF   | SUA0         |  8 KB
0x0200_2000 ~ 0x0200_3FFF   | SUA1         |  8 KB
0x0200_4000 ~ 0x0200_4FFF   | Backup (SEMI)|  4 KB
0x0200_5000 ~ 0x0207_D7FF   | BANK0 固件   | ~502 KB
0x0207_D800 ~ 0x0207_FFFF   | (Bank0 剩余)  | ~10 KB
----------------------------+--------------+--------
0x0208_0000 ~ 0x0208_27FF   | (Bank1 SUA)  |  10 KB（未使用）
0x0208_2800 ~ 0x020F_FFFF   | BANK1 固件   | ~502 KB
```

- **SUA0 / SUA1（Startup Area）**：各 8 KB，包含复位向量和启动代码。SAS 寄存器决定复位后使用 SUA0 还是 SUA1
- **Backup 区域**：4 KB，用于 EtherCAT 协议栈的存储参数备份（Store Parameter，索引 0x10F0）
- **BANK0 / BANK1**：各约 502 KB，存放应用程序和 EtherCAT 协议栈代码

## 2.4 启动流程

```
MCU 复位
  │
  ▼
读取 SAS 选项寄存器 → 确定 SUA（SUA0 或 SUA1）
  │
  ▼
执行 SUA 中的启动代码
  │
  ├── 检查当前活动 Bank 的有效性（通过 FLASH 中的标识信息）
  ├── 如果有效 → 跳转到 BANKx 的 main 入口
  └── 如果无效 → 尝试切换至另一个 Bank → 复位
  │
  ▼
hal_entry() → 初始化硬件 → 启动 EtherCAT 协议栈
  │
  ▼
等待 EtherCAT 主站连接 → 正常运行
```

项目中的启动代码（SUA）由 Renesas FSP BSP 生成并固化在 FLASH_STARTUP 区域，应用层代码（`hal_entry.c`）全部使用 `PLACE_IN_RAM_SECTION` 属性指定从 RAM 执行。

## 2.5 FoE 升级流程

```
  EtherCAT 主站                        EtherCAT 从站（RA8T2）
       │                                      │
       │  1. FoE Write Request                 │
       │  (filename: ECATFW_RA8T2_B01.efw)     │
       │─────────────────────────────────────►  │
       │                                      │
       │  2. FoE Data (S-record)              │
       │  (Motorola S3 format, .srec)         │
       │─────────────────────────────────────►  │
       │                                      │
       │  3. S-record 解析                    │
       │      └─ r_fw_up_buf.c:              │
       │         ASCII → Hex, 校验和检查      │
       │      └─ r_fw_up_ra.c:               │
       │         analyze_and_write_data()     │
       │         write_firmware() →           │
       │         R_MRAM_Write()               │
       │                                      │
       │  4. 新固件写入 非活动 Bank            │
       │     (BANK0运行时 → 写入 BANK1)        │
       │                                      │
       │  5. 写入完成 → 读取 Identify         │
       │     更新 SII EEPROM Revision No.     │
       │                                      │
       │  6. BL_Reboot()                     │
       │     └─ R_MRAM_StartUpAreaSelect()    │
       │     └─ NVIC_SystemReset()            │
       │                                      │
   ◄───┼──────────────────────────────────────│
       │  7. 复位后 SAS 切换到新 Bank          │
       │     旧 BANK0 → 非活动，新 BANK1 → 活动 │
```

### 文件名校验（防误刷）

`samplefoe.c` 中定义了文件名头匹配规则：

```c
// MRAM_BANK0 编译时（从 BANK0 运行）：期望接收 "ECATFW_RA8T2_B01"（BANK1 固件）
// MRAM_BANK1 编译时（从 BANK1 运行）：期望接收 "ECATFW_RA8T2_B00"（BANK0 固件）
```

文件名不匹配时返回 `ECAT_FOE_ERRCODE_DISKFULL` 拒绝写入，防止误刷。

### 地址校验机制

`fw_up_check_addr_value()` 在 `r_fw_up_ra.c:200`：

- 当在 BANK0 中运行时：只允许写入 `FW_UP_BANK1_ADDR` ~ `FW_UP_BANK1_ADDR + FW_UP_APPLI_SIZE` 范围内的地址
- 当在 BANK1 中运行时：只允许写入 BANK0 地址范围
- SUA 区域的写入地址会被自动偏移（`*p_addr_value += FW_UP_SUA0_SIZE`）

## 2.6 SAS 寄存器详解

SAS 选项寄存器的地址映射在 `OPTION_SETTING_SAS_START = 0x02C9F074`（`memory_regions.ld`）。

```
R_MRAM_StartUpAreaSelect()
    │
    ├─ 参数: block (FLASH_STARTUP_AREA_BLOCK0 / BLOCK1)
    ├─ 操作: 写入 SAS 选项寄存器
    └─ 生效: 下次硬件复位后生效
```

SAS 寄存器的 BT（Bank Toggle）位——`R_MRMS->MSUASMON_b.BTFLG`——指示当前启动的是哪个 Bank：

| BTFLG 值 | 当前启动区域 | 物理地址范围 |
|----------|------------|------------|
| 0 | STARTUP_AREA_BLOCK0 | SUA0 位于 `0x0200_0000` |
| 1 | STARTUP_AREA_BLOCK1 | SUA1 位于 `0x0200_2000` |

`BL_Reboot()` 根据 BTFLG 决定切换到另一个 Bank：

```c
void BL_Reboot(void)
{
    if(R_MRMS->MSUASMON_b.BTFLG == 1)
    {
        R_MRAM_StartUpAreaSelect(&g_mram0_ctrl, FLASH_STARTUP_AREA_BLOCK0, false);
    }
    else
    {
        R_MRAM_StartUpAreaSelect(&g_mram0_ctrl, FLASH_STARTUP_AREA_BLOCK1, false);
    }
    NVIC_SystemReset();
}
```

## 2.7 代码从 RAM 执行

该项目的一个关键设计：**EtherCAT 协议栈代码和应用代码均在 RAM 中执行**。这在 `fsp_mram_bank.ld` 的 `.ram_from_flash` 段中体现：

```ld
__ram_from_flash$$ :
{
    __ram_vector_start = .;
    KEEP(*(.isr_vector))
    __ram_vector_end = .;

    /* EtherCAT 协议栈全部 .text 和 .rodata 复制到 RAM */
    */bootmode.o(.text*)
    */coeappl.o(.text*)
    */ecatappl.o(.text*)
    */ecatfoe.o(.text*)
    ...（全部 EtherCAT 相关模块）

    /* 所有 .data 段 */
    *(.data*)
    *(vtable)
}> RAM AT > FLASH
```

同时 `hal_entry.c` 中所有关键函数标记为 `BSP_PLACE_IN_SECTION(".ram_from_flash")`。

> 注②：代码从 RAM 执行的原因：当 Flash 编程（`R_MRAM_Write`）正在进行时，不能从 Flash 取指令。EtherCAT 栈需要在升级过程中继续运行（保持 EtherCAT 通信在线）。因此必须将所有运行时所需的代码预先复制到 RAM 中执行。

# 三、与传统 MCU 硬件 Dual-Bank 方案的区别

## 3.1 传统硬件 Dual-Bank（如 STM32G4/ra6m5）

- **硬件 Flash 控制器**原生支持将 Flash 分为两个物理 Bank
- 通过 **Option Bytes** 设置活动 Bank
- 应用代码**只需编译一次**，链接到固定的地址（如 `0x08000000`）
- 硬件 Bank Swap 本质是**物理地址与系统地址的交换**：
  - Bank 0 物理 `0x08000000`，Bank 1 物理 `0x08040000`
  - 交换后 Bank 1 出现在 `0x08000000`，Bank 0 出现在 `0x08040000`
- 代码无需感知 Bank 切换——永远从 `0x08000000` 执行

## 3.2 RA8T2 SAS 方案

- **没有硬件双 Bank Flash 控制器**——`R_MRAM_BankSwap()` 返回 `FSP_ERR_UNSUPPORTED`
- 通过 **SAS 选项寄存器** 实现软件级别的地址重映射
- 需要**两份编译**，分别链接到不同物理地址
- SUA（Startup Area）和 Backup 区域**不重复**——仅存在于低 512KB 地址空间（`0x02000000` ~ `0x02004FFF`）
- 切换后 CPU 地址空间重映射，但代码链接地址不变 → 通过 RAM 执行避开地址不一致问题

## 3.3 对比总结

| 特性 | 硬件 Dual-Bank（STM32G0） | SAS 方案（RA8T2） |
|------|--------------------------|------------------|
| 编译次数 | **1 次**（固定地址） | **2 次**（不同物理地址） | 2 次（不同链接地址） |
| 硬件支持 | 专用 Flash 控制器 | SAS 选项寄存器 |
| Bank 切换粒度 | 整个 Bank（**固定**大小） | 由 SAS 配置（**软件**配置） |
| 切换生效 | 即时（通过 Option Bytes） | 需硬件复位 |
| 代码执行位置 | Flash（固定地址） | RAM（需初始化复制） |
| 链接地址要求 | 固定（Bank 无关） | 每 Bank 不同 |
| Startup 区域 | 每个 Bank 独立 | 共享 SUA0/SUA1 |
| 固件兼容性 | 新旧固件完全**兼容**（同地址） | 严格绑定 Bank（交叉**不兼容**） |
| 复杂度 | 低 | 中等 |

# 四、优缺点分析

## 4.1 方案对比表格

| 对比项 | SAS Swap（当前方案） | 硬件 Dual-Bank | MCUboot 纯软件方案 |
|--------|-------------------|---------------|-------------------|
| **编译次数** | **2 次**（不同链接地址） | **1 次** | **1 次** |
| **硬件依赖** | SAS 选项寄存器 | 专用 Flash 控制器 | 无特殊硬件要求 |
| **Bank 数量** | 2 个（502 KB × 2） | 2 个（固定大小） | 可任意分区 |
| **升级失败恢复** | 依赖 SUA 自检机制 | 硬件保证可选择 Bank | Image Trailer + 回滚 |
| **固件签名/验证** | 无 | 无 | 支持（MCUboot 内置） |
| **开发复杂度** | 中等（需双编译、双调试配置） | 低 | **高**（需 Bootloader 开发） |
| **灵活性** | 中（分区在链接脚本中配置） | 低（Bank 大小固定） | **高**（分区、Slot 可配） |
| **升级速度** | 中等（通过 FoE 接收后写入） | 同左 | 同左 |
| **资源占用** | 几乎无额外 Flash（Boot 代码在 SUA） | 极少 | **~50 KB**（Bootloader 自身） |
| **RAM 占用** | **大**（全部协议栈驻留 RAM） | 小 | 按需加载 |
| **交叉版本兼容** | **差**（Bank0/Bank1 固件交叉不兼容） | 好 | 好 |
| **OTA 期间通信保持** | 是（RAM 执行） | 否（需停机切换） | 否（需复位跳转） |
| **量产维护** | **复杂**（需管理两套固件镜像） | 简单 | 简单 |

## 4.2 SAS 方案的优势

1. **无额外 Bootloader 专用区域**：启动代码嵌入在 SUA 区域（8 KB × 2），无需像 MCUboot 那样预留 ~50 KB 的 Bootloader 分区
2. **升级过程可保持 EtherCAT 通信**：代码从 RAM 执行，Flash 写入期间不影响 EtherCAT 协议栈运行
3. **实现简单直接**：FoE 接收 → 写入 → 切换 → 复位，逻辑清晰
4. **利用 SAS 硬件机制**：不需要 Flash 控制器级别的 Bank Swap 支持，对 MRAM 这类新兴存储介质友好

## 4.3 SAS 方案的缺陷与局限性

1. **两套编译，交叉版本不兼容**
   - BANK0 固件和 BANK1 固件不能互换位置运行
   - 每次固件发布都需要生成两份 `.efw` 文件（`ECATFW_RA8T2_B00.efw` 和 `ECATFW_RA8T2_B01.efw`）
   - 文件名硬编码在 `samplefoe.c` 的 `aFileNameHeader` 中
   - 如果发布流程遗漏某份固件，会导致升级后无法再次升级

2. **代码必须从 RAM 执行**
   - 大量使用 `PLACE_IN_RAM_SECTION` 和 `.ram_from_flash` 段
   - 增加了启动时的复制开销和 RAM 占用（~200+ KB）
   - RA8T2 RAM 空间充足（~1.9 MB），但对于 RAM 紧张的 MCU 不适用

3. **无固件签名/验证机制**
   - 不校验固件的完整性和来源
   - 写入过程仅靠 S-record 校验和（单 record）保证数据完整性
   - 无法防止恶意固件写入

4. **升级失败恢复能力有限**
   - 依赖 SUA 自检机制的健壮性
   - 如果在切换 Bank 前断电，可能出现两个 Bank 都无效的情况
   - 没有固件回滚（Rollback）机制
   - 没有固件版本管理（仅通过 SII EEPROM Revision Number 记录）

5. **Debug 配置复杂**
   - 需要两套 Debug 启动配置（`.launch` 文件分别对应 BANK0 和 BANK1）
   - 调试时需要指定正确的程序计数器地址
   - 调试切换 Bank 后的代码需要额外的 GDB 命令

# 五、从 SAS 方案到 MCUboot 的思考

## 5.1 SAS 方案的本质

SAS Dual-Bank 方案本质是**利用 MCU 的地址重映射硬件机制 + 两套编译 + RAM 执行**的组合技巧实现固件升级。

它的核心特点是：
- **Bootloader = SUA 中最小化启动代码**（由 Renesas FSP BSP 管理）
- **应用程序 = 包含升级逻辑的 EtherCAT 协议栈**（升级代码嵌入在应用层）
- **恢复机制 = SAS 切换 + 复位**

这与传统的**独立 Bootloader + 单编译 Application** 架构有本质不同。SAS 方案将升级逻辑与业务逻辑强耦合，而独立 Bootloader 方案将二者解耦。

## 5.2 适用场景

| 场景 | SAS 方案适用性 |
|------|--------------|
| 快速原型验证 | ✅ **非常适合**——实现简单，无额外框架 |
| 量产小规模 | ⚠️ **可接受**——需规范的固件发布流程 |
| 量产大规模 | ❌ **不推荐**——两套编译增加出错概率 |
| 安全关键应用 | ❌ **不满足**——缺少签名验证和防回滚 |
| 远程无人升级 | ❌ **风险高**——回滚和恢复机制不足 |
| 多版本共存管理 | ❌ **不适用**——交叉版本严格绑定 |

## 5.3 从 SAS 方案理解 MCUboot

SAS 方案的局限性驱动了对更通用方案的调研：

| MCUboot 特性 | 解决的问题 |
|-------------|-----------|
| 单编译 | 消除交叉版本兼容问题 |
| 镜像签名验证（imgtool） | 保证固件来源可信 |
| Image Trailer（metadata） | 升级状态持久化，掉电恢复 |
| Swap / Overwrite / Direct-XIP | 灵活的分区策略 |
| 固件版本号 | 防止降级攻击（Downgrade Prevention） |
| 安全启动（Chain of Trust） | 构建从 ROM → Bootloader → App 的信任链 |
| 回滚计数器 | 避免重复回滚攻击 |

MCUboot 代表了一种**与硬件无关的通用升级框架**，而 SAS 方案是**利用特定 MCU 特性的简化实现**。理解 SAS 方案的原理对于理解 MCUboot 的设计动机非常有帮助——许多 MCUboot 的设计决策正是为了解决 SAS 方案中暴露的问题（如单编译、签名验证、版本管理和回滚保护）。

# 六、总结
- RA8T2 的 SAS 方案是一种**硬件辅助的软件级双 Bank 实现**，通过 SAS 寄存器和两套编译实现固件升级
  - 优势：实现简单，升级期间保持通信，无 Bootloader 专用区域
  - 缺点：两套编译，交叉版本不兼容，无签名验证，恢复能力有限
- 与传统硬件 Dual-Bank 方案相比，SAS 方案更依赖软件和 RAM 执行技巧
- SAS 方案暴露了对**通用升级框架**（如 MCUboot）的需求——解决交叉版本兼容、签名验证、版本管理和回滚保护等问题
- 对比rzn2l ecat foe dual-bank swap(纯软)安全性有所增加，主要由于SAS硬件和MRAM写入时间极短决定。
- 预告下一节分析mcuboot的实现原理和优势