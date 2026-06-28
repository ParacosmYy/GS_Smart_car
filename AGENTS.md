# Repository Guidelines

## Project Structure & Module Organization

This repository is an AURIX TC264 smart car firmware project. Keep self-owned code under `code/` and maintain strict one-way dependencies.

- `code/app/`: application lifecycle, task registration, and main-loop orchestration only.
- `code/service/`: algorithm services and owned runtime state, such as `vision/`, `sensor/`, `control/`, and `diagnostics/`.
- `code/bsp/`: board-level drivers for motor, servo, display, input, and buzzer; depend on PAL only.
- `code/platform/`: platform contract only. `platform.h` defines stable `pal_*` APIs and must not include Vendor SDK headers.
- `code/impl/tc264/`: TC264 implementation of PAL and ISR hardware facts, including `platform_tc264.c` and `isr_adapter.c/h`.
- `code/system/irq/`: system interrupt bridge. `SmartcarIrqRouter` maps SDK ISR sources to adapter facts and scheduler events.
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
```

Run `git diff --check` before committing. Use host syntax checks for platform-independent files; verify TC264/Vendor-dependent files in ADS or hardware when available.

## Coding Style & Naming Conventions

Use C99, Allman braces, declarations near the top of functions, and no dynamic allocation. Naming patterns: `s_` file-static state, `g_` true globals, `p_` pointers, `*_t` typedefs, and `Module_Action()` functions. Section banners and file header labels may stay English; behavior and intent comments should be concise Chinese.

## Architecture Guardrails

Before writing code, think through future extensibility, maintainability, ownership, initialization order, ISR timing, and test impact. Use mature embedded product codebases such as DJI or Insta360 as style references: stable interfaces, explicit lifecycle, table-driven dispatch where useful, thin ISR entries, and clear module ownership.

Dependency direction is: `App -> Service/Handler -> BSP/Driver -> Platform API -> Impl -> Vendor SDK`. `user/isr.c` is SDK entry, not business logic. `code/system/irq` may publish scheduler events but must not call App, Service, BSP, motor, vision, or control code directly. `code/impl/tc264` may call Vendor SDK and PAL primitives, but must not call application logic.

Runtime state must have a clear owner in a `context` or `handler`. Do not add writable `extern` globals to public headers. Avoid new runtime state in `code/common/data.c`; it is legacy compatibility only.

## Testing Guidelines

Add host tests under `tests/` for platform-independent logic. Name tests `test_<module>.c`. For ISR/router changes, test event semantics where possible and document ADS or hardware validation gaps.

## Commit & Pull Request Guidelines

Use Conventional Commits with Chinese descriptions:

```text
refactor(system): 拆分中断路由与TC264实现层
docs(readme): 同步架构边界与目录说明
```

PRs should include scope, architectural impact, verification commands, and hardware validation status. Do not commit build artifacts such as `Debug/`, `.o`, `.elf`, `.hex`, or temporary `.exe` files.
