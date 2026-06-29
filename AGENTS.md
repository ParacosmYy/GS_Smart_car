# Repository Guidelines

## Architecture

This is an AURIX TC264 smart car firmware project with link-time MCU ports.

```text
SDK Entry -> System Runtime -> App task table -> Services -> BSP -> Platform Interfaces
                                                             \-> TC264 link-time ports -> Vendor SDK
SDK Entry -> TC264 ISR adapter -> System IRQ facts -> Scheduler
```

- `code/app/`: lifecycle facade and application task table.
- `code/service/`: vision, sensor, control, diagnostics.
- `code/bsp/`: board-level motor, servo, display, input, buzzer modules.
- `code/platform/interface/`: neutral contracts: `mcu_io_if.h`, `device_if.h`.
- `code/platform/system/`: neutral system contracts: `system_port.h`, `irq_fact.h`.
- `code/impl/tc264/`: direct `McuIo_*` / `Device_*` implementations, board map, ISR adapter.
- `code/config/`: product parameters and board resources.
- `code/system/board/`: product board startup.
- `code/system/irq/`: IRQ fact -> event/tick posting.
- `user/`: TC264 SDK entry layer; keep `IFX_INTERRUPT` handlers thin.
- `libraries/`: SEEKFREE/iLLD vendor code; treat as read-only.

## Build

Build firmware with AURIX Development Studio:

```text
File -> Open Projects -> select this directory -> Build Project
```

This repository intentionally does not maintain host smoke tests, unit tests, or test scripts.

## Coding Rules

- C99, Allman braces, static allocation by default.
- File-static state uses `s_`; true globals use `g_`; pointer variables use `p_`; typedefs end in `_t`.
- App must not call Vision frame cleanup, element feedback, actuator output, or hardware APIs directly.
- Service public APIs should prefer snapshots or task entries over scattered single-value getters.
- Public headers must not include `zf_common_headfile.h`, `Ifx*`, `impl/*`, or board-resource headers.
- Non-vendor `.c` files must keep Doxygen `@file`, `@brief`, and function-level comments current.

## Guardrails

Do not reintroduce:

- runtime ops registration or platform dispatch source files;
- old PAL aliases or compatibility resource headers;
- duplicate IRQ fact headers or route tables;
- direct System/App/Service dependency on `impl/tc264`;
- `tests/`, `tests/smoke/`, smoke guards, host unit tests, or test runner scripts;
- broad host test suites or root Makefile wrappers.

MCU porting is done by replacing link-time port implementations under `code/impl/<target>/`, adding a target ISR adapter, and posting its facts through the System IRQ router.
