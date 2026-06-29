param(
    [string]$BuildDir = "build/host-smoke"
)

$ErrorActionPreference = "Stop"

$gcc = Get-Command gcc -ErrorAction SilentlyContinue
if (-not $gcc) {
    throw "gcc not found in PATH"
}

New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null

$exe = Join-Path $BuildDir "smoke.exe"
$sources = @(
    "tests/smoke/smoke.c",
    "code/scheduler/event.c",
    "code/scheduler/scheduler.c",
    "code/system/irq/smartcar_irq_router.c",
    "code/service/vision/vision.c"
)

$includePaths = @(
    "-Icode",
    "-Icode/config",
    "-Icode/platform",
    "-Icode/scheduler",
    "-Icode/system/irq",
    "-Icode/service/vision"
)

$args = @(
    "-std=c99",
    "-Wall",
    "-Wextra",
    "-Werror=implicit-function-declaration"
) + $includePaths + $sources + @("-o", $exe)

& $gcc.Source @args
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

& $exe
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

$syntaxSources = @(
    "code/app/smartcar_app.c",
    "code/service/smartcar_tasks.c"
)

$syntaxIncludes = @(
    "-Icode",
    "-Icode/app",
    "-Icode/scheduler",
    "-Icode/service",
    "-Icode/service/control",
    "-Icode/service/diagnostics",
    "-Icode/service/sensor",
    "-Icode/service/vision",
    "-Icode/platform",
    "-Icode/platform/interface"
)

$syntaxArgs = @(
    "-std=c99",
    "-Wall",
    "-Wextra",
    "-Werror=implicit-function-declaration",
    "-fsyntax-only"
) + $syntaxIncludes + $syntaxSources

& $gcc.Source @syntaxArgs
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

Write-Host "smoke-ok"
