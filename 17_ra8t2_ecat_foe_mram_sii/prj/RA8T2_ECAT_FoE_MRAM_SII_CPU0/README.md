# 十七、RA8T2 EtherCAT MRAM SII EEPROM Emulation

## 1. 项目说明

本工程基于 Renesas RA8T2 MCK、ETG5003 EtherCAT ESC 和 Beckhoff SSC
协议栈，原始功能是 EtherCAT FoE 例程，SII EEPROM 使用外部 EEPROM 芯片。

本次修改增加了一个可切换的后端：

- 原外部 EEPROM：保持原有 SSC 访问路径。
- MRAM 模拟 EEPROM：使用 RA8T2 Code MRAM 保存 2 KB SII 镜像。

当前默认配置为 **MRAM 模拟 EEPROM**，并通过 ECAT Bench 完成扫描、
进入 OP、复位后重新扫描进入 OP 的硬件验证。

### 1.1 工程命名和仓库位置

| 项目 | 名称 |
|---|---|
| RA8 仓库编号 | `17_ra8t2_ecat_foe_mram_sii` |
| e2Studio 工程 | `RA8T2_ECAT_FoE_MRAM_SII_CPU0` |
| 原始 Renesas 例程 | `RA8T2_MCK_esc_ETG5003_CPU0` |
| BANK0 发布固件 | `ECATFW_RA8T2_B00.efw` |
| BANK1 发布固件 | `ECATFW_RA8T2_B01.efw` |

工程目录和 e2Studio 工程名已经统一改为 MRAM SII 名称。FoE 协议使用的
`ECATFW_RA8T2_B00`、`ECATFW_RA8T2_B01` 是设备升级文件名，有意保持原样，
不能随工程重命名。

## 2. 当前状态

### 2.1 默认模式

配置文件：

```text
src/ethercat/renesas/ecat_eeprom_config.h
```

默认值：

```c
#define ECAT_EEPROM_USE_MRAM_EMULATION      (1)
```

含义：

```text
1 - 使用 RA8T2 Code MRAM 模拟 2 KB SII EEPROM
0 - 使用原外部 EEPROM
```

也支持通过编译参数覆盖：

```text
-DECAT_EEPROM_USE_MRAM_EMULATION=0
-DECAT_EEPROM_USE_MRAM_EMULATION=1
```

### 2.2 当前硬件验证结果

验证工具：

```text
ECAT Bench v0.2.0
J-Link V8.58
```

MRAM 模式验证结果：

```text
从站在线数量:        3/3
EtherCAT 主站状态:   OP
从站状态:            Group state -> OP
周期调度:            Cyclic scheduling enabled for OP
SII 起始地址:        0x02004800
SII Magic:           0x0E80
Vendor ID:           0x00001234
Product Code:        0x00002003
Revision:            0x00000200
```

此外执行了以下复测：

1. 烧录 MRAM 模式固件。
2. 读取 `0x02004800`，确认 SII 内容正确。
3. Bench 扫描从站并进入 OP。
4. 使用 J-Link 复位目标板。
5. 重新扫描，从站再次进入 OP，确认 MRAM 中的 SII 在复位后仍有效。

加入 FoE 同步 SII 写适配后，最新 MRAM 固件再次通过 J-Link 烧录，下载、
擦除、编程和 Verify 均成功。复位后直接读取 `0x02004800` 的结果为：

```text
02004800 = EE420E80 00000010 00000000 00020000
02004810 = 00001234 00002003 00000200 00000000
```

其中 SII Magic 为 `0x0E80`，Vendor ID 为 `0x00001234`，Product Code 为
`0x00002003`，Revision 为 `0x00000200`。

原外部 EEPROM 模式也做了对照验证：

1. 将宏改为 `0`。
2. 重新构建并烧录。
3. Bench 扫描到 `3/3` 从站并进入 OP。
4. 切回宏 `1`，重新构建、烧录并确认 MRAM 模式再次进入 OP。

当前板子最终运行的是 **宏 1 的 MRAM 模拟 EEPROM 固件**。

### 2.3 构建结果

当前 MRAM 模式 BANK0 构建结果：

```text
text = 57588
data = 0
bss  = 57364
```

同一版本切换为外部 EEPROM 模式（宏 `0`）后也已完成构建：

```text
text = 53868
data = 0
bss  = 57292
```

构建产物：

```text
BANK0/RA8T2_ECAT_FoE_MRAM_SII_CPU0.elf
BANK0/RA8T2_ECAT_FoE_MRAM_SII_CPU0.srec
BANK0/ECATFW_RA8T2_B00.efw
```

仓库导入时又在 `RA8T2_ECAT_FoE_MRAM_SII_CPU0` 新工程名下执行了完整清理构建，
上述 MRAM 模式尺寸保持一致，说明工程重命名没有改变功能配置。

工程中仍有 SSC 原始代码带来的部分编译 warning，但两种模式均能成功构建、
链接和烧录。

## 3. 原工程与修改后的区别

| 项目 | 原工程 | 当前修改 |
|---|---|---|
| SII 存储 | 外部 EEPROM 芯片 | 可选外部 EEPROM 或 Code MRAM |
| SII 大小 | 2 KB | 2 KB |
| SSC EEPROM 命令 | ESC 直接访问外部 EEPROM | MRAM 模式通过应用回调访问 |
| 持久化介质 | 外部 EEPROM | Code MRAM `0x02004800-0x02004FFF` |
| 写入方式 | 外部 EEPROM 驱动 | 2 KB SRAM shadow + 32 字节 MRAM 块写入 |
| 写后校验 | 外部 EEPROM 流程 | 每块写后立即回读校验 |
| 默认 SII | 外部 EEPROM 内容 | SSC 默认 SII + 运行时同步版本信息 |
| 切换方式 | 无 | `ECAT_EEPROM_USE_MRAM_EMULATION` |
| 原外部 EEPROM 路径 | 可用 | 宏 `0` 时保持可用 |

MRAM 模式下，SEMI 参数备份区由原来的 4 KB 拆分为：

```text
2 KB SEMI 参数备份
2 KB SII 模拟 EEPROM
```

BANK0 应用起始地址仍为 `0x02005000`，没有因为新增 SII 区域而整体后移。

## 4. 核心原理

数据流如下：

```text
EtherCAT Master
    |
    v
ETG5003 ESC
    |
    v
SSC EEPROM 命令处理
    |
    v
pAPPL_EEPROM_Read / Write / Reload / Store
    |
    v
mram_sii.c
    |
    +--> 2 KB SII SRAM shadow
    |
    +--> 32 字节 dirty block 跟踪
    |
    +--> R_MRAM_Write()
    |
    v
RA8T2 Code MRAM 0x02004800
```

实现要点：

- ESC 的 SII 接口按 word 访问，MRAM 按 32 字节块编程。
- RAM 中维护完整的 2 KB SII 镜像 `aSiiShadow[]`。
- EEPROM 写命令先修改 shadow，并标记对应的 32 字节块为 dirty。
- 最后一次写命令后等待 1000 ms，再执行 `pAPPL_EEPROM_Store`。
- 只写 dirty block，减少 MRAM 编程次数。
- 每个块写完后回读校验。
- MRAM 编程期间保存并恢复 `PRIMASK`，写入过程中关闭中断。
- 上电时从 MRAM 读取 SII，校验失败则加载 `eeprom.h` 默认镜像并写入。
- 运行时同步 `VENDOR_ID`、`PRODUCT_CODE`、`REVISION_NUMBER` 和 SII CRC。

### 4.1 FoE 升级与 SII Revision 更新

FoE 下载和写入固件的主要流程在 `bootmode.c` 的 `BL_Data()` 中。最后一段
固件数据接收完成后，原例程会读取新固件 Identify 区中的 Revision，然后通过
下面的 SSC 接口更新 SII：

```c
ESC_EepromAccess(SII_EEP_IDENTIFY_OFFSET + SII_EEP_REVESIONNO,
                 2,
                 &Buffer.word[SII_EEP_REVESIONNO],
                 ESC_WR);
```

MRAM 模式下完整的更新链路为：

```text
FoE BL_Data()
    -> ESC_EepromAccess(..., ESC_WR)
    -> EEPROM_CommandHandler()
    -> pAPPL_EEPROM_Write()
    -> MRAM_SII_WriteCallback()
    -> 更新 aSiiShadow[] 并标记 dirty block
    -> pAPPL_EEPROM_Store()
    -> MRAM_SII_StoreCallback()
    -> MRAM_SII_StoreNow()
    -> 写回 0x02004800 处的 MRAM SII
```

`bootmode.c` 中的 FoE 下载、固件写入和 Bank 切换逻辑不需要直接操作 MRAM，
也不需要增加 MRAM 专用分支。后端适配集中在 `ecatappl.c` 的 EEPROM 公共层：

- 普通主站 SII 写命令仍由 `ECAT_Main()` 检测 `EEPROM_CMD_PENDING` 后处理，
  并在最后一次写入约 1000 ms 后统一 Store。
- `ESC_EepromAccess()` 是 PDI 侧同步调用，FoE 会用它更新 Revision。MRAM 模式
  下必须在 busy 等待期间主动执行 `EEPROM_CommandHandler()`，否则 PDI 和 ESC
  会互相等待。
- FoE 更新 Revision 后马上会切换 Bank 并复位，因此该同步写路径在成功后立即
  调用 `pAPPL_EEPROM_Store()`，不能依赖 1000 ms 延迟 Store。

这些处理全部位于 `ESC_EEPROM_EMULATION == 1` 的条件编译范围内。宏为 `0`
时仍使用原来的外部 EEPROM 路径，FoE 代码和原有行为不变。

移植时必须一并保留上述同步 PDI 服务和写后立即 Store 的处理。只实现
`pAPPL_EEPROM_Read/Write/Store` 回调，通常足以支持主站普通 SII 访问，但不足
以保证 FoE 更新 Revision 后立即复位时的持久化。

## 5. MRAM 地址布局

| 区域 | 起始地址 | 结束地址 | 大小 |
|---|---:|---:|---:|
| SUA0 启动区 | `0x02000000` | `0x02001FFF` | 8 KB |
| SUA1 启动区 | `0x02002000` | `0x02003FFF` | 8 KB |
| SEMI 参数备份 | `0x02004000` | `0x020047FF` | 2 KB |
| 模拟 SII EEPROM | `0x02004800` | `0x02004FFF` | 2 KB |
| BANK0 应用 | `0x02005000` | 见链接脚本 | - |

核心地址定义：

```c
#define FW_UP_SEMI_PRM_START_ADDR  (0x02004000)
#define FW_UP_SII_PRM_START_ADDR   (0x02004800)
#define FW_UP_BANK0_ADDR           (0x02005000)
```

## 6. 核心修改文件

### 6.1 新增文件

| 文件 | 说明 |
|---|---|
| `src/ethercat/renesas/ecat_eeprom_config.h` | EEPROM 后端切换宏 |
| `src/ethercat/renesas/mram_sii.c` | RA8T2 MRAM SII 模拟实现 |
| `src/ethercat/renesas/mram_sii.h` | MRAM SII 对外接口 |

### 6.2 修改文件

| 文件 | 修改内容 |
|---|---|
| `src/ethercat/beckhoff/Src/ecat_def.h` | `ESC_EEPROM_EMULATION` 映射到项目切换宏 |
| `src/ethercat/beckhoff/Src/applInterface.h` | 增加 EEPROM 回调函数指针 |
| `src/ethercat/beckhoff/Src/ecatappl.c` | EEPROM 命令处理、FoE 同步写服务、读取宽度、延迟/立即 Store |
| `src/ethercat/beckhoff/Src/ecatappl.h` | 增加 EEPROM 命令处理声明 |
| `src/ethercat/beckhoff/Src/ecatslv.c` | 处理 `EEPROM_CMD_PENDING` 和 Revision 校验 |
| `src/hal_entry.c` | 打开 MRAM、初始化 SII、注册回调 |
| `src/r_fw_up_ra/r_fw_up_ra_if.h` | 新增 SII 区域并调整保留区大小 |
| `src/ethercat/renesas/samplesemi.h` | 模拟模式下 SEMI 备份区改为 2 KB |
| `src/ethercat/renesas/eeprom.h` | SSC 默认 SII 数据集成到当前工程 |
| `BANK0/src/ethercat/renesas/subdir.mk` | e2Studio 生成构建清单时加入 `mram_sii.c`，该目录不提交 |

### 6.3 SSC 生成代码和手写代码的边界

`src/ethercat/beckhoff/Src` 的基线代码来自 SSC/ET9300。
本次为了接入应用侧 EEPROM 模拟，修改了少量 SSC 文件。

`src/ethercat/renesas` 是 Renesas 适配目录，其中：

- `sampleappl.c/h`、`samplesemi.c/h` 等来自 SSC 示例和 Renesas 适配。
- `eeprom.h` 的 2048 字节 SII 内容来自 SSC 根据 ESI XML 生成的默认内容。
- `mram_sii.c/h` 和 `ecat_eeprom_config.h` 是本次新写的工程代码。

不要直接用 SSC 新输出覆盖整个 `src/ethercat`，否则会破坏 FoE、双 Bank、
MRAM 和板级适配。需要按文件进行合并。

## 7. SSC 参考工程和 XML

当前项目内的 SSC 参考资料：

```text
ssc/Renesas EtherCAT RA8 ETG5003.esp
ssc/Renesas EtherCAT RA8 ETG5003.xml
ssc/SscTool.log
```

说明：

- `.esp`：SSC/ET9300 工程文件，可用 SSC Tool 打开。
- `.xml`：ESI 设备描述文件，可导入 TwinCAT 或 ECAT Bench 参考。
- `SscTool.log`：SSC 生成日志，可用于确认生成配置。

生成 MRAM 模拟所需的 SSC 配置：

```text
ESC_EEPROM_EMULATION=1
ESC_EEPROM_ACCESS_SUPPORT=1
ESC_EEPROM_SIZE=0x800
EEPROM_WRITE_SIZE=0x2
CREATE_EEPROM_CONTENT=1
```

注意：

- `ESC_EEPROM_EMULATION=1` 只是主开关。
- 仍需要 `ESC_EEPROM_ACCESS_SUPPORT=1`，否则 SSC 不会保留 EEPROM 命令访问代码。
- `CREATE_EEPROM_CONTENT=1` 才会根据 ESI XML 生成 `eeprom.h` 默认 SII 内容。
- 如果 ESI XML 改变，应重新生成 `eeprom.h`，再按当前工程的方式合并。

## 8. 构建方法

### 8.1 e2Studio

1. 打开工程。
2. 选择 `BANK0` 构建配置。
3. 确认 `ecat_eeprom_config.h` 中的宏值。
4. Build Project。
5. 烧录 `BANK0/RA8T2_ECAT_FoE_MRAM_SII_CPU0.elf`。

### 8.2 命令行

确保 ARM GNU Toolchain 的 `arm-none-eabi-gcc` 在 PATH 中，然后执行：

```powershell
make -C BANK0 -j8 all
```

当前工程的硬件验证目标是 BANK0。当前 `BANK1` 目录没有完整构建清单；
如果以后启用 BANK1，必须把 `src/ethercat/renesas/mram_sii.c` 同步加入
BANK1 的 `subdir.mk`，否则 BANK1 在宏 `1` 下会缺少 MRAM SII 符号。

仓库只保存 FSP 配置和工程源码。`BANK0/`、`BANK1/`、`ra/`、`ra_cfg/`、
`ra_gen/` 都是 e2Studio/FSP 生成目录，首次打开工程或重新生成代码后会出现在
本地工作区，不提交到 Git。

## 9. 烧录和测试

### 9.1 烧录

使用 e2Studio 或 J-Link 下载：

```text
BANK0/RA8T2_ECAT_FoE_MRAM_SII_CPU0.elf
```

烧录完成后复位目标板。

### 9.2 最小验证流程

1. 确认固件使用宏 `1` 构建。
2. 烧录并复位。
3. 使用调试器读取 `0x02004800` 开始的 SII。
4. 确认 Magic、Vendor、Product、Revision。
5. 打开 ECAT Bench 或 TwinCAT3。
6. 启动主站并扫描从站。
7. 确认从站在线。
8. 请求 PREOP、SAFEOP、OP。
9. 复位目标板后重新扫描，确认仍能进入 OP。

FoE 升级及 SII Revision 持久化验证：

1. 准备一个带不同 `REVISION_NUMBER` 的 FoE 固件，并确保目标 Bank 也包含
   MRAM SII 适配代码。
2. 从 OP 或适用的升级状态进入 BOOT。
3. 使用 ECAT Bench、TwinCAT3 或其他 FoE 工具下载固件。
4. 完成 BOOT 到 INIT 的切换，使例程执行 Bank 切换和复位。
5. 读取 SII Revision：字地址为 `0x0C`，绝对地址为 `0x02004818`。
6. 确认该值等于新固件的 `REVISION_NUMBER`。
7. 再次扫描从站并确认能够进入 OP。

当前已经完成的硬件验证是 MRAM SII 扫描、进入 OP 以及复位后再次进入 OP。
新增的 FoE 同步写和立即 Store 路径已完成代码接入和构建验证；实际 FoE 下载
后的 Revision 更新仍需按上述流程做一次端到端确认。

### 9.3 原外部 EEPROM 对照验证

1. 将 `ECAT_EEPROM_USE_MRAM_EMULATION` 改为 `0`。
2. 重新构建 BANK0。
3. 烧录并复位。
4. 扫描从站并进入 OP。
5. 验证完成后改回 `1`，重新构建和烧录。

两种模式不能只改宏而不重新构建和烧录。

## 10. 移植到其他工程

### 10.1 移植前提

目标工程需要具备：

- EtherCAT SSC 协议栈。
- 可写的非易失存储区域。
- 至少 2 KB 连续空间用于 SII。
- 目标存储的编程粒度和擦除特性已知。
- 能在启动时执行存储初始化。

如果目标不是 RA8T2，需要把 `mram_sii.c` 中的 RA8T2 FSP 调用替换为目标平台
的 NVM/MRAM/Flash 驱动，其他 SII 逻辑可以保留。

### 10.2 推荐移植步骤

1. 预留 2 KB SII 区域。

   区域不能与启动区、参数备份区、应用 Bank 或链接脚本重叠。建议按目标
   NVM 的编程粒度对齐。

2. 新增后端切换宏。

   可参考 `ecat_eeprom_config.h`：

   ```c
   #define ECAT_EEPROM_USE_MRAM_EMULATION  (1)
   ```

3. 复制 SII 模拟文件。

   ```text
   mram_sii.c
   mram_sii.h
   eeprom.h
   ```

   `eeprom.h` 必须和目标设备的 ESI 一致，数组大小必须为 2048 字节。

4. 映射 SSC 开关。

   在 `ecat_def.h` 中确认：

   ```c
   #define ESC_EEPROM_EMULATION  ECAT_EEPROM_USE_MRAM_EMULATION
   ```

   同时保持：

   ```c
   #define ESC_EEPROM_ACCESS_SUPPORT  1
   #define ESC_EEPROM_SIZE            0x800
   #define EEPROM_WRITE_SIZE          0x2
   ```

5. 增加 EEPROM 回调。

   在 `applInterface.h` 中增加：

   ```c
   pAPPL_EEPROM_Read
   pAPPL_EEPROM_Write
   pAPPL_EEPROM_Reload
   pAPPL_EEPROM_Store
   ```

6. 接入 SSC EEPROM 命令处理。

   参考当前工程的：

   ```text
   ecatappl.c  -> EEPROM_CommandHandler()
   ecatslv.c   -> EEPROM_CMD_PENDING 处理
   ```

7. 接入初始化顺序。

   推荐顺序：

   ```text
   RelocateVectorTableToRAM()
   R_MRAM_Open()
   MRAM_SII_Init()
   EtherCAT/ESC Port Open()
   MainInit()
   MRAM_SII_RegisterCallbacks()
   MainLoop()
   ```

   `MainInit()` 会清除 SSC 的 EEPROM 回调指针，所以注册必须放在
   `MainInit()` 之后。

8. 更新构建清单。

   把 `mram_sii.c` 加入实际使用的 Bank 构建。当前工程已加入 BANK0；
   如果启用 BANK1，也必须同步加入 BANK1。

9. 适配存储驱动。

   需要确认：

   - 写入地址是否按目标 NVM 粒度对齐。
   - 是否需要先擦除。
   - 源地址和目标地址类型是否与 FSP API 一致。
   - 写操作能否在中断关闭期间完成。
   - 写后回读是否可靠。

10. 做硬件验证。

    至少验证：

    - 首次上电无有效镜像时能生成默认 SII。
    - 复位后能读取 MRAM SII。
    - Bench/TwinCAT 能扫描到从站。
    - 从站能进入 PREOP、SAFEOP、OP。
    - 修改 SII 后 Store 能持久化。
    - FoE 更新 SII Revision 后，Bank 切换复位不会丢失该值。
    - 原外部 EEPROM 模式仍能通过宏 `0` 工作。

### 10.3 不要直接整体覆盖 SSC 输出

推荐只合并以下关键接口：

```text
ecat_def.h
applInterface.h
ecatappl.c
ecatappl.h
ecatslv.c
eeprom.h
```

再把 `mram_sii.c/h` 作为目标平台适配层加入工程。

## 11. 已知限制

- 当前使用单份 2 KB SII 镜像，没有双副本事务切换。
- MRAM 写入期间关闭中断，极端掉电窗口内不能保证 SII 原子更新。
- Store 有 1000 ms 延迟，这是为了等待 ESC 完成一次 SII 写序列。
- FoE 使用的 PDI 同步 SII 写路径不受 1000 ms 延迟影响，会在
  `ESC_EepromAccess()` 返回前立即 Store。
- `eeprom.h` 是静态默认镜像；ESI 更新后需要重新生成并合并。
- 当前硬件验证针对 BANK0，BANK1 使用前必须补充构建清单和验证。

## 12. 快速切换速查

MRAM 模拟：

```c
#define ECAT_EEPROM_USE_MRAM_EMULATION  (1)
```

原外部 EEPROM：

```c
#define ECAT_EEPROM_USE_MRAM_EMULATION  (0)
```

修改后必须重新构建、重新烧录。
