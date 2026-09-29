# MPU-6500 DMP Library — Port Design

Date: 2026-09-29

## Goal

Turn this repository (a copy of `xreef/MPU-9250-DMP_Library`) into a standalone
library for the InvenSense MPU-6500. Every file, symbol and document refers to the
6500, not the 9250.

The MPU-6500 is a 6-DOF device (accelerometer + gyroscope). It has no AK8963
magnetometer, and its WHO_AM_I value is `0x70` (the 9250 returns `0x71`).

## Decisions

- **Magnetometer:** removed entirely. There are no stubs and no optional external-compass support.
- **Git:** keep the history, set `origin` to `https://github.com/xreef/MPU-6500-DMP_Library.git`,
  commit on `master` and do not push. The user creates the GitHub repository.
- **Approach:** a full port with an API that mirrors the 9250 library, minus the magnetometer.

## 1. File layout

| Old | New |
|---|---|
| `src/MPU9250-DMP.h` / `.cpp` | `src/MPU6500-DMP.h` / `.cpp` |
| `src/MPU9250_RegisterMap.h` | `src/MPU6500_RegisterMap.h` |
| `src/invesense/arduino_mpu9250_clk.c/.h` | `src/invesense/arduino_mpu6500_clk.c/.h` |
| `src/invesense/arduino_mpu9250_i2c.cpp/.h` | `src/invesense/arduino_mpu6500_i2c.cpp/.h` |
| `src/invesense/arduino_mpu9250_log.cpp/.h` | `src/invesense/arduino_mpu6500_log.cpp/.h` |
| `examples/MPU9250_<X>/MPU9250_<X>.ino` | `examples/MPU6500_<X>/MPU6500_<X>.ino` |
| `examples/MPU9250_DPM_wom` | `examples/MPU6500_DMP_wom` (typo fixed) |

- InvenSense file names stay unchanged: `inv_mpu.c/.h`, `inv_mpu_dmp_motion_driver.c/.h`,
  `dmpKey.h`, `dmpmap.h`.
- Deleted as orphans: the root-level `invesense/` folder (a stale copy that is never compiled)
  and `src/invesense/MPU9250_RegisterMap.h` (no file includes it).
- All `#include` directives, include guards (`_SPARKFUN_MPU9250_DMP_H_` → `_MPU6500_DMP_H_` style)
  and file header comments are updated to match.

## 2. InvenSense driver configuration

- The class header defines `MPU6500` only. `MPU9250`, `AK8963_SECONDARY` and `COMPASS_ENABLED`
  are removed.
- The `AK89xx_SECONDARY` blocks in `inv_mpu.c` and `inv_mpu_dmp_motion_driver.c` therefore drop
  out through the preprocessor. Vendor logic stays unmodified.
- Any remaining references to compass functions that are unguarded and would break the build
  get the minimal fix required: guard or delete them.
- Where the vendor files include the `arduino_mpu9250_*` helper headers, those includes change
  to `arduino_mpu6500_*`. Other than that, the vendor comments about chip variants stay as they are.

## 3. `MPU6500_DMP` class

- The class is renamed from `MPU9250_DMP` to `MPU6500_DMP`.
- **Removed:** members `mx`, `my`, `mz`, `heading`, `_mSense`; methods `updateCompass`, `calcMag`,
  `getMagFSR`, `getMagSens`, `setCompassSampleRate`, `getCompassSampleRate`,
  `computeCompassHeading`; the constant `UPDATE_COMPASS`.
- `begin()` enables `INV_XYZ_GYRO | INV_XYZ_ACCEL` and no longer calls `mpu_set_bypass(1)`.
- The default argument of `update()` becomes `UPDATE_ACCEL | UPDATE_GYRO`.
- `computeEulerAngles` stays. The DMP delivers a 6-axis quaternion, so yaw drifts over time.
  This is documented in the header comment and in the README.
- The self-test and any other API that reports compass status bits updates its comments to
  cover accel and gyro only.

## 4. Register map

- `MPU9250_*` becomes `MPU6500_*`.
- The AK8963 register section and `AK8963_WHO_AM_I_RESULT` are removed.
- `MPU6500_WHO_AM_I_RESULT` is set to `0x70`.

## 5. Examples

- Each example uses `MPU6500-DMP.h` and `MPU6500_DMP`.
- Magnetometer reads and prints (`UPDATE_COMPASS`, `mx/my/mz`, `calcMag`, heading) are removed.
- Comments and serial output text refer to the MPU-6500.

## 6. Metadata and documentation

- `library.properties`:
  - `name=MPU6500-DMP`
  - `version=1.0.0`
  - `sentence`/`paragraph` describe a 6-DOF IMU (accel ±2/4/8/16 g, gyro ±250/500/1000/2000 °/s, I²C up to 400 kHz), forked from SparkFun
  - `url`/`repository=https://github.com/xreef/MPU-6500-DMP_Library`
  - `includes=MPU6500-DMP.h`
- `keywords.txt`: `MPU6500_DMP` class and methods; magnetometer keywords removed.
- `README.md`:
  - rewritten for the MPU-6500, with no magnetometer sections and a note on yaw drift
  - 9250 tutorial links kept only as "see also"
  - the Arduino Library Registry link is removed until the library is registered
- `.idea/` and `.project`: untracked IDE files; they are not committed and not edited.

## 7. Verification

- `grep -rniE "9250|AK89|AK8963|compass|magnet"` over `src/`, `examples/`, `keywords.txt`,
  `library.properties` and `README.md` finds nothing, except:
  - vendor comments in `inv_mpu*.c/.h` listing supported chip variants
  - preprocessor-excluded vendor blocks
  - README "see also" links
- Every example compiles with `arduino-cli` for at least one board (for example `arduino:avr:uno`
  and/or `esp32:esp32:esp32`, whichever cores are installed). If `arduino-cli` is not available,
  the user is told the examples were not compiled.

## Out of scope

- External magnetometer support.
- SPI support.
- Behavioral changes to the DMP or the vendor driver beyond what removing the compass requires.
