// ============================================================
//  Gyro.ino - MPU6050 (DMP) initialisation and angle reading
// ============================================================

#define INTERRUPT_PIN 2  // NOTE: also used by ENCODER_A1

// MPU control/status
bool dmpReady = false;   // True if DMP init succeeded
uint8_t mpuIntStatus;    // Interrupt status byte from MPU
uint8_t devStatus;       // Status after each device operation (0 = success)
uint16_t packetSize;     // Expected DMP packet size (default 42 bytes)
uint16_t fifoCount;      // Number of bytes currently in FIFO
uint8_t fifoBuffer[64];  // FIFO storage buffer

// Orientation
Quaternion q;            // [w, x, y, z] quaternion
VectorFloat gravity;     // [x, y, z]    gravity vector
float ypr[3];            // [yaw, pitch, roll]
float angle;

// ======================= INTERRUPT ROUTINE =======================
volatile bool mpuInterrupt = false;  // True when MPU interrupt pin goes high

void dmpDataReady(void)
{
  mpuInterrupt = true;
}

// ========================= INITIALISATION ========================
void initGyro(void)
{
  Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();
  pinMode(INTERRUPT_PIN, INPUT);

  Serial.println(F("Testing device connections..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

  Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  // Optional: supply your own offsets here
  // mpu.setXGyroOffset(51);
  // mpu.setYGyroOffset(8);
  // mpu.setZGyroOffset(21);
  // mpu.setXAccelOffset(1150);
  // mpu.setYAccelOffset(-50);
  // mpu.setZAccelOffset(1060);

  if (devStatus == 0) {
    // Generate offsets and calibrate
    // mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);
    Serial.println();
    mpu.PrintActiveOffsets();

    Serial.println(F("Enabling DMP..."));
    mpu.setDMPEnabled(true);

    Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
    Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
    Serial.println(F(")..."));
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    Serial.println(F("DMP ready! Waiting for first interrupt..."));
    dmpReady = true;
    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    Serial.print(F("DMP Initialization failed (code "));
    Serial.print(devStatus);
    Serial.println(F(")"));
  }
}

// Reads the latest FIFO packet, recovering from overflow
uint8_t GetCurrentFIFOPacket(uint8_t *data, uint8_t length)
{
  uint8_t Error;
  uint8_t Flag = 0;
  uint8_t overflowed = 0;

  do {
    if ((Error = (fifoCount = mpu.getFIFOCount()) % length)) {
      if ((Error = (fifoCount = mpu.getFIFOCount()) % length)) {
        mpuIntStatus = mpu.getIntStatus();
        if (mpuIntStatus & _BV(MPU6050_INTERRUPT_FIFO_OFLOW_BIT)) {
          mpu.getFIFOBytes(data, Error);   // Remove the overflow portion
          mpu.getFIFOBytes(data, length);
          overflowed = 1;
          // Bytes may have arrived after we freed up space
          if ((Error = (fifoCount = mpu.getFIFOCount()) % length)) {
            mpu.getFIFOBytes(data, Error);
            if ((fifoCount = mpu.getFIFOCount()) % length) {
              mpu.resetFIFO();
              Serial.println(F("Failed to recover overflow error"));
              return 0;
            }
          }
        }
      }
    }

    fifoCount = mpu.getFIFOCount();
    if (fifoCount >= packetSize) {
      mpu.getFIFOBytes(data, length);
      Flag = 1 + overflowed;
      fifoCount -= packetSize;
    }
  } while (fifoCount >= packetSize);

  return Flag;
}

float getYaw(uint8_t *data)
{
  mpu.dmpGetQuaternion(&q, data);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
  return ypr[0] * 180 / M_PI;
}

// Returns the current yaw angle in degrees (-180 .. 180)
float getAngle(void)
{
  GetCurrentFIFOPacket(fifoBuffer, packetSize);
  return getYaw(fifoBuffer);
}
