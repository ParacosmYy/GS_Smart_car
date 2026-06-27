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
