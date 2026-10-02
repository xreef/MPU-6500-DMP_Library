/************************************************************
  MPU6500_Wake_On_Motion
  Wake-on-Motion interrupt sketch for MPU-6500 DMP Arduino Library

  This example demonstrates how to use the Wake-on-Motion (WOM) mode
  to wake the MPU6500 only when significant motion is detected.
  This approach saves power by putting the device into low-power mode
  until motion triggers an interrupt.

  Renzo Mischianti <info@mischianti.org>
  www.mischianti.org
*************************************************************/

// Select the correct Serial port based on the platform.
#if defined(SAMD)
  #define SerialPort SerialUSB
#else
  #define SerialPort Serial
#endif

#include <MPU6500-DMP.h>  // Include the MPU6500 DMP library

// Define the GPIO pin used for the Wake-on-Motion interrupt.
#define INTERRUPT_PIN 10

// Forward declaration of the Interrupt Service Routine (ISR)
void imuISR(void);

// Global flag to track the wake-on-motion event.
volatile bool imuWoke = false;

// Create an instance of the MPU6500_DMP class.
MPU6500_DMP imu;

// Function prototype to read and display sensor data.
void readSensorData();

void setup() {
  // Initialize the interrupt pin with an internal pull-up resistor.
  // The sensor will pull this pin LOW when motion is detected.
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);

  // Begin serial communication at 115200 baud.
  SerialPort.begin(115200);
  delay(2000); // Wait a bit for the serial monitor to open
  SerialPort.println("Avvio del programma...");
  while (!SerialPort) {
    delay(100);
  }
  SerialPort.println("MPU6500_Wake_On_Motion Example");

  // Initialize the MPU6500 sensor.
  if (imu.begin() != INV_SUCCESS) {
    SerialPort.println("Unable to communicate with MPU-6500");
    SerialPort.println("Check connections, and try again.");
    SerialPort.println();
    while (1);  // Halt the program if initialization fails.
  }

  SerialPort.println("MPU-6500 inizializzato con successo!");

  // Configure the sensor to use only the accelerometer to save power.
  // This powers down the gyroscope.
  imu.setSensors(INV_XYZ_ACCEL);

  // Configure the interrupt pin to be active-low.
  imu.setIntLevel(INT_ACTIVE_LOW);

  // Disable the auxiliary I2C master interface to prevent interference.
  mpu_set_bypass(0);

  // Enable Wake-on-Motion (WOM) low-power mode.
  // Parameters:
  //   Threshold: 40 mg (milli-g) - motion sensitivity threshold.
  //   Duration: 0 (no duration filtering).
  //   Frequency: 2 (accelerometer data rate of ~15.63 Hz).
  if (mpu_lp_motion_interrupt(40, 0, 2) != INV_SUCCESS) {
    SerialPort.println("IMU setup failed. Please check the installed IMU IC.");
    while (1);  // Halt if WOM setup fails.
  }

  // Attach the interrupt service routine (ISR) to the interrupt pin.
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), imuISR, FALLING);

  // Initialize the wake-on-motion flag.
  imuWoke = false;

  SerialPort.println("MPU6500 configured for Wake-on-Motion.");
}

void loop()
{
  // If a wake-on-motion event is detected, process the sensor data.
  if (imuWoke) {
    SerialPort.println("Motion detected! Reading sensor data...");

    // Read and display sensor data.
    readSensorData();

    // Reset the wake-on-motion flag.
    imuWoke = false;
  }

  // Additional logic can be added here if needed.
  delay(10); // Prevents I2C spam on fast microcontrollers like ESP32
}

// Interrupt Service Routine (ISR) for Wake-on-Motion.
// This function is called when a falling edge is detected on the interrupt pin.
void imuISR() {
  imuWoke = true;  // Set the flag to indicate motion detection.
}

// Read sensor data if available and display it in a formatted table.
void readSensorData() {
  // Check if new sensor data is available.
  if (imu.dataReady()) {
    // Update the sensor data (accelerometer only).
    // Note: Gyroscope is disabled to save power during WOM.
    // If you need gyro data, you must enable it in setup() with:
    // imu.setSensors(INV_XYZ_ACCEL | INV_XYZ_GYRO);
    // Be aware this will significantly increase power consumption.
    imu.update(UPDATE_ACCEL);

    // Calculate sensor readings.
    float ax = imu.calcAccel(imu.ax);
    float ay = imu.calcAccel(imu.ay);
    float az = imu.calcAccel(imu.az);

    // Print sensor data in a neat tabular format.
    SerialPort.println("------------------------------------------------");
    SerialPort.println("       Sensor Data (New Motion Detected)        ");
    SerialPort.println("------------------------------------------------");
    SerialPort.print("Accel (g):   X = "); SerialPort.print(ax, 2);
    SerialPort.print(" | Y = "); SerialPort.print(ay, 2);
    SerialPort.print(" | Z = "); SerialPort.println(az, 2);
    SerialPort.println("------------------------------------------------\n");
  }
}
