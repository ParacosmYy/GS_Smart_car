# GS_Smart_car

**AURIX TC264D 智能车固件**

基于 Infineon AURIX TC264D、逐飞 SEEKFREE SDK 与 iLLD。当前架构采用事件驱动协作调度、链接期端口替换和静态 IRQ route 表；业务层不依赖 TC264 Vendor 头，也不经过运行期 dispatch/ops 回环。

![Platform](https://img.shields.io/badge/Platform-AURIX%20TC264D-blue?style=flat-square)
![Language](https://img.shields.io/badge/Language-C99-orange?style=flat-square)
![IDE](https://img.shields.io/badge/IDE-AURIX%20Development%20Studio-green?style=flat-square)
![Architecture](https://img.shields.io/badge/Architecture-Link--Time%20Ports-teal?style=flat-square)

## Architecture

```mermaid
flowchart LR
    ENTRY["SDK Entry<br/>cpu0_main / user/isr"]
    RUNTIME["System Runtime<br/>smartcar_system"]
    APP["App<br/>task table only"]
    TASKS["Service Tasks<br/>VisionTask / ControlTask / FeedbackTask / DiagnosticsTask"]
    SERVICE["Services<br/>vision / sensor / control / diagnostics"]
    PORT["Port Interfaces<br/>mcu_io_if / device_if / service_port_if"]
    BOARD["Board Contract<br/>smartcar_board / smartcar_contract"]
    BSP["BSP<br/>motor / servo / display / input / buzzer"]
    TC264["TC264 Link-Time Ports<br/>tc264_mcu_io_ops / tc264_device_ops"]
    SDK["Vendor SDK<br/>SEEKFREE / iLLD"]

    ENTRY --> RUNTIME --> APP --> TASKS --> SERVICE --> PORT
    RUNTIME --> BOARD --> BSP --> PORT
    SERVICE --> PORT
    PORT -. "link-time symbols" .-> TC264 --> SDK
```

核心边界：

| Layer | Responsibility |
|:--|:--|
| App | 只注册调度任务，不编排视觉、控制、反馈细节 |
| Service | 持有业务状态与算法流水线 |
| BSP | 板级动作封装，只面向平台接口 |
| Platform Interface | `McuIo_*`、`Device_*`、`service_port_if.h` 契约 |
| Impl/TC264 | 直接实现链接期端口符号，调用 SEEKFREE/iLLD |
| Vendor | 第三方 SDK，默认只读 |

## IRQ Flow

```mermaid
flowchart LR
    HW["PIT / DMA / ERU / UART"]
    ISR["IFX_INTERRUPT<br/>user/isr.c"]
    ROUTER["SmartcarIrq_Dispatch"]
    ROUTES["Target IRQ Routes<br/>tc264_board_bind.c"]
    ADAPTER["TC264 ISR Adapter"]
    EVENT["event.c"]
    SCHED["scheduler.c"]
    TASK["Service Task"]

    HW --> ISR --> ROUTER
    ROUTER --> ROUTES --> ADAPTER --> ROUTER
    ROUTER --> EVENT --> SCHED --> TASK
```

保留的调度事件：

| Source | Event |
|:--|:--|
| CCU60 PIT CH0 | `EVT_ENCODER_50MS` |
| CCU60 PIT CH1 | `EVT_GYRO_10MS` + scheduler tick |
| DMA CH5 | `EVT_CAM_FRAME` |
| ERU / UART | 只做清标志或 Vendor 回调 |

## Repository Map

```text
code/
  app/                    task table and main-loop handoff
  service/                task facade plus vision/sensor/control/diagnostics
  bsp/                    motor, servo, display, input, buzzer
  platform/interface/     mcu_io_if, device_if, service_port_if
  platform/system/        system_port, encoder_sample, irq_fact
  platform/target/        target IRQ route contract
  impl/tc264/             TC264 link-time ports, board map, IRQ adapter
  system/board/           board init and product contract binding
  system/irq/             source -> fact/event/tick router
  system/runtime/         boot composition root
  scheduler/              event flags and cooperative scheduler
  config/                 product parameters
user/                     TC264 SDK entry layer
libraries/                SEEKFREE and Infineon vendor code
scripts/                  host smoke guard
tests/smoke/              minimal pure-logic smoke test
```

## Build And Verify

Firmware build:

```text
AURIX Development Studio -> Open Projects -> Build Project
```

Minimal host guard:

```powershell
powershell -ExecutionPolicy Bypass -File scripts/check_syntax.ps1
```

The smoke guard covers event flags, scheduler dispatch, IRQ routing, vision logic, and app/service task syntax. `tests/`, `scripts/`, `docs/`, and `build/` are excluded from ADS target source paths.

## Porting Rules

To add another MCU target:

1. Add `code/impl/<target>/` with direct `McuIo_*` and `Device_*` implementations.
2. Add a target board map and IRQ adapter.
3. Provide `TargetPlatform_GetIrqRoutes()` for that target.
4. Keep App, Service, BSP public headers free of Vendor, board-resource, and `impl/*` includes.
5. Do not reintroduce runtime ops registration, platform dispatch files, old PAL aliases, or duplicate IRQ fact headers.

## Commit Format

```text
type(scope): 中文描述
```

Common scopes: `app`, `service`, `bsp`, `platform`, `impl`, `system`, `scheduler`, `build`, `docs`.

## License

Vendor SDK follows the original SEEKFREE and Infineon licenses under `libraries/`. Project code under `code/` should preserve existing copyright and license notices.
