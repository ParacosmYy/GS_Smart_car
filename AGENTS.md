# Repository Guidelines

## Architecture

This is an AURIX TC264 smart car firmware project with link-time MCU ports.

```text
SDK Entry -> System Runtime -> App -> Service Tasks -> Services -> Platform Interfaces
                                                             \-> TC264 link-time ports -> Vendor SDK
System Runtime -> Board Contract -> BSP -> Platform Interfaces
```

- `code/app/`: lifecycle facade only.
- `code/service/`: task registry plus vision, sensor, control, diagnostics.
- `code/bsp/`: board-level motor, servo, display, input, buzzer modules.
- `code/platform/interface/`: neutral contracts: `mcu_io_if.h`, `device_if.h`, `service_port_if.h`.
- `code/platform/system/`: neutral system contracts: `system_port.h`, `irq_fact.h`.
- `code/impl/tc264/`: direct `McuIo_*` / `Device_*` implementations, board map, IRQ routes, ISR adapter.
- `code/config/`: product parameters and board resources.
- `code/system/board/`: product board startup and service-port binding.
- `code/system/irq/`: target route contract plus IRQ fact -> event/tick posting.
- `user/`: TC264 SDK entry layer; keep `IFX_INTERRUPT` handlers thin.
- `libraries/`: SEEKFREE/iLLD vendor code; treat as read-only.

## Build And Verification

Build firmware with AURIX Development Studio:

```text
File -> Open Projects -> select this directory -> Build Project
```

Run the maintained host guard:

```powershell
powershell -ExecutionPolicy Bypass -File scripts/check_syntax.ps1
```

It builds and runs `tests/smoke/smoke.c` for event, scheduler, IRQ router, and vision, then syntax-checks app/service task wiring.

## Coding Rules

- C99, Allman braces, static allocation by default.
- File-static state uses `s_`; true globals use `g_`; pointer variables use `p_`; typedefs end in `_t`.
- App must not call Vision frame cleanup, element feedback, actuator output, or hardware APIs directly.
- Service public APIs should prefer snapshots or task entries over scattered single-value getters.
- Public headers must not include `zf_common_headfile.h`, `Ifx*`, `impl/*`, or board-resource headers.

## Guardrails

Do not reintroduce:

- runtime ops registration or platform dispatch source files;
- old PAL aliases or compatibility resource headers;
- duplicate IRQ fact headers;
- direct System/App/Service dependency on `impl/tc264`;
- broad host test suites or root Makefile wrappers.

MCU porting is done by replacing link-time port implementations under `code/impl/<target>/` and providing that target's IRQ route table for the System IRQ router.
