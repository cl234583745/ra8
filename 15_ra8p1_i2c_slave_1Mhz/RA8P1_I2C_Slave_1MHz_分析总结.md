十五、RA8P1 I2C Slave 1MHz高速传输问题分析与解决
===
[toc]
# 1. 背景

| 角色 | 硬件 | 配置 |
|---|---|---|
| 从机 | EK-RA8P1 | IIC1，7bit 地址 0x4A，FASTPLUS(1MHz)，`R_IIC_SLAVE` + DTC |
| 主机 | STM32U575 | HAL I2C1，地址 0x94（0x4A<<1），Fast Mode Plus(1MHz) |

问题现象：
1. 1MHz 下首字节错误（应返 00，实测 C0；400kHz 正常）
2. 实测 1MHz 达不到设定速率

---

# 2. slave问题分析
- FSP TXI 回调链过长?

master 读 → 从机硬件进入发送模式 → ICDRT 空产生 TXI → ISR 须在下一位时序内写入 ICDRT。FSP 默认路径首次 TXI 要穿 5~7 层嵌套：

![](./images/i2c%20slave%20read.png)

| 层数 | 默认 `iic_slave_txi_isr` | 自定义 `usr_iic_slave_txi_isr` |
|---|---|---|
| 1 | R_ICU 向量表 | R_ICU 向量表 |
| 2 | `iic_txi_slave()` 状态机 | 直接判断 direction |
| 3 | `iic_slave_notify()` 分发 | （首次）仅一次 `R_IIC_SLAVE_Write` |
| 4 | 用户回调 `g_i2c_slave0CB()` | — |
| 5 | `R_IIC_SLAVE_Write()` | — |
| 6 | `iic_slave_initiate_transaction()` | 后续字节纯写 ICDRT |
| 7 | 写 ICDRT | — |

1MHz 每位 1µs，这串调用把首字节写入拖过时序，硬件只能发 ICDRT 残留值（C0 之类）。

# 3. 解决办法1：简化中断处理
## 3.1 slave自定义中断函数
- e2 studio `Interrupts → New User Event` 把 IIC1 TXI 绑定到自定义函数 `usr_iic_slave_txi_isr`（生成的 `vector_data.c` [1] 不会被重新生成顶替），RXI/TEI/ERI 保留 FSP 默认。
- 自定义中断函数优点是不会被新生成代码代替
## 3.2 slave代码

```c
#define BUF_LEN            5
static uint8_t g_slave_rx_buf[BUF_LEN] = {0xaa,0xbb,0xcc,0xdd,0xee};
static uint8_t g_slave_tx_buf[BUF_LEN] = {0x00,0x11,0x22,0x33,0x44};
void g_i2c_slave0CB(i2c_slave_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        switch (p_args->event)
        {
            case I2C_SLAVE_EVENT_RX_REQUEST:
            {
                R_IIC_SLAVE_Read(&g_i2c_slave_ctrl, g_slave_rx_buf, BUF_LEN);
                break;
            }
            //已经自定义usr_iic_slave_txi_isr，不再走该case
            case I2C_SLAVE_EVENT_TX_REQUEST:
            {
                R_IIC_SLAVE_Write(&g_i2c_slave_ctrl, g_slave_tx_buf, BUF_LEN);
                break;
            }
            default:
                break;
        }
    }
 }

void usr_iic_slave_txi_isr (void)
{
    /* Save context if RTOS is used */
    FSP_CONTEXT_SAVE

    #define ICSR2_START_BIT       (0x04U)
    #define ICSR2_STOP_BIT        (0x08U)
    #define IIC_STR_EN_BIT        (0x04U)
    #define IIC_STP_EN_BIT        (0x08U)
    #define IIC_TMO_EN_BIT        (0x01U)
    #define IIC_ALD_EN_BIT        (0x02U)
    #define IIC_NAK_EN_BIT        (0x10U)
    #define IIC_RXI_EN_BIT        (0x20U)
    #define IIC_TXI_EN_BIT        (0x80U)

    R_BSP_IrqStatusClear(R_FSP_CurrentIrqGet());

    iic_slave_instance_ctrl_t * p_ctrl = &g_i2c_slave_ctrl;

    /* TXI 只服务于 master 读。两种情况：
     * 1) 本事务首个 TXI（方向未登记为 MASTER_READ）
     * 2) 上一笔写收尾竞态留下的残留状态（do_dummy_read 卡死、RX DTC 可能误布署） */
    if (IIC_SLAVE_TRANSFER_DIR_MASTER_READ_SLAVE_WRITE != p_ctrl->direction)
    {
        /* 自愈：掐掉残留 RX DTC，清竞态标记，重新登记读方向+数据，重配中断 */
        if (NULL != p_ctrl->p_cfg->p_transfer_rx)
        {
            p_ctrl->p_cfg->p_transfer_rx->p_api->disable(p_ctrl->p_cfg->p_transfer_rx->p_ctrl);
        }
        p_ctrl->activation_on_rxi = false;
        p_ctrl->do_dummy_read     = false;
        p_ctrl->notify_request    = true;

        (void) R_IIC_SLAVE_Write(&g_i2c_slave_ctrl, g_slave_tx_buf, BUF_LEN);

        p_ctrl->p_reg->ICSR2 &= (uint8_t) ~(ICSR2_STOP_BIT | ICSR2_START_BIT);
        p_ctrl->p_reg->ICIER  = (uint8_t) (IIC_STR_EN_BIT | IIC_STP_EN_BIT | IIC_TMO_EN_BIT |
                                           IIC_ALD_EN_BIT | IIC_NAK_EN_BIT | IIC_RXI_EN_BIT | IIC_TXI_EN_BIT);
        p_ctrl->start_interrupt_enabled = true;
    }

    if (IIC_SLAVE_TRANSFER_DIR_MASTER_READ_SLAVE_WRITE == p_ctrl->direction)
    {
        if (p_ctrl->total == p_ctrl->loaded)         /* 末字节已发，等 master NACK */
        {
            R_BSP_IrqStatusClear(p_ctrl->p_cfg->tei_irq);
            NVIC_ClearPendingIRQ(p_ctrl->p_cfg->tei_irq);
            p_ctrl->p_reg->ICIER_b.TEIE = 1U;
        }
        else                                         /* 写数据，释放 SCL */
        {
            p_ctrl->p_reg->ICDRT = p_ctrl->p_buff[p_ctrl->loaded];
            p_ctrl->loaded++;
            p_ctrl->transaction_count++;

#if IIC_SLAVE_CFG_DTC_ENABLE                            /* 保留 DTC 才需要这段 */
            if ((NULL != p_ctrl->p_cfg->p_transfer_tx) && (p_ctrl->total > 2U) &&
                (false == p_ctrl->activation_on_txi) && (p_ctrl->loaded == 2U))
            {
                uint8_t volatile const * p_icdrt = &(p_ctrl->p_reg->ICDRT);
                p_ctrl->p_cfg->p_transfer_tx->p_api->reset(p_ctrl->p_cfg->p_transfer_tx->p_ctrl,
                                                           (void *) (p_ctrl->p_buff + 2U),
                                                           (uint8_t *) p_icdrt,
                                                           (uint16_t) (p_ctrl->total - 2U));
                p_ctrl->loaded             = p_ctrl->total;
                p_ctrl->transaction_count += (p_ctrl->total - 2U);
                p_ctrl->activation_on_txi  = true;
            }
#endif
        }
    }

    /* Restore context if RTOS is used */
    FSP_CONTEXT_RESTORE
}

#undef ICSR2_START_BIT
#undef ICSR2_STOP_BIT
#undef IIC_STR_EN_BIT
#undef IIC_STP_EN_BIT
#undef IIC_TMO_EN_BIT
#undef IIC_ALD_EN_BIT
#undef IIC_NAK_EN_BIT
#undef IIC_RXI_EN_BIT
#undef IIC_TXI_EN_BIT
```

- 要点：
  - **TXI 是硬件触发**（master 读 + ICDRT 空），`R_IIC_SLAVE_Write()` 只是"登记数据"，不产生 TXI；master 读 N 字节 → TXI 约 N+1 次。DTC 开启时第 3 字节起由 DTC 自动搬运，CPU 每笔 5 字节读只进 ISR 3 次。
  - 两个 `if` 不写 `if/else`：首个 TXI 要在同一中断里完成"登记方向 + 补首字节"，避免少拍破坏时序。
  - 必须保留 `FSP_CONTEXT_SAVE/RESTORE`（裸机展开为空）与 `#undef` 收尾；参数检查已关（`bsp_cfg.h` `BSP_CFG_PARAM_CHECKING_ENABLE=0`）。

---




## 3.3  master CubeMX配置和分析

![](./images/i2c%20master%20timing.png)

| 配置项 | 值 | 说明 |
|---|---|---|
| I2C Speed Mode | **Fast Mode Plus** | FM+ 最高支持 1MHz |
| I2C Speed Frequency (KHz) | **1000** | 目标频率 |
| Rise Time (ns) / Fall Time (ns) | **0** | 预留边沿时间，0=最快 |
| Coefficient of Digital Filter | **0** | DNF，0=最快 |
| Analog Filter | **Enabled** | 抗干扰（每沿约 50ns） |
| **Timing**（灰色只读） | **0x00701F6B** | CubeMX 算出的 `I2C_TIMINGR` 值 |

   - 生成代码里只出现这一个值：`hi2c1.Init.Timing = 0x00701F6B;`，它就是写进 `I2C1->TIMINGR` 的 32bit 寄存器。改上面任一项，Timing 自动重算。


## 3.4 master测试代码

```c
uint8_t receivedData[5];
uint8_t sendData[5] = {0xaa,0xbb,0xcc,0xdd,0xee};

while (1)
{
    memset(receivedData, 0, sizeof(receivedData));
    if (HAL_I2C_Master_Receive(&hi2c1, 0x94, receivedData, 5, 10) != HAL_OK)  /* 0x94=0x4A<<1 */
        printf("i2c read error\n");
    else
        for (int i=0;i<5;i++) printf("%02x ", receivedData[i]);  /* 期望 00 11 22 33 44 */

    if (HAL_I2C_Master_Transmit(&hi2c1, 0x94, sendData, 5, 10) != HAL_OK)
        printf("i2c write error\n");

    HAL_Delay(1000);
}
```

# 4. 解决办法2：slave开启i2c streching

- i2c slave收到read命令后，硬件自动拉低scl引脚，等待返回数据。
- i2c master也需要开启i2c streching，部分可能不支持(例如RA)。

# 5. 1MHz实测达不到：软硬件双因素

## 5.1 硬件决定论

I2C 开漏，高电平靠上拉充电。AN4235：`tr = Rp × Cb × 0.8473`。软件填 0 只是"不预留时间"，物理 tr 不变。

| 模式 | tr 上限 |
|---|---|
| 100kHz | 1000ns |
| 400kHz | 300ns |
| 1MHz (FM+) | **120ns** |

- `R_pu ≤ tr / (0.8473 × C_bus)`（3.3V、FM+）：

| C_bus | R_pu 建议 |
|---|---|
| 50pF | 2.2kΩ |
| 100pF | 1.2kΩ |
| 200pF | 680Ω |
| 300pF | 470Ω |
| 400pF | 330Ω |

- `R_pu ≥ (V_DD - V_OL)/I_OL`：FM+ 3.3V ≈ ≥145Ω。
- 电容来源：走线 ~0.5~1pF/cm、连接器数 pF、每从机 5~20pF。**线越长电容越大 → 上拉要更小**。FM+ 常用 470Ω~1kΩ。

## 5.2 软件侧

- STM32：TIMINGR 依 PCLK1 计算；DNF/模拟滤波叠加延迟；FM+ 必须使能（见 4.2）。
- RA8P1 从机：`hal_data.c` 的 `clock_settings`（`cks_value/brl_value/digital_filter_stages`），无 TIMINGR，FSP 按 PCLK 自动算。
- 排查法：示波器看 SCL——
  - 上升沿是**斜的 RC 充电曲线** → 硬件（上拉/电容/线长），软件无解；
  - 波形很方但周期仍 >1µs → PCLK1/寄存器粒度问题（提高 PCLK1）。
- 结论：**1MHz 就是 FM+ 天花板**（标准、U5、RA8P1 三层都封顶），可靠到 1MHz 靠上拉/线长/PCLK1，超频无意义。

---

# 6. 结论

1. **首字节错（1MHz）** → FSP TXI 回调链 5~7 层太慢，自定义 `usr_iic_slave_txi_isr` 直写寄存器解决。
2. i2c slave可以打开时钟拉伸功能，解决应答实时性不够的问题
3. **1MHz 达不到** → 上拉/电容/线长为主因，软件只是预留预算；CubeMX 全 0 最快、填实测值最稳。

