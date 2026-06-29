# Smartcar HAL Prefix Refactor Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Replace the legacy generic platform contract with a project-specific `SmartcarHal_*` HAL contract and remove redundant forwarding layers.

**Architecture:** Upper layers include only `code/hal/hal.h`; TC264 remains the link-time target implementation under `code/target/tc264/`. BSP keeps zero-cost facades, while element-to-buzzer policy lives in `feedback_service.c`.

**Tech Stack:** C99 firmware for AURIX TC264, SEEKFREE/iLLD vendor SDK, AURIX Development Studio project metadata.

---

### Task 1: HAL Contract And TC264 Target

**Files:**
- Create: `code/hal/hal.h`
- Modify: `code/config/smartcar_board_resources.h`
- Modify: `code/target/tc264/ops_tc264.c`
- Modify: `code/target/tc264/ops_tc264_irq.c`
- Delete: `code/platform/port_if.h`
- Delete: `code/platform/sensor_hal.h`

- [x] **Step 1: Create the unified HAL public header**

Define `SmartcarHal_*` system, GPIO, PWM, UART, encoder, PIT, camera, display, IMU, wireless, and encoder snapshot APIs in `code/hal/hal.h`. Include `stdbool.h` and `stdint.h`, keep C++ guards, and keep vendor/target/board headers out of the public header.

- [x] **Step 2: Update board resource ID typedefs**

Change `smartcar_board_resources.h` to include `hal/hal.h` and derive product resource IDs from `smartcar_hal_*_id_t`, with no remaining `mcuio_*` type names.

- [x] **Step 3: Rename target implementations**

In `ops_tc264.c`, keep the existing link-time implementation and comments, but rename exported definitions from `SystemPort_*`, `McuIo_*`, and `Device_*` to the corresponding `SmartcarHal_*` symbols. Rename `Device_CameraGetFrameDesc` to `SmartcarHal_CameraGetDesc` and use `smartcar_hal_camera_desc_t`.

- [x] **Step 4: Rename target IRQ HAL usage**

In `ops_tc264_irq.c`, include `hal/hal.h`, use `smartcar_hal_pit_id_t`, call `SmartcarHal_IrqCtrl`, `SmartcarHal_PitClearFlag`, `SmartcarHal_EncoderGet`, and `SmartcarHal_EncoderClear`, and expose the encoder snapshot as `SmartcarHal_EncoderTakeSnapshot`.

- [x] **Step 5: Remove obsolete contract headers**

Delete `code/platform/port_if.h` and `code/platform/sensor_hal.h` after all include sites have been migrated.

- [x] **Step 6: Verify Task 1**

Run:

```powershell
rg -n "SystemPort_|McuIo_|Device_|SensorHal_|camera_frame_desc_t|mcuio_|MCUIO_|platform/(port_if|sensor_hal)" code/config code/target code/hal
rg -n "zf_common_headfile|Ifx|target/|smartcar_board_resources" code/hal/hal.h
```

Expected: no old symbols in the scanned project-owned HAL/target/config files; no forbidden public-header dependencies in `hal.h`.

### Task 2: Upper-Layer Call-Site Migration

**Files:**
- Modify: `code/scheduler/event.c`
- Modify: `code/app/smartcar_app.c`
- Modify: `code/system/runtime/smartcar_system.c`
- Modify: `code/system/board/smartcar_board.c`
- Modify: `code/bsp/buzzer.c`
- Modify: `code/bsp/motor.c`
- Modify: `code/bsp/servo.c`
- Modify: `code/bsp/display.c`
- Modify: `code/bsp/input.c`
- Modify: `code/service/sensor/sensor.c`
- Modify: `code/service/vision/vision.c`
- Modify: `code/service/vision/vision.h`
- Modify: `code/service/diagnostics/debug_display.c`
- Modify: `user/cpu1_main.c`

- [x] **Step 1: Replace include paths**

Replace `#include "platform/port_if.h"` and `#include "platform/sensor_hal.h"` with `#include "hal/hal.h"` in every owned call-site file.

- [x] **Step 2: Replace function names and comments**

Apply the old-to-new mapping: `SystemPort_*`, `McuIo_*`, `Device_*`, and `SensorHal_EncoderTakeSnapshot` become their `SmartcarHal_*` equivalents. Update comments that mention old names, especially in `vision.c`.

- [x] **Step 3: Replace public HAL types and constants**

Change `mcuio_pwm_id_t` to `smartcar_hal_pwm_id_t`, `mcuio_gpio_id_t` to `smartcar_hal_gpio_id_t`, `camera_frame_desc_t` to `smartcar_hal_camera_desc_t`, and `MCUIO_GPIO_INPUT/OUTPUT` to `SMARTCAR_HAL_GPIO_INPUT/OUTPUT`.

- [x] **Step 4: Verify Task 2**

Run:

```powershell
rg -n "SystemPort_|McuIo_|Device_|SensorHal_|camera_frame_desc_t|mcuio_|MCUIO_|platform/(port_if|sensor_hal)" code/scheduler code/system code/bsp code/service user
```

Expected: no old HAL symbols or old platform includes in migrated upper layers.

### Task 3: Actuator, Feedback, Docs, And Project Metadata

**Files:**
- Modify: `code/bsp/actuator.h`
- Delete: `code/bsp/actuator.c`
- Modify: `code/service/diagnostics/feedback_service.c`
- Modify: `code/service/diagnostics/feedback_service.h`
- Modify: `AGENTS.md`
- Modify: `README.md`
- Modify: `.cproject`

- [x] **Step 1: Inline actuator facade**

Make `actuator.h` include `motor.h`, `servo.h`, and `buzzer.h`, then implement `Actuator_SetServo`, `Actuator_SetMotorLeft`, `Actuator_SetMotorRight`, `Actuator_TickFeedback`, `Actuator_IsBuzzerBusy`, and `Actuator_TriggerBuzzer` as `static inline` wrappers.

- [x] **Step 2: Move element-to-buzzer policy into feedback service**

Remove `Actuator_NotifyTrackElement`. In `feedback_service.c`, map `ELEM_NONE`, `ELEM_RING`, and `ELEM_CROSSROAD` to `BUZZER_EVENT_NONE`, `BUZZER_EVENT_RING`, and `BUZZER_EVENT_CROSSROAD`, then call `Actuator_TriggerBuzzer`.

- [x] **Step 3: Update repository docs**

Update `AGENTS.md` and `README.md` so the stable upper-layer contract is `code/hal/hal.h` with `SmartcarHal_*`. Remove old `port_if.h`, `sensor_hal.h`, `SystemPort_*`, `McuIo_*`, `Device_*`, and `SensorHal_*` references except where explicitly describing things not to reintroduce.

- [x] **Step 4: Update ADS include paths**

In `.cproject`, replace every include path entry for `code/platform` with `code/hal`.

- [x] **Step 5: Verify Task 3**

Run:

```powershell
rg -n "Actuator_NotifyTrackElement|code/platform|port_if|sensor_hal|SystemPort_|McuIo_|Device_|SensorHal_" AGENTS.md README.md .cproject code/bsp/actuator.h code/service/diagnostics/feedback_service.c
rg -n "code/hal|SmartcarHal_|Actuator_TriggerBuzzer|Actuator_IsBuzzerBusy" AGENTS.md README.md .cproject code/bsp/actuator.h code/service/diagnostics/feedback_service.c
```

Expected: no stale positive references to the removed platform contract; new HAL and actuator inline APIs are documented and present.

- [x] **Step 6: Address final review cleanup**

Update stale HAL boundary wording in `smartcar_board_resources.h` and share the vision element result as `vision_track_element_t` between `Vision_DetectElement()` and `FeedbackService_NotifyTrackElement()` so the feedback mapping no longer duplicates raw element IDs.

### Task 4: Integrated Verification

**Files:**
- Inspect all non-vendor project files under `code/`, `user/`, `AGENTS.md`, `README.md`, `.cproject`, and this plan.

- [x] **Step 1: Scan for old symbols**

Run:

```powershell
rg -n "SystemPort_|McuIo_|Device_|SensorHal_|camera_frame_desc_t|mcuio_|MCUIO_|platform/(port_if|sensor_hal)|port_if.h|sensor_hal.h" code user AGENTS.md README.md .cproject
```

Expected: no old contract symbols remain outside deliberate historical notes that say not to reintroduce them.

- [x] **Step 2: Scan public HAL dependency hygiene**

Run:

```powershell
rg -n "zf_common_headfile|Ifx|target/|smartcar_board_resources" code/hal/hal.h
```

Expected: no matches.

- [x] **Step 3: Confirm obsolete files are gone**

Run:

```powershell
Test-Path code/platform/port_if.h
Test-Path code/platform/sensor_hal.h
Test-Path code/bsp/actuator.c
```

Expected: all three commands print `False`.

- [x] **Step 4: Review final diff**

Run:

```powershell
git diff --stat
git diff -- code user
git diff -- AGENTS.md README.md .cproject docs/superpowers/plans/2026-06-30-smartcar-hal-prefix-refactor.md
```

Expected: changes are scoped to HAL contract migration, actuator inline refactor, feedback mapping, docs, ADS include paths, and this plan.
