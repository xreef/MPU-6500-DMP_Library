/************************************************************
 * MPU6500_WebSerial_3d
 * Quaternion and Euler angle for MPU-6500 DMP Arduino Library
 * to show movement of 3d model in realtime
 *
 *  Renzo Mischianti @ mischianti.org
 * https://github.com/xreef/MPU-6500-DMP_Library
 *
 * The data are displayed as needed for WebSerial_3DModelViewer
 * https://mischianti.org/3d-model-viewer-visualize-quaternions-and-euler-angles-from-serial-data-in-real-time/
 *
 * Note: the MPU-6500 has no magnetometer, so the yaw is
 * relative to the start-up position and drifts slowly.
 *
 *************************************************************/
#include <MPU6500-DMP.h>

#if defined(SAMD)
#define SerialPort SerialUSB
#else
#define SerialPort Serial
#endif

MPU6500_DMP imu;

void printIMUData(void);

void setup()
{
  SerialPort.begin(115200);
  delay(2000); // Wait a bit for the serial monitor to open
  SerialPort.println("Starting...");

  // Call imu.begin() to verify communication and initialize
  if (imu.begin() != INV_SUCCESS)
  {
    while (1)
    {
      SerialPort.println("Unable to communicate with MPU-6500");
      SerialPort.println("Check connections, and try again.");
      SerialPort.println();
      delay(5000);
    }
  }

  SerialPort.println("MPU-6500 initialized successfully!");

  imu.dmpBegin(DMP_FEATURE_6X_LP_QUAT | // Enable 6-axis quat
               DMP_FEATURE_GYRO_CAL, // Use gyro calibration
              10); // Set DMP FIFO rate to 10 Hz
  // DMP_FEATURE_LP_QUAT can also be used. It uses the
  // accelerometer in low-power mode to estimate quat's.
  // DMP_FEATURE_LP_QUAT and 6X_LP_QUAT are mutually exclusive
}

void loop()
{
  // Check for new data in the FIFO
  if ( imu.fifoAvailable() )
  {
    // Use dmpUpdateFifo to update the ax, gx, qw, etc. values
    if ( imu.dmpUpdateFifo() == INV_SUCCESS)
    {
      // computeEulerAngles can be used -- after updating the
      // quaternion values -- to estimate roll, pitch, and yaw
      imu.computeEulerAngles();
      printIMUData();
    }
  }
  delay(10); // Prevents I2C spam on fast microcontrollers like ESP32
}

void printIMUData(void)
{
  // After calling dmpUpdateFifo() the ax, gx, qw, etc. values
  // are all updated.
  // Quaternion values are, by default, stored in Q30 long
  // format. calcQuat turns them into a float between -1 and 1
  float qw = imu.calcQuat(imu.qw);
  float qx = imu.calcQuat(imu.qx);
  float qy = imu.calcQuat(imu.qy);
  float qz = imu.calcQuat(imu.qz);


  /* The WebSerial 3D Model Viewer expects data as heading, pitch, roll */
  SerialPort.print(F("Orientation: "));
  SerialPort.print(180 - (float)imu.yaw);
  SerialPort.print(F(", "));
  SerialPort.print(180 - (float)imu.pitch);
  SerialPort.print(F(", "));
  SerialPort.print(180 - (float)imu.roll);
  SerialPort.println(F(""));

  SerialPort.print(F("Quaternion: "));
  SerialPort.print(qw, 4);
  SerialPort.print(F(", "));
  SerialPort.print(qx, 4);
  SerialPort.print(F(", "));
  SerialPort.print(qy, 4);
  SerialPort.print(F(", "));
  SerialPort.print(qz, 4);
  SerialPort.println(F(""));

  delay(100);
}
