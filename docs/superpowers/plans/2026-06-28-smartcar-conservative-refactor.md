# Smartcar Conservative Refactor Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Clean the TC264 smartcar repository, fix one confirmed utility bug, and introduce a conservative app-layer entry point without rewriting the SeekFree libraries.

**Architecture:** Keep `libraries/` as Vendor/SDK and leave ADS/Eclipse project files intact. First introduce narrow self-owned boundaries under `code/app/`, while preserving existing function signatures and hardware calls.

**Tech Stack:** AURIX TC264, TASKING ADS/Eclipse project files, SeekFree TC264 library, C99-style embedded C, Git, MinGW GCC for host-only micro tests.

---

## File Structure

- Modify: `.gitignore` to ignore ADS/TASKING build output and local worktrees.
- Modify: `README.md` to document branch state, build entry, directory roles, and commit hygiene.
- Modify: `code/service.c` to fix `my_abs()` for negative inputs.
- Create: `tests/stubs/zf_common_headfile.h` to provide host-test typedefs without pulling the embedded SDK.
- Create: `tests/test_my_abs.c` to verify `my_abs()` behavior on host GCC.
- Create: `code/app/smartcar_app.h` to expose the app-layer entry points.
- Create: `code/app/smartcar_app.c` to move one-loop business scheduling out of `core0_main()`.
- Modify: `user/cpu0_main.c` to call `SmartcarApp_Init()` and `SmartcarApp_RunOnce()`.

## Task 1: Create An Isolated Worktree

**Files:**
- No production file changes.

- [ ] **Step 1: Create implementation worktree**

Run from `E:\Workplcace\embedded_workplace\GS_Smart_car`:

```bash
git worktree add C:\Users\Legion\.config\superpowers\worktrees\GS_Smart_car\smartcar-conservative-refactor -b refactor/smartcar-conservative
```

Expected: worktree created with branch `refactor/smartcar-conservative`.

- [ ] **Step 2: Verify baseline**

Run:

```bash
git status --short --branch
dir .project .cproject
```

Expected: clean status on `refactor/smartcar-conservative`; `.project` and `.cproject` exist. No automated ADS compile is run locally.

## Task 2: Repository Hygiene

**Files:**
- Modify: `.gitignore`
- Modify: `README.md`
- Remove from Git tracking: `Debug/`

- [ ] **Step 1: Add `.gitignore`**

Create `.gitignore` with:

```gitignore
# ADS / TASKING build output
Debug/
Release/
*.o
*.d
*.src
*.elf
*.hex
*.map
*.mdf
*.opt

# Local IDE and OS noise
.metadata/
*.launch
Thumbs.db
desktop.ini

# Local worktrees
.worktrees/
worktrees/
```

- [ ] **Step 2: Remove tracked build artifacts**

Run:

```bash
git rm -r --cached Debug
```

Expected: `Debug/` files are removed from Git tracking but not needed for source review.

- [ ] **Step 3: Update README**

Replace `README.md` with a concise project guide that states:

```markdown
# GS_Smart_car

AURIX TC264 smartcar firmware based on TASKING ADS/Eclipse and the SeekFree TC264 library.

## Branch Note

The source project currently lives on the `master` history. The `main` branch is a repository index and should not be used as the firmware working tree until the histories are reconciled.

## Directory Layout

| Path | Purpose |
| --- | --- |
| `user/` | TC264 core entry points and interrupt handlers generated or shaped by the SDK template. |
| `code/` | Self-owned smartcar application, vision, control, and utility code. |
| `libraries/` | SeekFree and Infineon SDK/vendor code. Treat as a stable dependency. |
| `.project`, `.cproject`, `.settings/` | ADS/Eclipse project metadata. Keep these tracked. |

## Build

Open this directory with AURIX Development Studio or TASKING ADS-compatible Eclipse, then build the existing project configuration.

## Contribution Rules

- Do not commit `Debug/`, object files, hex files, maps, or other generated build output.
- Keep `libraries/` changes separate from application changes.
- Prefer small commits: repository hygiene, bug fixes, and refactors should be separate.
```

- [ ] **Step 4: Verify hygiene**

Run:

```bash
git status --short
git check-ignore Debug/Seekfree_TC264_Opensource_Library.elf
```

Expected: `Debug/` files staged for removal, `.gitignore` and `README.md` modified, and the sample Debug file is ignored.

- [ ] **Step 5: Commit repository hygiene**

Run:

```bash
git add .gitignore README.md
git commit -m "chore(core): 清理智能车仓库构建产物"
```

## Task 3: Fix `my_abs()` With A Failing Test First

**Files:**
- Create: `tests/stubs/zf_common_headfile.h`
- Create: `tests/test_my_abs.c`
- Modify: `code/service.c`

- [ ] **Step 1: Add host-test stub**

Create `tests/stubs/zf_common_headfile.h`:

```c
#ifndef ZF_COMMON_HEADFILE_H_
#define ZF_COMMON_HEADFILE_H_

#include <stdint.h>

typedef int16_t int16;

#endif /* ZF_COMMON_HEADFILE_H_ */
```

- [ ] **Step 2: Add failing test**

Create `tests/test_my_abs.c`:

```c
#include <stdio.h>
#include "service.h"

static int expect_equal_int16(const char *name, int16 actual, int16 expected)
{
    if (actual != expected)
    {
        (void)printf("%s: expected %d, got %d\n", name, (int)expected, (int)actual);
        return 1;
    }
    return 0;
}

int main(void)
{
    int failures = 0;

    failures += expect_equal_int16("positive", my_abs(7), 7);
    failures += expect_equal_int16("zero", my_abs(0), 0);
    failures += expect_equal_int16("negative", my_abs(-7), 7);

    return failures;
}
```

- [ ] **Step 3: Run test and verify RED**

Run:

```bash
gcc -Itests/stubs -Icode tests/test_my_abs.c code/service.c -o tests/test_my_abs.exe
tests\test_my_abs.exe
```

Expected: executable returns non-zero and prints `negative: expected 7, got -7`.

- [ ] **Step 4: Fix `my_abs()` minimally**

Change `code/service.c` function body to:

```c
int16 my_abs(int16 value)
{
    if(value > 0)
    {
        return value;
    }
    else if(value < 0)
    {
        return (int16)(-value);
    }
    else
    {
        return 0;
    }
}
```

- [ ] **Step 5: Run test and verify GREEN**

Run:

```bash
gcc -Itests/stubs -Icode tests/test_my_abs.c code/service.c -o tests/test_my_abs.exe
tests\test_my_abs.exe
```

Expected: exit code `0` and no output.

- [ ] **Step 6: Commit bug fix**

Run:

```bash
git add tests/stubs/zf_common_headfile.h tests/test_my_abs.c code/service.c
git commit -m "fix(core): 修复绝对值计算错误"
```

## Task 4: Introduce App Layer Entry Point

**Files:**
- Create: `code/app/smartcar_app.h`
- Create: `code/app/smartcar_app.c`
- Modify: `user/cpu0_main.c`

- [ ] **Step 1: Create app header**

Create `code/app/smartcar_app.h`:

```c
#ifndef CODE_APP_SMARTCAR_APP_H_
#define CODE_APP_SMARTCAR_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

void SmartcarApp_Init(void);
void SmartcarApp_RunOnce(void);

#ifdef __cplusplus
}
#endif

#endif /* CODE_APP_SMARTCAR_APP_H_ */
```

- [ ] **Step 2: Create app implementation**

Create `code/app/smartcar_app.c`:

```c
#include "smartcar_app.h"
#include "zf_common_headfile.h"

void SmartcarApp_Init(void)
{
    init_all();
    pit_init_all();
}

void SmartcarApp_RunOnce(void)
{
    if(mt9v03x_finish_flag == 1)
    {
        task_calculte();
    }

    tft180_show_gray_image(0, 0, mt9v03x_image_bandw_zip[0], 94, 60, MT9V03X_W / 2, MT9V03X_H / 2, 0);

    tft180_show_string(0, 80, "left:");
    tft180_show_int(50, 80, left_encoder_speed, 4);

    tft180_show_string(0, 60, "right:");
    tft180_show_int(50, 60, right_encoder_speed, 4);

    tft180_show_string(0, 100, "l_spd:");
    tft180_show_string(0, 120, "r_spd:");

    tft180_show_int(50, 100, (int32)left_motor_pid_output, 6);
    tft180_show_int(50, 120, (int32)right_motor_pid_output, 6);

    tft180_show_string(0, 140, "err:");
    tft180_show_int(50, 140, calculate_error, 4);
}
```

- [ ] **Step 3: Include app header in `core0_main`**

Add this include after `#include "zf_common_headfile.h"`:

```c
#include "smartcar_app.h"
```

- [ ] **Step 4: Replace initialization calls**

Replace:

```c
    init_all();

    pit_init_all();
```

with:

```c
    SmartcarApp_Init();
```

- [ ] **Step 5: Replace main loop body**

Inside `while (TRUE)`, replace the active application and TFT display statements with:

```c
        SmartcarApp_RunOnce();
```

Keep existing commented debug blocks out of the loop body only if they do not obscure the active flow.

- [ ] **Step 6: Verify source references**

Run:

```bash
rg -n "SmartcarApp_|init_all\\(|pit_init_all\\(|task_calculte\\(" code user
```

Expected: `SmartcarApp_Init()` and `SmartcarApp_RunOnce()` appear in the new app files and `cpu0_main.c`; `task_calculte()` is called by `smartcar_app.c`.

- [ ] **Step 7: Commit app entry point**

Run:

```bash
git add code/app/smartcar_app.h code/app/smartcar_app.c user/cpu0_main.c
git commit -m "refactor(main): 收敛智能车主循环入口"
```

## Task 5: Final Verification

**Files:**
- No expected production changes.

- [ ] **Step 1: Run host test**

Run:

```bash
gcc -Itests/stubs -Icode tests/test_my_abs.c code/service.c -o tests/test_my_abs.exe
tests\test_my_abs.exe
```

Expected: exit code `0`.

- [ ] **Step 2: Check ignored generated executable**

Run:

```bash
git status --short
```

Expected: no tracked build products. If `tests/test_my_abs.exe` appears untracked, add `*.exe` under a test-output section in `.gitignore` and commit that hygiene fix.

- [ ] **Step 3: Inspect recent commits**

Run:

```bash
git log --oneline -5
```

Expected: plan commit plus three implementation commits for hygiene, bug fix, and app entry.

