# Repository Guidelines

## Project Structure & Module Organization

This repository is an AURIX TC264 smart car firmware project. Keep self-owned code under `code/` and maintain strict one-way dependencies.

- `code/app/`: application task table and `SmartcarApp_RunOnce()` only; no board init, PIT start, or Vendor macros.
- `code/system/runtime/`: boot composition root between SDK entry and App.
- `code/system/board/`: product board startup sequence, device init, and periodic IRQ start policy.
- `code/system/irq/`: generic interrupt routing from source/fact to scheduler event/tick.
- `code/service/`: algorithm services and owned runtime state, such as `vision/`, `sensor/`, `control/`, and `diagnostics/`.
- `code/bsp/`: board-level drivers for motor, servo, display, input, and buzzer; depend on PAL only.
- `code/platform/`: platform contract only. Use `common/`, `mcu/`, `device/`, and `system/` capability headers; `platform.h` is a legacy compatibility aggregator and must not include Vendor SDK headers.
- `code/impl/tc264/`: TC264 PAL implementation, ISR hardware facts, and target IRQ port (`tc264_irq_port.c/h`).
- `code/common/init.h`: SEEKFREE compatibility shim only; do not add startup logic here.
- `user/`: TC264 SDK entry layer with `IFX_INTERRUPT` functions; keep it thin.
- `libraries/`: Infineon iLLD and SEEKFREE Vendor SDK; treat as read-only.
- `tests/`: host-side GCC tests and stubs.

## Build, Test, and Development Commands

Firmware is normally built in AURIX Development Studio by opening the project root and running **Build Project**.

```bash
gcc -Itests/stubs -Icode/common tests/test_my_abs.c code/common/utils.c -o test_my_abs.exe
./test_my_abs.exe

gcc -Itests/stubs -Icode/platform -Icode/scheduler tests/test_event.c code/scheduler/event.c -o test_event.exe
./test_event.exe

gcc -Itests/stubs -Icode/scheduler tests/test_scheduler.c code/scheduler/scheduler.c -o test_scheduler.exe
./test_scheduler.exe
```

Run `git diff --check` before committing. Host syntax checks should cover `code/app`, `code/system`, `code/service`, `code/bsp`, `code/scheduler`, and `code/common`. Verify `platform_tc264.c`, `isr_adapter.c`, and `user/isr.c` in ADS or hardware because they depend on TC264/SEEKFREE macros.

## Coding Style & Naming Conventions

Use C99, Allman braces, declarations near the top of functions, and no dynamic allocation. Naming patterns: `s_` file-static state, `g_` true globals, `p_` pointers, `*_t` typedefs, and `Module_Action()` functions. Section banners and file header labels may stay English; behavior and intent comments should be concise Chinese.

## Architecture Guardrails

Before writing code, first identify the owner, dependency direction, lifecycle timing, ISR impact, test surface, and future MCU-porting cost. Use mature embedded product codebases such as DJI or Insta360 as style references: stable interfaces, explicit lifecycle, table-driven dispatch where useful, thin ISR entries, and clear module ownership.

Dependency direction is: `SDK Entry -> System Runtime -> App -> Service/Handler -> BSP/Driver -> Platform API -> Impl -> Vendor SDK`. `user/isr.c` is SDK entry, not business logic. `code/system/board` may initialize BSP devices but must not call business services. `code/system/runtime` is the composition root for service lifecycle such as `Control_Init()`. `code/system/irq` may publish scheduler events but must not call App, Service, BSP, motor, vision, or control code directly. `code/impl/tc264` may call Vendor SDK and PAL primitives, but must not call application logic.

New production code must not include `platform.h`; include the narrow PAL header it needs, such as `platform/mcu/pal_gpio.h`, `platform/mcu/pal_pwm.h`, `platform/device/pal_camera.h`, `platform/device/pal_display.h`, or `platform/system/pal_system.h`. Keep `platform.h` only for legacy compatibility aggregation.

Runtime state must have a clear owner in a `context` or `handler`. Do not add writable `extern` globals to public headers. Avoid new runtime state in `code/common/data.c`; it is legacy compatibility only.

For MCU portability, a new target should add `code/impl/<target>/platform_<target>.c`, `<target>_irq_binding.c/h`, and SDK entry glue. Do not change App or Service code just to replace the MCU. If a platform-specific detail leaks upward, extend PAL or add a BSP capability interface instead.

## Testing Guidelines

Add host tests under `tests/` for platform-independent logic. Name tests `test_<module>.c`. For ISR/router changes, test event semantics where possible and document ADS or hardware validation gaps.

## Commit & Pull Request Guidelines

Use Conventional Commits with Chinese descriptions:

```text
refactor(system): 拆分中断路由与TC264实现层
docs(readme): 同步架构边界与目录说明
```

PRs should include scope, architectural impact, verification commands, and hardware validation status. Do not commit build artifacts such as `Debug/`, `.o`, `.elf`, `.hex`, or temporary `.exe` files.
