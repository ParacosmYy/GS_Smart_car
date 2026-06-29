# Repository Guidelines

## Architecture

This is a smart car firmware project with a stable HAL contract and link-time MCU ports by default. The current target is AURIX TC264.

```text
SDK Entry -> System Runtime -> App task table -> Services -> BSP -> HAL Contract
                                                             \-> Target <target> port -> Vendor SDK
SDK Entry -> Target <target> IRQ adapter -> Scheduler
```

- `code/app/`: lifecycle facade and application task table.
- `code/service/`: vision, sensor, control, diagnostics.
- `code/bsp/`: board-level actuator, motor, servo, display, input, buzzer modules.
- `code/hal/hal.h`: project-specific `SmartcarHal_*` contract.
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
- HAL interaction uses the project-specific `SmartcarHal_*` prefix.
- Public headers must not include `zf_common_headfile.h`, `Ifx*`, `target/*`, or board-resource headers.
- Non-vendor `.c` files must keep Doxygen `@file`, `@brief`, and function-level comments current.

## Porting And Dispatch Policy

Default MCU porting is done by replacing link-time HAL implementations under `code/target/<target>/`, switching vendor/project configuration, and letting that target's IRQ adapter post scheduler events/ticks directly.

The current baseline must not add runtime ops registration files or HAL dispatch source files. The policy below only defines future, conditional exceptions.

Runtime ops registration or HAL dispatch may be introduced only as a deliberate architecture change when link-time replacement is no longer enough. Acceptable triggers include runtime board detection, multiple hardware revisions in one firmware image, selectable mock/simulation backends, or shared middleware that truly needs dependency injection.

Runtime dispatch changes must satisfy all of these constraints:

- keep `code/hal/hal.h` and `SmartcarHal_*` as the only stable upper-layer contract, with no Vendor, `Ifx*`, `zf_common_headfile.h`, `target/*`, or board-resource leakage;
- add a short ADR or README section explaining why link-time ports are insufficient, what is dispatched, and where ownership lives;
- keep one composition root for binding ops, preferably in `system/` or a clearly named `hal/` module, and avoid scattering registration calls through App/Service/BSP code;
- use static allocation or `const` ops tables by default; dynamic allocation needs the project memory-safety justification and must not appear in ISR or hot data paths;
- make dispatch fail closed: validate required function pointers during system startup and expose an explicit init failure path rather than allowing null calls later;
- keep target implementations in `code/target/<target>/` or target-owned impl files; HAL defines contracts and dispatch, Target binds Vendor;
- avoid duplicate mechanisms for the same concern. Do not keep parallel link-time and runtime paths unless one is explicitly marked transitional and removed in the same migration series;
- update README porting instructions and build configuration in the same change;
- verify with AURIX Development Studio build, plus focused static checks for forbidden includes and duplicate symbols.

Do not reintroduce:

- old `platform/port_if.h` / `sensor_hal.h` contracts;
- old PAL aliases or compatibility resource headers;
- duplicate IRQ fact headers, IRQ routers, or route tables;
- direct System/App/Service dependency on `code/target/<target>`;
- `tests/`, `tests/smoke/`, smoke guards, host unit tests, or test runner scripts;
- broad host test suites or root Makefile wrappers.
