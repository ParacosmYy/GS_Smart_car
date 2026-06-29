# GS_Smart_car

**智能车固件（当前目标：AURIX TC264D）**

本工程面向四轮舵机镜头车，当前目标平台为 Infineon AURIX TC264D，底层使用逐飞 SEEKFREE SDK 与 Infineon iLLD。工程采用事件驱动协作调度与链接期端口替换：业务层不包含 Vendor 头文件，不依赖具体 MCU，也不引入运行期 ops 注册或平台 dispatch 回环。

![Platform](https://img.shields.io/badge/Platform-AURIX%20TC264D-blue?style=flat-square)
![Language](https://img.shields.io/badge/Language-C99-orange?style=flat-square)
![IDE](https://img.shields.io/badge/IDE-AURIX%20Development%20Studio-green?style=flat-square)
![Architecture](https://img.shields.io/badge/Architecture-Link--Time%20Ports-teal?style=flat-square)

## Architecture

```mermaid
flowchart LR
    subgraph ENTRY["入口层 SDK Entry"]
        CPU["cpu0_main.c"]
        ISR["user/isr.c"]
    end

    subgraph SYSTEM["系统层 System"]
        BOOT["smartcar_system.c<br/>启动编排"]
        BOARD["smartcar_board.c<br/>板级初始化"]
        SCHED["scheduler.c / event.c<br/>协作调度"]
    end

    subgraph APP["应用层 App"]
        TASKS["smartcar_app.c<br/>任务表"]
    end

    subgraph SERVICE["服务层 Service"]
        VISION["vision"]
        SENSOR["sensor"]
        CONTROL["control"]
        DIAG["diagnostics"]
    end

    subgraph BSP["板级动作 BSP"]
        ACT["motor / servo"]
        IO["display / input / buzzer"]
    end

    subgraph PLATFORM["平台契约 Platform"]
        PORT["port_if.h<br/>SystemPort / McuIo / Device"]
    end

    subgraph TARGET["目标端口 Target"]
        TPORT["target_port.c"]
        TMAP["target_board_map.c"]
        TIRQ["target_irq.c"]
    end

    subgraph VENDOR["厂家底座 Vendor"]
        SDK["SEEKFREE SDK + iLLD"]
    end

    CPU --> BOOT --> BOARD --> PORT
    BOOT --> TASKS --> SCHED --> VISION
    SCHED --> SENSOR
    SCHED --> CONTROL
    SCHED --> DIAG
    VISION --> IO
    SENSOR --> PORT
    CONTROL --> ACT
    DIAG --> IO
    ACT --> PORT
    IO --> PORT
    PORT -. "link-time symbols" .-> TPORT --> SDK
    TPORT --> TMAP
    ISR --> TIRQ --> SCHED
    TIRQ --> SDK

    classDef entry fill:#f8fafc,stroke:#64748b,color:#0f172a;
    classDef system fill:#eff6ff,stroke:#2563eb,color:#1e3a8a;
    classDef app fill:#ecfdf5,stroke:#059669,color:#064e3b;
    classDef service fill:#fff7ed,stroke:#ea580c,color:#7c2d12;
    classDef bsp fill:#f0fdfa,stroke:#0d9488,color:#134e4a;
    classDef platform fill:#f5f3ff,stroke:#7c3aed,color:#4c1d95;
    classDef target fill:#fef2f2,stroke:#dc2626,color:#7f1d1d;
    classDef vendor fill:#f9fafb,stroke:#6b7280,color:#111827;

    class CPU,ISR entry
    class BOOT,BOARD,SCHED system
    class TASKS app
    class VISION,SENSOR,CONTROL,DIAG service
    class ACT,IO bsp
    class PORT platform
    class TPORT,TMAP,TIRQ target
    class SDK vendor
```

## Layer Rules

| Layer | Responsibility | Boundary |
|:--|:--|:--|
| App | 注册应用任务，描述产品运行节奏 | 不直接访问 BSP/Vendor |
| Service | 视觉、传感、控制、诊断策略 | 不包含 MCU/Vendor 头 |
| BSP | 电机、舵机、显示、蜂鸣器、输入动作封装 | 只调用 `port_if.h` |
| Platform | 稳定端口契约：`SystemPort_*`、`McuIo_*`、`Device_*` | 不出现芯片专有类型 |
| Target | 当前 MCU 端口实现、资源映射、中断适配 | 唯一可调用 Vendor 的业务侧适配层 |
| Vendor | SEEKFREE SDK 与 Infineon iLLD | 默认只读 |

## IRQ Flow

```mermaid
flowchart LR
    HW["硬件中断<br/>PIT / DMA / ERU / UART"]
    ENTRY_ISR["user/isr.c<br/>薄入口"]
    TARGET_IRQ["target_irq.c<br/>清标志 / 采样 / Vendor 回调"]
    EVENT["event_post_from_isr"]
    TICK["Scheduler_AddTickFromIsr"]
    LOOP["scheduler_run"]
    TASK["Service task"]

    HW --> ENTRY_ISR --> TARGET_IRQ
    TARGET_IRQ --> EVENT --> LOOP --> TASK
    TARGET_IRQ --> TICK --> LOOP
```

ISR 只做短路径处理：清中断标志、必要采样、调用 Vendor 回调、投递事件或 tick。视觉、控制、显示等耗时逻辑全部回到主循环调度执行。

## Repository Map

```text
code/
  app/                    应用任务表和任务注册
  service/                vision / sensor / control / diagnostics
  bsp/                    motor / servo / display / input / buzzer
  platform/               port_if.h，稳定平台契约
  target/tc264/           当前目标端口、板级映射、中断适配
  system/                 启动编排、板级初始化
  scheduler/              事件标志和协作式调度器
  config/                 产品参数和 SMARTCAR_* 资源 ID
user/                     当前 SDK 入口层
libraries/                Vendor SDK
```

## Build

使用 AURIX Development Studio 构建：

```text
AURIX Development Studio -> Open Projects -> Build Project
```

本仓库不维护 host smoke tests、unit tests 或测试脚本；不要重新引入 `tests/`、`tests/smoke/`、`scripts/check_syntax.ps1` 或同类 host guard。

## MCU Porting

更换 MCU 时只替换目标端口与 Vendor 工程配置，业务层保持不动。

1. 新增 `code/target/<target>/`。
2. 提供 `target_port.c`，实现 `port_if.h` 中的 `SystemPort_*`、`McuIo_*`、`Device_*`。
3. 提供 `target_board_map.c/.h`，完成 `SMARTCAR_*` 到新 MCU 引脚、定时器、串口、DMA 的映射。
4. 提供 `target_irq.c/.h` 与 `target_irq_config.h`，完成目标中断适配并直接投递 scheduler event/tick。
5. 更新 IDE 工程，只编译当前 target 和对应 Vendor SDK，避免多个 target 同时定义端口符号。
6. 保持 `app/`、`service/`、`bsp/`、`scheduler/`、`system/` 不包含 Vendor 头或 target 私有头。

禁止回退到运行期 ops 注册、旧 PAL 兼容层、IRQ fact/router 或平台 dispatch 文件。

## Documentation

非 Vendor 的 `.c` / `.h` 文件保持 Doxygen 文件头与函数注释同步；逻辑注释使用中文，文件头结构和分区标题保持工程统一风格。`libraries/` 下 Vendor 文件保留原始头部，默认不改。

## Commit Format

```text
type(scope): 中文描述
```

常用 scope：`app`、`service`、`bsp`、`platform`、`target`、`system`、`scheduler`、`build`、`docs`。

## License

Vendor SDK 遵循 `libraries/` 下 SEEKFREE 与 Infineon 原始许可证；项目代码保持现有版权和许可证声明。
