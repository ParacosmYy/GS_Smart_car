param(
    [string]$Gcc = "gcc"
)

$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = Resolve-Path (Join-Path $ScriptDir "..")
$BuildDir = Join-Path $RepoRoot "build/host-tests"

New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null

$CommonArgs = @(
    "-std=c99",
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

$Tests = @(
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

    foreach ($Test in $Tests) {
        $ExePath = Join-Path $BuildDir ($Test.Name + ".exe")
        $Args = @()
        $Args += $CommonArgs
        $Args += $Test.Defines
        $Args += $Test.Sources
        $Args += @("-o", $ExePath)

        Write-Host ""
        Write-Host "==> Building $($Test.Name)"
        Write-Host "$Gcc $($Args -join ' ')"
        & $Gcc @Args
        if ($LASTEXITCODE -ne 0) {
            $Failures += "$($Test.Name): build failed ($LASTEXITCODE)"
            continue
        }

        Write-Host "==> Running $($Test.Name)"
        & $ExePath
        if ($LASTEXITCODE -ne 0) {
            $Failures += "$($Test.Name): run failed ($LASTEXITCODE)"
        }
    }

    Write-Host ""
    Write-Host "Host test output directory: $BuildDir"
    if ($Failures.Count -eq 0) {
        Write-Host "Host tests: PASS ($($Tests.Count)/$($Tests.Count))"
        exit 0
    }

    Write-Host "Host tests: FAIL ($($Failures.Count) failure(s))"
    foreach ($Failure in $Failures) {
        Write-Host " - $Failure"
    }
    exit 1
}
finally {
    Pop-Location
}
