# Repository Guidelines

## Architecture

This is a smart car firmware project with link-time MCU ports. The current target is AURIX TC264.

```text
SDK Entry -> System Runtime -> App task table -> Services -> BSP -> Platform Port
                                                             \-> Target <target> port -> Vendor SDK
SDK Entry -> Target <target> IRQ adapter -> Scheduler
```

- `code/app/`: lifecycle facade and application task table.
- `code/service/`: vision, sensor, control, diagnostics.
- `code/bsp/`: board-level motor, servo, display, input, buzzer modules.
- `code/platform/port_if.h`: neutral `SystemPort_*`, `McuIo_*`, and `Device_*` contracts.
- `code/target/<target>/`: link-time OPS port implementations, target-local resource maps, IRQ adapter, IRQ priority policy.
- `code/config/`: product parameters and board resources.
- `code/system/board/`: product board startup.
- `user/`: current SDK entry layer; keep interrupt handlers thin and forward to target IRQ entry points.
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
- Public headers must not include `zf_common_headfile.h`, `Ifx*`, `target/*`, or board-resource headers.
- Non-vendor `.c` files must keep Doxygen `@file`, `@brief`, and function-level comments current.

## Guardrails

Do not reintroduce:

- runtime ops registration or platform dispatch source files;
- old PAL aliases or compatibility resource headers;
- duplicate IRQ fact headers, IRQ routers, or route tables;
- direct System/App/Service dependency on `code/target/<target>`;
- `tests/`, `tests/smoke/`, smoke guards, host unit tests, or test runner scripts;
- broad host test suites or root Makefile wrappers.

MCU porting is done by replacing link-time port implementations under `code/target/<target>/`, switching vendor/project configuration, and letting that target's IRQ adapter post scheduler events/ticks directly.
