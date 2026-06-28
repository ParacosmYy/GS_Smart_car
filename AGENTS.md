# Repository Guidelines

## Project Structure & Module Organization

This is an AURIX TC264 smart car firmware project with a strict ops-based platform architecture.

```
App → Service → Platform Interface (ops contracts) → Platform Dispatch → Board Composition → Target Bind → Impl → Vendor SDK
```

- `code/app/`: application task table and `SmartcarApp_RunOnce()` only. Application task assembly layer, not pure business Service.
- `code/system/runtime/`: boot composition root. Calls `TargetPlatform_RegisterAll()` and `SmartcarSystem_Boot()`.
- `code/system/board/`: product board assembly, device init sequence, and ops registration.
- `code/system/irq/`: generic interrupt routing from source/fact to scheduler event/tick.
- `code/service/`: algorithm services (vision, sensor, control, diagnostics).
- `code/bsp/`: board-level drivers for motor, servo, display, input, buzzer.
- `code/platform/interface/`: **MCU IO & Device ops contracts** (`mcu_io_if.h`, `device_if.h`, `actuator_if.h`, `feedback_if.h`, `track_display_if.h`).
- `code/platform/board/`: board ops registry type (`board_ops_if.h` → `target_board_ops_t`).
- `code/platform/target/`: neutral target registration header (`target_platform.h` → `TargetPlatform_RegisterAll()`).
- `code/platform/system/`: dispatch files + IRQ contract + encoder sampling + neutral system port.
- `code/impl/tc264/`: TC264 ops implementations + board bind + board map + IRQ port + ISR adapter.
- `code/config/`: product tuning parameters (`config.h`).
- `user/`: TC264 SDK entry layer with `IFX_INTERRUPT` functions; keep it thin.
- `libraries/`: Infineon iLLD and SEEKFREE Vendor SDK; treat as read-only.
- `tests/`: host-side GCC tests and stubs.

## Build, Test, and Development Commands

```powershell
powershell -ExecutionPolicy Bypass -File scripts/run_host_tests.ps1
```

The script builds and runs all host tests, writes executables to `build/host-tests/`, always includes `-Icode`, and keeps each test's source list explicit so `event.c` is not linked into tests that provide their own event stubs.

## Coding Style & Naming Conventions

C99, Allman braces, no dynamic allocation. `s_` file-static, `g_` true globals, `p_` pointers, `*_t` typedefs, `Module_Action()` functions.

**API naming (production code):**
- `McuIo_GpioInit()` / `McuIo_PwmSetDuty()` / `McuIo_UartInit()` / `McuIo_EncoderGet()` / `McuIo_PitInit()` — MCU IO
- `Device_CameraInit()` / `Device_DisplayPoint()` / `Device_ImuRead()` / `Device_WirelessInit()` — Device drivers
- `Actuator_SetServo()` / `Feedback_NotifyElement()` / `TrackDisplay_DrawLines()` — Domain contracts

**Resource naming:** `SMARTCAR_PWM_SERVO` / `SMARTCAR_GPIO_BUZZER` / `SMARTCAR_ENCODER_LEFT` — in `system/board/smartcar_board_resources.h`

## Architecture Guardrails

**Dependency direction:** `SDK Entry → System Runtime → App → Service → Platform Interface → Board → Impl → Vendor SDK`

**Production code must NOT:**
- Service files include old `pal_*.h` headers (use `platform/interface/*`)
- Platform interface headers include `board_resources.h`, `zf_common_headfile.h`, or `impl/*`
- `impl/tc264` files include `App` or `Service` headers
- `smartcar_system.c` include `impl/tc264/*` (use `platform/target/target_platform.h`)
- Any code use `pal_gpio_init()` / `pal_pwm_set_duty()` etc. — use `McuIo_*` / `Device_*` instead

**MCU porting:** Add `code/impl/<target>/` with ops files + board bind. System/App/Service/Board code unchanged.
