# MPU-6500 DMP Library

**Author:** Renzo Mischianti \
**Website:** [**mischianti.org**](https://mischianti.org/)

Arduino library for the TDK InvenSense **MPU-6500**, a 6-DOF IMU (3-axis gyroscope + 3-axis accelerometer), with support for the chip's **Digital Motion Processor (DMP)**.

> [!NOTE]
> This library is derived from my [MPU-9250 DMP Library](https://github.com/xreef/MPU-9250-DMP_Library), which in turn is a fork of the **SparkFun MPU-9250 DMP Arduino Library**.
> The MPU-6500 is the same accel/gyro die used inside the MPU-9250, **without the AK8963 magnetometer**, so all magnetometer functions have been removed.

> [!TIP]
> Many modules sold as "MPU-9250" actually mount an MPU-6500. If `WHO_AM_I` (register `0x75`) returns **`0x70`**, you have an MPU-6500 and this is the right library. `0x71` means a genuine MPU-9250: use the [MPU-9250 DMP Library](https://github.com/xreef/MPU-9250-DMP_Library) instead.

---

## Table of Contents

- [Overview](#overview)
- [Repository Contents](#repository-contents)
- [Getting Started](#getting-started)
  - [Setting Up the MPU-6500](#setting-up-the-mpu-6500)
  - [Configuring Sensor Settings](#configuring-sensor-settings)
  - [Reading Sensor Data Without FIFO](#reading-sensor-data-without-fifo)
  - [Using the DMP for Orientation Data](#using-the-dmp-for-orientation-data)
- [Differences from the MPU-9250 library](#differences-from-the-mpu-9250-library)
- [See also on mischianti.org](#see-also-on-mischiantiorg)
- [Changelog](#changelog)

---

## Overview

Along with configuring and reading from the accelerometer and gyroscope, this library supports the chip's DMP features:

- Quaternion calculation (6-axis)
- Pedometer
- Gyroscope calibration
- Tap detection
- Orientation detection
- Wake-on-motion (low-power accelerometer interrupt)

## Repository Contents

| Path                 | Description                                                                                                  |
|----------------------|--------------------------------------------------------------------------------------------------------------|
| `/examples`          | Example sketches for the library (`.ino`). Run these from the Arduino IDE.                                   |
| `/src`               | Source files for the library (`.cpp`, `.h`).                                                                 |
| `/src/invesense`     | InvenSense Embedded MotionDriver 6.12 driver and DMP image, plus the Arduino I²C/clock/log adapters.         |
| `keywords.txt`       | Keywords from this library that will be highlighted in the Arduino IDE.                                      |
| `library.properties` | General library properties for the Arduino package manager.                                                  |

---

## Getting Started

### Setting Up the MPU-6500

```cpp
#include <MPU6500-DMP.h>

MPU6500_DMP imu;

void setup() {
  Serial.begin(115200);

  // Initialize the MPU-6500 and check if it's connected properly
  if (imu.begin() != INV_SUCCESS) {
    while (1) {
      Serial.println("Unable to communicate with MPU-6500");
      delay(5000);
    }
  }

  // Enable gyroscope and accelerometer
  imu.setSensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
}
```

### Configuring Sensor Settings

```cpp
// Set gyroscope full-scale range to ±2000 degrees per second (dps)
imu.setGyroFSR(2000);

// Set accelerometer full-scale range to ±2g
imu.setAccelFSR(2);

// Set digital low-pass filter to 5 Hz for smooth data output
imu.setLPF(5);

// Set the sample rate for accelerometer and gyroscope to 10 Hz
imu.setSampleRate(10);
```

| Setting                | Method            | Example value |
|------------------------|-------------------|---------------|
| Gyroscope range        | `setGyroFSR()`    | `2000` dps    |
| Accelerometer range    | `setAccelFSR()`   | `2` g         |
| Low-pass filter        | `setLPF()`        | `5` Hz        |
| Accel/Gyro sample rate | `setSampleRate()` | `10` Hz       |

### Reading Sensor Data Without FIFO

```cpp
void loop() {
  if (imu.dataReady()) {
    // Update accelerometer and gyroscope (the default)
    imu.update(UPDATE_ACCEL | UPDATE_GYRO);

    Serial.println("Accel X: " + String(imu.calcAccel(imu.ax)) + " g");
    Serial.println("Gyro X: "  + String(imu.calcGyro(imu.gx))  + " dps");
  }
}
```

### Using the DMP for Orientation Data

```cpp
void setup() {
  // ... sensor initialization (see above) ...

  // Initialize the DMP to output 6-axis quaternion data at 10 Hz
  imu.dmpBegin(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_GYRO_CAL, 10);
}

void loop() {
  if (imu.fifoAvailable()) {
    if (imu.dmpUpdateFifo() == INV_SUCCESS) {
      imu.computeEulerAngles();

      Serial.print("Roll: ");  Serial.println(imu.roll);
      Serial.print("Pitch: "); Serial.println(imu.pitch);
      Serial.print("Yaw: ");   Serial.println(imu.yaw);
    }
  }
}
```

> [!IMPORTANT]
> Without a magnetometer the DMP computes a **6-axis** quaternion. Roll and pitch are referenced to gravity and stay stable; **yaw is relative to the start-up heading and drifts slowly over time**. If you need an absolute heading, add an external magnetometer or use an MPU-9250.

---

## Differences from the MPU-9250 library

| MPU-9250 DMP Library                              | MPU-6500 DMP Library                    |
|---------------------------------------------------|-----------------------------------------|
| `#include <MPU9250-DMP.h>`                        | `#include <MPU6500-DMP.h>`              |
| `MPU9250_DMP imu;`                                | `MPU6500_DMP imu;`                      |
| `mx`, `my`, `mz`, `heading`                       | removed                                 |
| `updateCompass()`, `calcMag()`                    | removed                                 |
| `getMagFSR()`, `getMagSens()`                     | removed                                 |
| `setCompassSampleRate()`, `getCompassSampleRate()`| removed                                 |
| `computeCompassHeading()`                         | removed                                 |
| `UPDATE_COMPASS`, `INV_XYZ_COMPASS` in `setSensors`| not used                               |
| `update()` default: accel + gyro + compass        | `update()` default: accel + gyro        |
| `WHO_AM_I` = `0x71`                               | `WHO_AM_I` = `0x70`                     |

---

## See also on mischianti.org

The MPU-9250 articles apply to the MPU-6500 for everything except the magnetometer:

- [TDK InvenSense MPU-9250 module: pinout, datasheet, schema and specs](https://mischianti.org/tdk-invensense-mpu-9250-module-high-resolution-pinout-datasheet-schema-and-specs/): includes a `WHO_AM_I` test sketch to identify MPU-6500 vs MPU-9250
- [MPU9250 with ESP32 and Arduino: Accelerometer, Magnetometer, and Gyroscope via I2C and SPI](https://mischianti.org/mpu9250-with-esp32-and-arduino-accelerometer-magnetometer-and-gyroscope-via-i2c-and-spi/)
- [MPU9250 with ESP32 and Arduino: interrupt and low power mode](https://mischianti.org/mpu9250-with-esp32-and-arduino-interrupt-and-low-power-mode/)
- [3D model viewer: visualize Quaternions and Euler Angles from Serial Data in Real-Time](https://mischianti.org/3d-model-viewer-visualize-quaternions-and-euler-angles-from-serial-data-in-real-time/)

---

## Changelog

| Date       | Version | Notes                                                          |
|------------|---------|----------------------------------------------------------------|
| 2026-09-29 | 1.0.0   | First release: MPU-6500 port of the MPU-9250 DMP Library       |

---

<p align="center">
  Made by <b>Renzo Mischianti</b> · More tutorials, libraries and projects at <a href="https://mischianti.org/"><b>mischianti.org</b></a>
</p>
