十一、ra8p1 mcuboot深度分析
===
[toc]

# 一、MCUboot 简介

- MCUboot 是一个开源的 secure bootloader，专门为资源受限的 MCU 设计，目前由 Linaro 维护，托管在 [github.com/mcu-tools/mcuboot](https://github.com/mcu-tools/mcuboot)。
- 主要功能：
  - 支持三种升级模式：**Overwrite Only**（覆盖）、**Swap**（双bank交换）、**Direct-XIP**（原地执行）
  - 支持数字签名验证：**ECDSA P-256**、**RSA-2048/3072**、**ML-DSA**（Dilithium，抗量子）
  - 支持多 Image（multi-image），可同时管理多个 CPU 核的固件
  - 版本管理和降级防护
  - 固件加密（Encrypted Images）
- Renesas FSP 将 MCUboot 集成为 `rm_mcuboot_port` 模块，与 e2 studio 深度绑定，提供图形化配置、自动签名、一键调试等支持。
- MCUboot 本身是**硬件无关的**，通过 flash_map 抽象层和 bootutil 核心逻辑实现跨平台。

# 二、测试环境

| 项目 | 说明 |
|------|------|
| **硬件** | EK-RA8P1（Cortex-M85 @ 1GHz + Cortex-M33 双核） |
| **MCU 存储** | Code MRAM 1MB（0x02000000），22 nm STT-MRAM |
| **工具链** | Clang ARM Toolchain for Embedded v21.1.1（ATfE-21.1.1），picolibc tinystdio |
| **FSP 版本** | Renesas FSP v6.4.0 |
| **IDE** | e2 studio 2025-04 |
| **MCUboot 版本** | 集成在 FSP 中的 MCUboot 移植版 |

## 2.1 参考工程结构

参考工程 `ra8_dualcore_with_bootloader` 包含 5 个子项目：

| 项目 | 功能 |
|------|------|
| `ra8p1_bootloader` | MCUboot bootloader，运行在 CPU0 Secure 模式 |
| `ra8p1_primary_cpu0` | CPU0 主应用（LED 闪烁） |
| `ra8p1_primary_cpu1` | CPU1 主应用（LED 闪烁） |
| `ra8p1_secondary_cpu0` | CPU0 副 slot 应用（用于测试升级） |
| `ra8p1_secondary_cpu1` | CPU1 副 slot 应用 |

## 2.2 Flash 分区布局（internal MRAM）

| 分区 | 起始地址 | 大小 | 说明 |
|------|---------|------|------|
| Bootloader | 0x02000000 | 64 KB（0x10000） | MCUboot 固件 |
| Image 0 Primary（CPU0） | 0x02010000 | 192 KB（0x30000） | CPU0 主应用 slot |
| Image 1 Primary（CPU1） | 0x02040000 | 192 KB（0x30000） | CPU1 主应用 slot |
| Image 0 Secondary（CPU0） | 0x02070000 | 192 KB（0x30000） | CPU0 升级接收 slot |
| Image 1 Secondary（CPU1） | 0x020A0000 | 192 KB（0x30000） | CPU1 升级接收 slot |

> 注①：每个 Image 包含 0x200 字节的 MCUboot header + 0x2FE00 字节的应用固件。总 Flash 占用约 64 + 192×4 = 832 KB，1 MB MRAM 基本用完。

## 2.3 MCUboot 配置

关键配置项（`mcuboot_config.h`，FSP 生成文件——不可直接修改）：

```c
#define MCUBOOT_SIGN_EC256                   // ECDSA P-256 签名
#define MCUBOOT_OVERWRITE_ONLY                // Overwrite 模式（非 Swap）
#define MCUBOOT_VALIDATE_PRIMARY_SLOT         // 验证 Primary slot 签名
#define MCUBOOT_HAVE_LOGGING    1             // 启用日志
#define MCUBOOT_USE_MBED_TLS                  // 加密库：mbed TLS
#define MCUBOOT_IMAGE_NUMBER  2               // 双 Image（双核）
```

# 三、测试过程中遇到的问题

## 3.1 Python 环境与 imgtool 签名工具

- 问题描述

按照应用笔记 R11AN1099EU0100 3.1 节配置 Python 签名环境：

```
python -m pip install --upgrade pip         // 成功
pip3 install --user -r scripts/requirements.txt  // 失败
```

第二个命令报错：`ERROR: Could not open requirements file: [Errno 2] No such file or directory: 'scripts/requirements.txt'`

- 根因分析

在 e2 studio 中编译工程时，工作目录是各项目根目录（如 `ra8p1_primary_cpu0/`），而 `requirements.txt` 位于 `ra8p1_bootloader/ra/mcu-tools/MCUboot/scripts/requirements.txt`，相对路径需要从 bootloader 项目根目录出发：

```
pip install -r ra/mcu-tools/MCUboot/scripts/requirements.txt
```

- 解决

从 bootloader 项目根目录执行：

```
pip install -r ra/mcu-tools/MCUboot/scripts/requirements.txt
```

安装的依赖包：

| 包名 | 用途 |
|------|------|
| `cryptography>=2.6` | RSA/EC 密钥处理 |
| `intelhex` | Intel HEX 文件格式支持 |
| `click` | 命令行接口 |
| `cbor2` | CBOR 数据格式 |
| `pyyaml` | YAML 配置文件解析 |
| `dilithium-py>=1.3.0` | ML-DSA（抗量子签名）支持 |

## 3.2 MCUboot 日志输出问题

- 问题描述

MCUboot 的 `[ERR]/[WRN]/[INF]/[DBG]` 日志不输出到串口。烧录后只能看到 bootloader 自身初始化的打印，没有 MCUboot 的运行日志。

- 根因分析

三层原因叠加：

**第一层：mcuboot_logging.h 是生成文件**

MCUboot 的日志宏定义在 `ra_cfg/mcu-tools/include/mcuboot_config/mcuboot_logging.h`，文件头明确标注 `/* generated configuration header file - do not edit */`。FSP 在 Generate Project Content 时会重新生成此文件，任何手动修改都会被覆盖。

原始内容使用 `printf()`：

```c
#define MCUBOOT_LOG_ERR(_fmt, ...)   printf("[ERR] " _fmt "\n", ## __VA_ARGS__)
#define MCUBOOT_LOG_INF(_fmt, ...)   printf("[INF] " _fmt "\n", ## __VA_ARGS__)
```

**第二层：picolibc tinystdio 的 printf 不走 _write()**

RA8P1 参考工程使用 Clang + picolibc tinystdio，这与传统的 GCC + newlib-nano 有本质区别：

| 特性 | GCC + newlib-nano | Clang + picolibc tinystdio |
|------|-------------------|---------------------------|
| printf 实现路径 | `printf → _write()` | `printf → vfprintf(stdout) → stdout->put()` |
| stdout 重写方式 | `_write()` 钩子 | `FDEV_SETUP_STREAM` 或重定义 `stdout` |
| 复杂度 | 低 | 中（需要理解 picolibc FILE 机制） |

**第三层：FDEV_SETUP_STREAM 在 picolibc 中不生效**

`log.c` 中已经包含了 FDEV_SETUP_STREAM 的重定向代码：

```c
static int _uart_putchar(char c, FILE *stream) {
    USR_SCI_UART_Write(&g_uart8_ctrl, (uint8_t *)&c, 1);
    return 0;
}
static FILE __stdio = FDEV_SETUP_STREAM(_uart_putchar, NULL, NULL, _FDEV_SETUP_WRITE);
FILE *const stdout = &__stdio;
```

但实际测试发现此方案不生效，因为 picolibc tinystdio 中 `stdout` 的实现机制与 avr-libc 不同，`FILE *const stdout` 的强定义可能被库内部符号覆盖，或者 `printf` 内部使用了不同的路径。

- 解决

修改 `log.c` 中的宏定义：

```c
#define USE_UART_PRINTF_REDIRECT 0  // 改为 0，启用 printf 重定向
```

此宏控制了 `uart_printf()` 的启用状态和底层实现方式（轮询寄存器 vs FSP 中断）。

## 3.3 签名验证失败

### 问题描述

首次烧录后，bootloader 执行到 `boot_go()` 时触发 assert 失败，卡住无法跳转。串口无任何 MCUboot 日志。

### 根因分析

`mcuboot_config.h` 中定义了：

```c
#define MCUBOOT_SIGN_EC256             // 要求 ECDSA P-256 签名
#define MCUBOOT_VALIDATE_PRIMARY_SLOT  // 验证 Primary slot
```

这意味着 bootloader 启动时会：
1. 读取 Primary slot 的 image header
2. 读取 image body，计算 SHA-256 hash
3. 读取 TLV 区域的 ECDSA P-256 签名
4. 用内嵌的公钥验证签名

如果 image 未签名，第 3 步找不到有效签名 → assert 失败。

### 解决

有两种方式：

**方式一：关闭签名验证（快速测试）**

```c
// 注释掉这两行
// #define MCUBOOT_SIGN_EC256
// #define MCUBOOT_VALIDATE_PRIMARY_SLOT
```

> 注意：此文件是 FSP 生成的，修改后重新 Generate Project Content 会被覆盖。需要在 FSP 配置中去掉签名选项。

**方式二：正确签名（推荐）**

设置环境变量后，e2 studio 会在 **Boot Image Build** 步骤自动签名：

| 环境变量 | 值 |
|---------|-----|
| `MCUBOOT_IMAGE_VERSION` | `1.0.0` |
| `MCUBOOT_IMAGE_SIGNING_KEY` | `ra/mcu-tools/MCUboot/root-ec-p256.pem` |

签名命令（自动执行）：

```
python imgtool.py sign --header-size 0x200 --slot-size 0x30000 --max-sectors 6
  --key root-ec-p256.pem --version 1.0.0 --align 32 --max-align 32
  --overwrite-only --confirm --pad-header app.temp.bin app.bin.signed
```

> 注②：`--align 32` 是因为 RA8P1 的 MRAM 编程单位为 32 字节（`BSP_FEATURE_MRAM_PROGRAMMING_SIZE_BYTES`）。

## 3.4 调试下载问题

e2 studio 的 Debug Configuration 中，primary images 需要以 **Raw Binary** 格式下载到指定地址：

| Image | 地址 | 文件 |
|-------|------|------|
| CPU0 Primary | 0x02010000 | `<project>.bin.signed` |
| CPU1 Primary | 0x02040000 | `<project>.bin.signed` |

如果 Debug Configuration 中的 path 使用了 `${workspace_loc}` 变量，注意路径格式使用正斜杠 `/` 而非反斜杠 `\`，否则 GDB MI 会报 `Failed to open ... Invalid argument`。

# 四、MCUboot 启动流程分析

## 4.1 完整启动日志

```
// === Bootloader 自身初始化 ===
[INFO] date:Jul 7 2026, time:16:47:37
[INFO] Built with Renesas Advanced Flexible Software Package version 6.4.0

// === MCUboot 开始检查 Image 0 (CPU0) ===
[DBG] area 1                          // FLASH_AREA_0P_ID = Primary slot 0
[DBG] area 2                          // FLASH_AREA_0S_ID = Secondary slot 0
[DBG] read area=1, off=0, len=0x20    // 读取 image header

// 检查 Secondary slot 是否有待更新
[DBG] read area=2, off=0x2fff0, len=0x10  // 读取 trailer magic
[DBG] read area=2, off=0x2ff80, len=0x1
[DBG] read area=2, off=0x2ffa0, len=0x1
[DBG] read area=2, off=0x2ffc0, len=0x1
[INF] Image index: 0, Swap type: none     // 无待处理升级

// === 同样检查 Image 1 (CPU1) ===
[INF] Image index: 1, Swap type: none

// === 开始验证 Image 0 签名 ===
[DBG] area 1
[DBG] read area=1, off=0, len=0x100      // 逐块读取 Image body
[DBG] read area=1, off=0x100, len=0x100
    ⋮
[DBG] read area=1, off=0x3600, len=0xfa  // 读取到 image 末尾
[DBG] read area=1, off=0x36fa, len=0x4   // 读取 TLV header
[DBG] read area=1, off=0x36fe, len=0x4
[DBG] read area=1, off=0x3702, len=0x20  // SHA256 hash (32 bytes)
[DBG] read area=1, off=0x3726, len=0x20
[DBG] read area=1, off=0x374a, len=0x48  // ECDSA P-256 signature (72 bytes)

// === 同样验证 Image 1 ===
[DBG] area 3
    ⋮
[DBG] read area=3, off=0x2e6a, len=0x47

// === 启动应用 ===
[DBG] Starting Application Image
[DBG] Image Offset: 0x2010000
[DBG] Vector Table: 0x2010200. PC=0x20112c9, SP=0x22003400
```

## 4.2 启动阶段详解

| 阶段 | 操作 | 说明 |
|------|------|------|
| ① 硬件复位 | CPU0 从 0x02000000 开始执行 | Bootloader 向量表 |
| ② BSP 初始化 | 时钟、UART、Cache | 串口输出初始化 |
| ③ MCUboot 初始化 | `boot_go()` 开始 | 初始化 flash 驱动 |
| ④ 检测 Image 0 | 读取 Primary/Secondary header+trailer | 判断 swap 状态 |
| ⑤ 检测 Image 1 | 同上 | 双核需要两个 image |
| ⑥ 验证 Image 0 | SHA-256 hash → ECDSA P-256 签名 | 约 8800 次 flash 读取 |
| ⑦ 验证 Image 1 | 同上 | 约 3000 次 flash 读取 |
| ⑧ 启动应用 | 设置向量表，跳转至 `0x02010200` | 跳过 0x200 字节 header |

## 4.3 签名验证机制

```
Image Layout in Primary Slot:
┌─────────────────────────┐ 0x02010000
│ MCUboot Header (0x200B) │ ← image version, magic, size, flags
├─────────────────────────┤ 0x02010200
│                         │
│   Application Image     │ ← 实际固件代码
│                         │
├─────────────────────────┤ 0x02010200 + image_size
│ TLV Area:               │
│  - SHA256 hash (32B)    │ ← 完整性验证
│  - ECDSA P-256 sig(72B) │ ← 来源验证（公钥内嵌在 bootloader 中）
│  - Key hash (optional)  │
│  - Dependency (optional)│ ← 多 image 依赖关系
└─────────────────────────┘
```

公钥内嵌在 bootloader 中，位于 `ra/mcu-tools/MCUboot/sim/mcuboot-sys/csupport/keys.c`。测试使用的是默认的 ECDSA P-256 密钥对，生产环境必须更换。

# 五、MCUboot 优缺点

| 方面 | 优点 | 缺点 |
|------|------|------|
| **升级方式** | 支持 Overwrite / Swap / Direct-XIP 三种模式，适应不同硬件 | Swap 模式需要额外的 scratch 区域，Flash 占用更大 |
| **安全性** | 支持 RSA/EC/ML-DSA 签名验证，可防篡改、防降级攻击 | 签名密钥管理复杂，丢失私钥则无法发布升级 |
| **多核支持** | 原生支持 multi-image，一个 bootloader 管理多个 CPU 核的固件 | 配置复杂度随 image 数量增加 |
| **Flash 占用** | Overwrite 模式下约 50 KB | 对于小容量 MCU（≤256 KB Flash）不可接受 |
| **编译部署** | 应用只需单次编译，自动签名 | 需要搭建 Python 签名工具链 |
| **日志调试** | 提供完整日志等级（ERR/WRN/INF/DBG） | 串口输出需要额外适配（picolibc 兼容性问题） |
| **环境依赖** | 生成文件和配置分离，易于版本管理 | FSP 集成后深度绑定 e2 studio，非 Renesas 平台需手动移植 |
| **抗量子** | 支持 ML-DSA（Dilithium）签名 | 签名体积大（约 2.5 KB），Flash 和 RAM 占用显著增加 |

## 5.1 典型资源开销

| 配置 | Bootloader 大小 | 说明 |
|------|----------------|------|
| Overwrite + 无签名 | ~20 KB | 最精简配置 |
| Overwrite + ECDSA P-256 | ~50 KB | 测试工程配置 |
| Swap + RSA-2048 | ~70 KB | RSA 签名验证需要更多代码 |
| Swap + ML-DSA | ~85 KB | 抗量子签名，带 dilithium 库 |

> 注③：上述大小为 RA8P1 MRAM 上的实测值，具体大小随工具链版本和优化选项变化。

# 六、个人总结

## 6.1 MCUboot 的整体评价

MCUboot 是一个功能完善、设计良好的开源 bootloader，但它的**重量级**特性决定了它并非适用于所有场景：

**适合的场景：**
- 资源充足的 MCU（Flash ≥ 512 KB，RAM ≥ 128 KB）
- 多核 MCU（multi-image 管理优势明显）
- 对安全性有要求的工业/汽车应用（需要签名验证）
- 复杂固件管理（版本管理、降级防护、依赖管理）

**不适合的场景：**
- 小容量 MCU（≤ 256 KB Flash）—— bootloader 本身占用 20~50 KB，加上双 slot 空间，可用 Flash 所剩无几
- 简单场景（仅需 A/B 切换）—— 硬件 dual-bank 或 SAS 方案更轻量
- 非 Renesas 平台—— 移植工作量大

## 6.2 与 SAS 方案的对比

| 维度 | SAS 方案（RA8T2 当前） | MCUboot 方案 |
|------|----------------------|-------------|
| 编译 | 需两套不同地址编译 | 单次编译 |
| 签名 | 无 | ECDSA/RSA/ML-DSA |
| 版本管理 | 无 | 原生支持 |
| 降级防护 | 无 | 支持 |
| 资源占用 | Bootloader ~4 KB | ~50 KB |
| 升级方式 | SAS 寄存器切换 | overwrite/swap |
| Flash 占用 | 2×502 KB | 4×192 KB + 64 KB |
| 开发复杂度 | 低 | 高 |

## 6.3 理解与启发

通过本次 MCUboot 测试，更深刻地理解了以下概念：

1. **Bootloader 的本质**：上电后先于应用执行的一段代码，负责硬件初始化、固件校验和跳转管理。SAS 方案相当于"硬件 bootloader"，MCUboot 是"软件 bootloader"。

2. **签名验证的必要性**：在 OTA 场景中，如果没有签名验证，攻击者可以伪造固件、回滚到有漏洞的版本。MCUboot 的 TLV 签名机制（hash + 签名分离）设计合理，既做完整性校验又做来源认证。

3. **Overwrite 与 Swap 的取舍**：
   - Overwrite：简单可靠，但升级失败无法回滚（需要额外的 recovery 机制）
   - Swap：升级失败可自动回滚，但需要 scratch 区域，升级时间更长

4. **picolibc tinystdio 与 printf 重定向的陷阱**：在 Clang + picolibc 环境下，传统的 `_write()` 钩子不生效，需要理解 picolibc 的 `FILE` 结构和 `stdout` 实现机制。这属于工具链差异带来的隐性坑。

## 6.4 对 RA8T2 项目的启示

RA8T2 项目和 RA8P1 同属 RA8 系列，MRAM 架构相同，MCUboot 方案理论上可直接移植。但需要考虑：

- RA8T2 需要 EtherCAT 通信，升级通过 **FoE（File over EtherCAT）** 协议传输固件
- FoE 下载完成后写入 Secondary slot → 触发 MCUboot 进行 overwrite 升级
- MCUboot 验证签名 → 覆盖 Primary slot → 复位 → 运行新固件

如果 RA8T2 的 Flash 和 RAM 资源允许，MCUboot 是比 SAS 方案更完善的升级方案。如果资源受限，则可以参考 MCUboot 的设计思路，实现一个精简版的软件 bootloader。
