<div align="center">

# GS_Smart_car

**AURIX TC264D 四轮舵机镜头车固件**

基于 Infineon AURIX TC264D + 逐飞 SEEKFREE SDK，采用事件驱动协作式调度与单向依赖分层架构。

![Platform](https://img.shields.io/badge/Platform-AURIX%20TC264D-blue?style=flat-square)
![Language](https://img.shields.io/badge/Language-C99-orange?style=flat-square)
![IDE](https://img.shields.io/badge/IDE-AURIX%20Development%20Studio-green?style=flat-square)
![Architecture](https://img.shields.io/badge/Architecture-App%20Service%20BSP%20PAL%20Impl-teal?style=flat-square)
![Branch](https://img.shields.io/badge/Branch-tc264--four--wheel--servo--camera--car-black?style=flat-square)

</div>

---

## 系统架构

工程边界按大厂嵌入式项目的可维护性要求拆分：**App 只做应用编排，BSP/Driver 只封装板级设备，Platform 只定义 PAL 契约，Impl 负责 TC264/逐飞实现，Vendor SDK 默认只读**。运行态状态逐步收敛到明确 owner 的 `context/handler`。

```mermaid
%%{init: {'theme':'base','themeVariables':{'fontFamily':'Segoe UI,Arial,sans-serif','fontSize':'14px','primaryTextColor':'#202124','lineColor':'#5f6368','clusterBkg':'#ffffff','clusterBorder':'#c9d1d9'}}}%%
flowchart TB
    classDef entry fill:#f1f3f4,stroke:#5f6368,stroke-width:1.5px,color:#202124
    classDef app fill:#e8f0fe,stroke:#1a73e8,stroke-width:2px,color:#174ea6
    classDef service fill:#f3e8fd,stroke:#9334e6,stroke-width:2px,color:#681da8
    classDef system fill:#fef7e0,stroke:#f9ab00,stroke-width:2px,color:#7a4b00
    classDef bsp fill:#e6f4ea,stroke:#34a853,stroke-width:2px,color:#137333
    classDef platform fill:#e0f7fa,stroke:#00838f,stroke-width:2px,color:#006064
    classDef impl fill:#fce8e6,stroke:#d93025,stroke-width:2px,color:#a50e0e
    classDef vendor fill:#f8f9fa,stroke:#80868b,stroke-width:1.5px,color:#3c4043

    subgraph APP["code/app · 应用层"]
        APP_MAIN["SmartcarApp<br/>生命周期 / 任务注册 / 主循环"]:::app
    end

    subgraph SERVICE["code/service · 服务与算法层"]
        SENSOR["SensorService<br/>gyro / encoder context"]:::service
        VISION["Vision<br/>图像处理 / 控制快照"]:::service
        CONTROL["Control<br/>PID handler / actuator cmd"]:::service
        DEBUG["DebugDisplayService<br/>调试显示编排"]:::service
    end

    subgraph BSP["code/bsp · 板级设备驱动"]
        MOTOR["Motor"]:::bsp
        SERVO["Servo"]:::bsp
        DISPLAY["Display"]:::bsp
        INPUT["Input"]:::bsp
        BUZZER["Buzzer"]:::bsp
    end

    subgraph PLATFORM["code/platform · PAL 契约"]
        PAL["platform.h<br/>pal_* stable API"]:::platform
    end

    subgraph IMPL["code/impl/tc264 · TC264 实现层"]
        PAL_IMPL["platform_tc264.c<br/>PAL -> SEEKFREE/iLLD"]:::impl
        ISR_ADAPTER["isr_adapter.c/h<br/>ack / bounded sample / ISR facts"]:::impl
    end

    subgraph VENDOR["libraries · Vendor SDK 只读"]
        SEEKFREE["SEEKFREE<br/>zf_common / zf_driver / zf_device"]:::vendor
        ILLD["Infineon iLLD"]:::vendor
    end

    APP_MAIN --> SENSOR
    APP_MAIN --> VISION
    APP_MAIN --> CONTROL
    APP_MAIN --> DEBUG
    CONTROL --> MOTOR
    CONTROL --> SERVO
    DEBUG --> DISPLAY
    APP_MAIN --> INPUT
    APP_MAIN --> BUZZER
    MOTOR --> PAL
    SERVO --> PAL
    DISPLAY --> PAL
    INPUT --> PAL
    BUZZER --> PAL
    SENSOR --> PAL
    VISION --> PAL
    PAL -. implemented by .-> PAL_IMPL
    ISR_ADAPTER --> PAL
    PAL_IMPL --> SEEKFREE
    PAL_IMPL --> ILLD
    ISR_ADAPTER --> SEEKFREE
    ISR_ADAPTER --> ILLD
```

**依赖铁律：** App/Service/BSP 只能向下依赖 PAL 或本层稳定接口，不能 include `zf_common_headfile.h`、`Ifx*` 或 Vendor 类型。`code/platform` 只放 `platform.h`，不放 TC264 实现。`libraries/` 是 Vendor SDK，默认只读。

## 中断与调度

中断路径不是 App 职责。`user/isr.c` 是 TC264 SDK entry，只保留 `IFX_INTERRUPT` 入口和 source id；`code/system/irq` 是系统中断桥；`code/impl/tc264` 处理 TC264 硬件事实。

```mermaid
flowchart LR
    classDef sdk fill:#f1f3f4,stroke:#5f6368,color:#202124
    classDef system fill:#fff4ce,stroke:#f9ab00,color:#7a4b00
    classDef impl fill:#fce8e6,stroke:#d93025,color:#a50e0e
    classDef sched fill:#e8f0fe,stroke:#1a73e8,color:#174ea6
    classDef service fill:#e6f4ea,stroke:#34a853,color:#137333

    HW["TC264 IRQ source<br/>PIT / DMA / ERU / UART"]:::sdk
    SDK["user/isr.c<br/>IFX_INTERRUPT thin entry"]:::sdk
    ROUTER["code/system/irq<br/>SmartcarIrqRouter"]:::system
    ADAPTER["code/impl/tc264<br/>IsrAdapter returns facts"]:::impl
    EVENTS["scheduler/event.c<br/>flag + counter policy"]:::sched
    SCHED["scheduler.c<br/>cooperative dispatch"]:::sched
    TASKS["Service tasks<br/>Vision / Sensor / Control"]:::service

    HW --> SDK
    SDK --> ROUTER
    ROUTER --> ADAPTER
    ADAPTER --> ROUTER
    ROUTER -->|EVT_CAM_FRAME / EVT_GYRO_10MS / EVT_ENCODER_50MS| EVENTS
    EVENTS --> SCHED
    SCHED --> TASKS
```

关键约束：

- `SmartcarIrqRouter` 位于 `code/system/irq/smartcar_irq_router.c/h`，负责 source 校验、表驱动分发、事件映射和 tick 发布。
- `IsrAdapter` 位于 `code/impl/tc264/isr_adapter.c/h`，只做清标志、有界整数采样、Vendor ISR callback，返回 `ISR_ADAPTER_EVT_*`。
- DMA 摄像头帧完成由 adapter 返回 `ISR_ADAPTER_EVT_CAMERA_FRAME`，router 统一发布 `EVT_CAM_FRAME`；App 不再轮询后伪造 ISR 事件。
- `EVT_GYRO_10MS` 使用计数语义，避免主循环阻塞时吞掉 10ms tick。

## 目录结构

```text
GS_Smart_car/
├── code/
│   ├── app/                       # 应用层：只做生命周期、任务注册、主循环编排
│   │   └── smartcar_app.c/h
│   ├── service/                   # 服务/算法层：vision、sensor、control、diagnostics
│   ├── bsp/                       # BSP/Driver：motor、servo、display、input、buzzer
│   ├── platform/                  # PAL 契约：仅 platform.h
│   ├── impl/tc264/                # TC264 Impl：platform_tc264.c、isr_adapter.c/h
│   ├── system/irq/                # 系统中断桥：smartcar_irq_router.c/h
│   ├── scheduler/                 # event + cooperative scheduler
│   ├── config/                    # config.h 集中参数
│   └── common/                    # init/utils/legacy data，避免新增运行态全局
├── user/                          # TC264 SDK entry：cpu0/cpu1/isr/isr_config
├── libraries/                     # Vendor SDK：Infineon iLLD + SEEKFREE，默认只读
├── tests/                         # 主机端测试与 stubs
├── .cproject / .project           # ADS/Eclipse 工程配置
└── Lcf_Tasking_Tricore_Tc.lsl     # TASKING 链接脚本
```

## 快速开始

### ADS 编译

```bash
# AURIX Development Studio
# File -> Open Projects -> 选择本目录 -> Build Project
```

### 主机端测试

```bash
gcc -Itests/stubs -Icode/common tests/test_my_abs.c code/common/utils.c -o test_my_abs.exe
./test_my_abs.exe

gcc -Itests/stubs -Icode/platform -Icode/scheduler tests/test_event.c code/scheduler/event.c -o test_event.exe
./test_event.exe
```

### 主机端语法检查

```bash
gcc -std=c99 -Werror=implicit-function-declaration -fsyntax-only \
  -Icode -Icode/app -Icode/platform -Icode/config -Icode/common -Icode/bsp \
  -Icode/service/control -Icode/service/vision -Icode/service/sensor \
  -Icode/service/diagnostics -Icode/scheduler -Icode/system/irq -Icode/impl/tc264 \
  code/app/smartcar_app.c code/system/irq/smartcar_irq_router.c code/common/init.c \
  code/service/control/control.c code/service/control/pid.c code/service/vision/vision.c \
  code/service/sensor/sensor.c code/service/diagnostics/debug_display.c \
  code/scheduler/event.c code/scheduler/scheduler.c code/common/data.c code/common/utils.c \
  code/bsp/motor.c code/bsp/servo.c code/bsp/input.c code/bsp/buzzer.c code/bsp/display.c
```

`code/impl/tc264/*.c` 和 `user/isr.c` 依赖 TC264/逐飞宏，主要通过 ADS 或实车环境验证。

## 模块一览

| 层级 | 模块 | 文件 | 职责 |
|:---:|:---:|:---|:---|
| App | 主循环 | `code/app/smartcar_app.c` | 初始化、任务注册、调度器驱动 |
| System/IRQ | 中断路由 | `code/system/irq/smartcar_irq_router.c` | source -> adapter fact -> scheduler event/tick |
| Service | 视觉 | `code/service/vision/vision.c` | 图像处理、边线/中线、控制快照 |
| Service | 传感器 | `code/service/sensor/sensor.c` | 陀螺仪积分、编码器速度 context |
| Service | 控制 | `code/service/control/control.c` | PID handler、执行器指令 |
| BSP | 板级设备 | `code/bsp/*.c` | 电机、舵机、显示、输入、蜂鸣器 |
| Platform API | PAL | `code/platform/platform.h` | 平台无关硬件接口 |
| TC264 Impl | PAL/ISR 实现 | `code/impl/tc264/*.c` | PAL 转 Vendor SDK、ISR 硬件事实 |
| SDK Entry | TC264 入口 | `user/isr.c` | `IFX_INTERRUPT` 薄入口 |
| Vendor SDK | 原厂库 | `libraries/` | Infineon iLLD + SEEKFREE |

## 开发规则

新增功能先判断 owner：

1. 应用流程放 `code/app`，不能包含硬件宏。
2. 算法和状态放 `code/service/<module>`，用 context/handler 管理运行态。
3. 板级设备能力放 `code/bsp`，只依赖 `platform.h`。
4. 新硬件抽象先扩展 `code/platform/platform.h`，再在 `code/impl/tc264` 实现。
5. 中断源新增时同步 `user/isr.c` source、`SmartcarIrqRouter` 路由表、`IsrAdapter` fact。
6. Vendor SDK 只读；确需修改时单独提交并说明原因。

提交格式：

```text
type(scope): 中文描述

feat | fix | refactor | docs | chore | test
scope: app | service | bsp | platform | impl | system | scheduler | docs
```

## 演进路线

- [x] App 与 System/IRQ 解耦，`SmartcarIrqRouter` 移出 `code/app`
- [x] Platform API 与 TC264 Impl 解耦，`platform_tc264.c` / `isr_adapter.c` 移入 `code/impl/tc264`
- [x] 摄像头 DMA 帧事件归入 adapter fact -> system router -> scheduler event 链路
- [x] `Driver + Handler + Context` 第一阶段：Sensor/ISR/Control/Vision 状态 owner 收敛
- [x] README / AGENTS / `.cproject` 同步新架构边界
- [ ] 拆分 `isr_adapter.c` 为 camera/encoder/uart 子 adapter
- [ ] 为 `event` 增加 per-event policy：flag / counter / saturating counter
- [ ] CPU1 视觉处理 offload
- [ ] ADS + 实车完整验证

## 许可证

底层 Vendor SDK 遵循逐飞 TC264 开源库及 Infineon iLLD 的原许可。自研代码位于 `code/`，请保留原版权声明与第三方库许可文件。
