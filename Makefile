SHELL := powershell.exe
.SHELLFLAGS := -NoProfile -ExecutionPolicy Bypass -Command

CC = gcc
BUILD_DIR ?= build

.PHONY: help test syntax clean

help:
	@Write-Host "Targets:"
	@Write-Host "  make syntax     Run GCC syntax-only checks for project sources"
	@Write-Host "  make test       Alias of syntax, no unit-test framework required"
	@Write-Host "  make clean      Remove build outputs"
	@Write-Host ""
	@Write-Host "Variables:"
	@Write-Host "  CC=gcc          Override GCC executable"

test:
	@powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_syntax.ps1 -Gcc "$(CC)"

syntax:
	@powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_syntax.ps1 -Gcc "$(CC)"

clean:
	@if (Test-Path "$(BUILD_DIR)") { Remove-Item -Recurse -Force "$(BUILD_DIR)" }
	@Write-Host "Cleaned $(BUILD_DIR)"
