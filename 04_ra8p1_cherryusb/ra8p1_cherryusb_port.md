四、RA8P1移植CherryUSB尝鲜
===
[toc]

# 一、背景

- CherryUSB 在 commit [faf9360](https://github.com/cherry-embedded/CherryUSB/commit/faf9360) 正式支持 Renesas RA 系列
- 硬件：RT-Thread RA8P1 Titan Board（Cortex-M85，480MHz）
- 软件：e2studio + GCC（arm-none-eabi-gcc）
- 之前做过 RZN2L 移植 CherryUSB 和瑞萨 USB IP 性能对比，这次继续相关测试

# 二、心理曲线变化

整个移植过程的心情波动如下：

| 阶段 | 心理状态 |
| :--- | :--- |
| CherryUSB 已支持 RA | "cherryusb已支持了，直接拿来用就行" → 轻松 |
| 发现 hal_renesas | "原来 cherryusb使用的hal_renesas 是 forked from zephyrproject-rtos/hal_renesas，官方在 Zephyr 验证过的" |
| 回想 RZN2L 移植 | "当时完全不知道 hal_renesas，不是做了很多无用功么？" |
| 查看 hal_renesas 支持情况 | "支持 RA，但 RZN 的 USB 暂未支持" → "还好，没有白做，主要怪自己没发现 hal_renesas" |
| 高速性能对比 RZN2L | "RA8P1 高速性能低于 RZN2L..." → 些许失落 |
| 初步分析原因 | "可能与 16bit/32bit 有关，暂未优化" → "似乎又有一点安慰，没有白做" |



# 三、CherryUSB 文件选择与编译

## 3.1 需要编译的源文件

先用笨办法，将整个 CherryUSB 文件夹加入 e2studio 工程（方便后续直接替换），然后排除不需要的文件。实际需要编译的只有：

| 分类 | 文件 | 说明 |
| :--- | :--- | :--- |
| **Core** | `core/usbd_core.c` | USB 设备核心栈 |
| **Class** | `class/cdc/usbd_cdc_acm.c` | CDC ACM 类驱动 |
| **Port** | `port/renesas/device/usb_dc_rensas.c` | RA USB DCD 驱动 |
| **Port** | `port/renesas/device/r_usb_device.c` | Renesas FSP USB 设备 HAL |

对应的包含路径需要添加：

- `CherryUSB`
- `CherryUSB/common`
- `CherryUSB/core`
- `CherryUSB/port/renesas`
- `CherryUSB/port/renesas/device`

## 3.2 排除方法

e2studio 的多选排除编译功能不太好用（不支持 Shift/Ctrl 多选后批量 Exclude from build）。推荐逐个文件右键 → **Resource Configuration → Exclude from build**。或者直接在 `.cproject` 中手动编辑排除规则。

| 排除类别 | 说明 |
| :--- | :--- |
| 全部排除再逐个添加 | 整个 CherryUSB 文件夹 Exclude，再对上述 4 个 .c 文件取消 Exclude |
| 保留 demo | `demo/` 目录是模板参考，不需要编译 |
| osal | 当前只做 Device 模式，osal 文件不需要（CherryUSB 设备栈不依赖 OSAL） |

## 3.3 usb_config.h 关键配置

```c
#define CONFIG_USB_HS                          /* 高速模式 */
#define CONFIG_USBDEV_ADVANCE_DESC             /* 使用高级描述符回调 */
#define CONFIG_USBDEV_REQUEST_BUFFER_LEN 512
#define CONFIG_USBDEV_MAX_BUS 2
#define CONFIG_USBHOST_MAX_BUS 0               /* 仅设备模式 */
```

通过注释/取消注释 `CONFIG_USB_HS` 切换 Full-Speed / High-Speed。

# 四、测试代码

## 4.1 设备端

- `src/examples/cdc_acm_descriptor.c` — USB 描述符（VID 0x0483, PID 0x5720）
- `src/examples/cdc_acm_example.c` — CDC ACM 端点处理与初始化
- `src/hal_entry.c` — 主入口，通过 `R_IOPORT_PinWrite(IO_PORT_04_PIN_13)` 切换 FS/HS

FS/HS 切换逻辑：

```c
#ifndef CONFIG_USB_HS
    R_IOPORT_PinWrite(&IOPORT_CFG_CTRL, BSP_IO_PORT_04_PIN_13, 0);  // FS
    cdc_acm_init(0, R_USB_FS0_BASE);
#else
    R_IOPORT_PinWrite(&IOPORT_CFG_CTRL, BSP_IO_PORT_04_PIN_13, 1);  // HS
    cdc_acm_init(1, R_USB_HS0_BASE);
#endif
```

## 4.2 上位机测试脚本

`test_cdc_speed.py` 增加了命令行参数，不带参数使用默认 COM，带参数使用指定 COM：

```python
test_comx = sys.argv[1] if len(sys.argv) > 1 else 'COM14'
```

用法：

```
python test_cdc_speed.py           # 默认 COM14
python test_cdc_speed.py COM10     # 指定 COM10
```

# 五、Titan Board 特殊说明

## 5.1 调试接口

| 特性 | 说明 |
| :--- | :--- |
| 调试器 | DAPLink（CMSIS-DAP），无法直接在 e2studio 调试和下载 |
| 替代方案 | 移植 RT-Thread Studio 的 pyocd，编写 `pyocd_flash.bat` ，运行bat脚本即下载固件 |
| 进阶 | 也可将 DAPLink 烧录为 JLinkOB，使用 JLink 调试 |

pyocd 烧录命令：

```
pyocd flash --target=R7KA8P1KF --erase=auto --frequency=1000000 ra8p1_cherryusb.elf
```

## 5.2 UART 调试口

Titan Board 的调试串口连接的是 **UART8**，与官方开发板不同。

## 5.3 USB FS/HS 引脚切换

RA8P1 的 USB 全速和高速使用不同的引脚，官方板使用 2 个 USB 端口（Type-C 各一）。Titan Board 通过 **GPIO 4_13** 控制模拟开关切换：

| 模式 | GPIO 4_13 | USB 基址 |
| :--- | :--- | :--- |
| Full-Speed | 0 | `R_USB_FS0_BASE` |
| High-Speed | 1 | `R_USB_HS0_BASE` |

软件中通过宏 `CONFIG_USB_HS` 配合切换。

# 六、性能测试结果

## 6.1 Full-Speed

```
test cdc out speed
cdc out speed 6.073954 MB/s
test cdc in speed
cdc in speed 6.312565 MB/s
```

## 6.2 High-Speed

```
test cdc out speed
cdc out speed 6.073870 MB/s
test cdc in speed
cdc in speed 6.256247 MB/s
```

## 6.3 结果分析

| 模式 | OUT (Host→Device) | IN (Device→Host) |
| :--- | :--- | :--- |
| Full-Speed | 6.07 MB/s | 6.31 MB/s |
| High-Speed | 6.07 MB/s | 6.26 MB/s |

FS 和 HS 测速结果几乎一致，说明当前高速模式未发挥应有带宽。

# 七、对比 RZN2L 与初步分析

## 7.1 与 RZN2L CherryUSB 对比

| 项目 | RA8P1 | RZN2L |
| :--- | :--- | :--- |
| 内核 | Cortex-M85 (480MHz) | Cortex-R52 (400MHz) |
| USB HS 性能 | ~6 MB/s | 更高（具体数值见 RZN2L 报告） |
| USB IP | Renesas RA USB | Renesas RZN USB |

RA8P1 高速性能低于 RZN2L CherryUSB，初步调查可能与 **USB IP 数据路径位宽**有关：

- RZN2L 可能使用 32bit 数据路径
- RA8P1 可能使用 16bit 数据路径

暂未做进一步的优化测试。

## 7.2 结论

仅代表当前提交 [faf9360](https://github.com/cherry-embedded/CherryUSB/commit/faf9360) 和当前测试条件下的结果。RA8P1 CherryUSB 高速性能低于 RZN2L 虽然令人意外，但也说明 RZN2L 的移植工作并非没有价值。
