/************************************************************
MPU6500_DMP_Pedometer
 Pedometer example for MPU-6500 DMP Arduino Library

  Renzo Mischianti @ mischianti.org
 https://github.com/xreef/MPU-6500-DMP_Library

The MPU-6500's digital motion processor (DMP) can estimate
steps taken -- effecting a pedometer.

After uploading the code, try shaking the board up and
down at a "stepping speed."

*************************************************************/
#include <MPU6500-DMP.h>

#if defined(SAMD)
#define SerialPort SerialUSB
#else
#define SerialPort Serial
#endif

MPU6500_DMP imu;

unsigned long stepCount = 0;
unsigned long stepTime = 0;
unsigned long lastStepCount = 0;

void setup() 
{
  SerialPort.begin(115200);
  delay(2000); // Wait a bit for the serial monitor to open
  SerialPort.println("Avvio del programma...");

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

  SerialPort.println("MPU-6500 inizializzato con successo!");

  
  imu.dmpBegin(DMP_FEATURE_PEDOMETER);
  imu.dmpSetPedometerSteps(stepCount);
  imu.dmpSetPedometerTime(stepTime);
  // NOTE: The pedometer algorithm requires several consecutive, rhythmic
  // steps (typically 5-7) before it starts counting. Single isolated
  // movements are ignored to prevent false positives.
}

void loop() 
{
  stepCount = imu.dmpGetPedometerSteps();
  stepTime = imu.dmpGetPedometerTime();
  
  if (stepCount != lastStepCount)
  {
    lastStepCount = stepCount;
    SerialPort.print("Walked " + String(stepCount) + 
                     " steps");
    SerialPort.println(" (" + 
              String((float)stepTime / 1000.0) + " s)");
  }
  
  delay(100); // Evita di saturare il bus I2C leggendo continuamente
}

