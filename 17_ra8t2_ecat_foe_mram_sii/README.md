# 17. RA8T2 EtherCAT FoE MRAM SII EEPROM Emulation

本目录是 RA8 系列仓库的第 17 个示例工程，基于 Renesas RA8T2 MCK、
ETG5003 EtherCAT ESC 和 Beckhoff SSC 协议栈。

工程在保留原外部 EEPROM 路径的同时，新增 RA8T2 Code MRAM 模拟 2 KB SII
EEPROM 的后端。两种模式通过 `ECAT_EEPROM_USE_MRAM_EMULATION` 切换：

```c
#define ECAT_EEPROM_USE_MRAM_EMULATION  (1) /* MRAM SII emulation */
#define ECAT_EEPROM_USE_MRAM_EMULATION  (0) /* original external EEPROM */
```

当前默认值为 `1`。MRAM SII 占用 `0x02004800-0x02004FFF`，BANK0 应用仍从
`0x02005000` 开始。

已验证 MRAM 模式可以通过 ECAT Bench 扫描到从站、进入 OP，并在目标板复位后
重新扫描进入 OP。原外部 EEPROM 模式也完成了对照验证。

## 目录

```text
17_ra8t2_ecat_foe_mram_sii/
|-- README.md
`-- prj/
    `-- RA8T2_ECAT_FoE_MRAM_SII_CPU0/
        |-- README.md
        |-- configuration.xml
        |-- src/
        `-- ssc/
```

`BANK0/`、`BANK1/`、`ra/`、`ra_cfg/`、`ra_gen/` 均由 e2Studio/FSP 根据
`configuration.xml` 生成，不纳入仓库。

完整说明、核心修改、FoE 升级分析、SSC 配置、测试步骤和移植方法见：

[RA8T2_ECAT_FoE_MRAM_SII_CPU0/README.md](./prj/RA8T2_ECAT_FoE_MRAM_SII_CPU0/README.md)

## 关键文件

```text
prj/RA8T2_ECAT_FoE_MRAM_SII_CPU0/src/ethercat/renesas/ecat_eeprom_config.h
prj/RA8T2_ECAT_FoE_MRAM_SII_CPU0/src/ethercat/renesas/mram_sii.c
prj/RA8T2_ECAT_FoE_MRAM_SII_CPU0/src/ethercat/renesas/mram_sii.h
prj/RA8T2_ECAT_FoE_MRAM_SII_CPU0/src/ethercat/beckhoff/Src/ecatappl.c
```

SSC 工程和 ESI XML 位于：

```text
prj/RA8T2_ECAT_FoE_MRAM_SII_CPU0/ssc/
```

FoE 升级继续通过 SSC 的 `ESC_EepromAccess()` 更新 SII Revision。MRAM 后端在
`ecatappl.c` 中同步处理 EEPROM 命令并立即 Store，因此 `bootmode.c` 的 FoE
下载和 Bank 切换主流程不需要增加 MRAM 专用分支。
