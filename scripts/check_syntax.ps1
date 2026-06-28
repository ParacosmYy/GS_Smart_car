param(
    [string]$Gcc = "gcc"
)

$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = Resolve-Path (Join-Path $ScriptDir "..")
$BuildDir = Join-Path $RepoRoot "build/syntax"

New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null

$CommonArgs = @(
    "-std=c99",
    "-Wall",
    "-Wextra",
    "-Icode",
    "-Itests/stubs",
    "-Icode/common",
    "-Icode/config",
    "-Icode/scheduler",
    "-Icode/platform/interface",
    "-Icode/platform/system",
    "-Icode/system/irq",
    "-Icode/system/board",
    "-Icode/impl/tc264",
    "-Icode/service/vision"
)

$Units = @(
    @{
        Name = "test_my_abs"
        Defines = @()
        Sources = @("tests/test_my_abs.c", "code/common/utils.c")
    },
    @{
        Name = "test_event"
        Defines = @()
        Sources = @("tests/test_event.c", "code/scheduler/event.c")
    },
    @{
        Name = "test_scheduler"
        Defines = @()
        Sources = @("tests/test_scheduler.c", "code/scheduler/scheduler.c")
    },
    @{
        Name = "test_irq_router"
        Defines = @()
        Sources = @("tests/test_irq_router.c", "code/system/irq/smartcar_irq_router.c")
    },
    @{
        Name = "test_board_map"
        Defines = @()
        Sources = @("tests/test_board_map.c", "code/impl/tc264/tc264_board_map.c")
    },
    @{
        Name = "test_vision_algo"
        Defines = @("-DVISION_ENABLE_TEST_ACCESS")
        Sources = @("tests/test_vision_algo.c", "code/service/vision/vision.c", "code/common/utils.c")
    }
)

Push-Location $RepoRoot
try {
    $Failures = @()

    foreach ($Unit in $Units) {
        $Args = @()
        $Args += $CommonArgs
        $Args += $Unit.Defines
        $Args += "-fsyntax-only"
        $Args += $Unit.Sources

        Write-Host ""
        Write-Host "==> Syntax checking $($Unit.Name)"
        Write-Host "$Gcc $($Args -join ' ')"
        & $Gcc @Args
        if ($LASTEXITCODE -ne 0) {
            $Failures += "$($Unit.Name): syntax failed ($LASTEXITCODE)"
        }
    }

    Write-Host ""
    Write-Host "Syntax output directory: $BuildDir"
    if ($Failures.Count -eq 0) {
        Write-Host "Syntax checks: PASS ($($Units.Count)/$($Units.Count))"
        exit 0
    }

    Write-Host "Syntax checks: FAIL ($($Failures.Count) failure(s))"
    foreach ($Failure in $Failures) {
        Write-Host " - $Failure"
    }
    exit 1
}
finally {
    Pop-Location
}
