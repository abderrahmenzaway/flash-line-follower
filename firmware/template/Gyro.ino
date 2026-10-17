// ============================================================
//  Gyro.ino - MPU6050 (DMP) initialisation and gyro-based moves
// ============================================================

#define INTERRUPT_PIN 43

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

// Gyro PID
int16_t ma = 0;
int16_t mb = 0;
int16_t Pg;
int16_t Ig;
int16_t Dg;
int16_t lastErrorg   = 0;
int16_t errorg       = 0;
uint8_t maxspeedag   = 0;
uint8_t maxspeedbg   = 0;
int16_t basespeedag  = 0;
int16_t basespeedbg  = 0;
int motorspeedg      = 0;
int motorspeedag     = 0;
int motorspeedbg     = 0;
float Kpg = 0;
float Kdg = 0;
float Kig = 0;

// ======================= INTERRUPT ROUTINE =======================
volatile bool mpuInterrupt = false;  // True when MPU interrupt pin goes high

void dmpDataReady(void)
{
  mpuInterrupt = true;
}

// ========================= INITIALISATION ========================
void InitGyro(void)
{
  Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();
  delay(100);
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

float getYawVal(uint8_t *data)
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
  return getYawVal(fifoBuffer);
}

// ========================= GYRO MOVES ============================

// Simple bang-bang heading hold around "zero" (handles the +/-180 wrap)
void Avance_Gyro(float angle, float zero, int precision, int vav, int vitess)
{
  if (zero >= 0) {  // Positive target
    if (zero + precision < 180) {
      if (angle < zero - precision)
        Drive(vitess, 0);
      else if (angle > zero + precision)
        Drive(0, vitess);
      else
        Drive(vav, vav);
    } else {  // zero + precision >= 180
      if (((angle >= 0) && (angle > zero - precision)) || ((angle < 0) && (angle < -360 + precision + zero)))
        Drive(vav, vav);
      else if ((angle >= 0) && (angle <= zero + precision))
        Drive(vitess, 0);
      else if ((angle < 0) && (angle > -360 + zero + precision))
        Drive(0, vitess);
    }
  } else {  // Negative target
    if (zero - precision > -180) {
      if (angle < zero - precision)
        Drive(vitess, 0);
      else if (angle > zero + precision)
        Drive(0, vitess);
      else
        Drive(vav, vav);
    } else {
      if (((angle <= 0) && (angle < zero + precision)) || ((angle > 0) && (angle >= (360 - precision + zero))))
        Drive(vav, vav);
      else if ((angle > zero + precision) && (angle <= 0))
        Drive(0, vitess);
      else if ((angle < 360 + zero - precision) && (angle > 0))
        Drive(vitess, 0);
    }
  }
}

// Drive straight holding the heading "Setpoint" (PID, no angle wrap)
void Avance_Gyro_PID(float Setpoint, int16_t vitesse)
{
  maxspeedag  = 255;
  maxspeedbg  = 255;
  basespeedag = vitesse;
  basespeedbg = vitesse;
  Kpg = 9;  // Tune first
  Kdg = 5;  // Tune second
  Kig = 0;

  angle  = getAngle();
  errorg = Setpoint - angle;
  Pg = errorg;
  Ig = Ig + errorg;
  Dg = errorg - lastErrorg;

  motorspeedg  = Pg * Kpg + Ig * Kig + Dg * Kdg;
  motorspeedag = basespeedag + motorspeedg;
  motorspeedbg = basespeedbg - motorspeedg;

  if (motorspeedag > maxspeedag) motorspeedag = maxspeedag;
  if (motorspeedbg > maxspeedbg) motorspeedbg = maxspeedbg;

  Drive(motorspeedag, motorspeedbg);
  lastErrorg = errorg;
}

// Drive straight holding the heading "Setpoint" (PID, shortest-path angle wrap)
void Avance_Gyro_PID_New(float Setpoint, int16_t vitesse)
{
  maxspeedag  = 255;
  maxspeedbg  = 255;
  basespeedag = vitesse;
  basespeedbg = vitesse;
  Kpg = 4;
  Kdg = 3;
  Kig = 0;

  angle  = getAngle();
  errorg = Setpoint - angle;

  // Wrap error to [-179, 179]
  if (errorg > 179)       errorg -= 360;
  else if (errorg < -179) errorg += 360;

  Pg = errorg;
  Ig = Ig + errorg;
  Dg = errorg - lastErrorg;

  motorspeedg  = Pg * Kpg + Ig * Kig + Dg * Kdg;
  motorspeedag = basespeedag + motorspeedg;
  motorspeedbg = basespeedbg - motorspeedg;

  if (motorspeedag > maxspeedag) motorspeedag = maxspeedag;
  if (motorspeedbg > maxspeedbg) motorspeedbg = maxspeedbg;

  Drive(motorspeedag, motorspeedbg);
  lastErrorg = errorg;
}

// Turn in place until the heading reaches "Setpoint" (blocking, no angle wrap)
void Turn_Gyro_PID(float Setpoint)
{
  maxspeedag  = 100;
  maxspeedbg  = 100;
  basespeedag = 50;
  basespeedbg = 50;
  Kpg = 0.9;
  Kdg = 0.9;
  Kig = 0.0;

  bool stp = false;
  while (!stp) {
    angle  = getAngle();
    errorg = Setpoint - angle;
    Pg = errorg;
    Ig = Ig + errorg;
    Dg = errorg - lastErrorg;

    motorspeedg  = Pg * Kpg + Ig * Kig + Dg * Kdg;
    motorspeedag = basespeedag + motorspeedg;
    motorspeedbg = basespeedbg - motorspeedg;

    if (motorspeedag > maxspeedag) motorspeedag = maxspeedag;
    if (motorspeedbg > maxspeedbg) motorspeedbg = maxspeedbg;

    if (motorspeedag > motorspeedbg) {
      ma = motorspeedag;
      mb = -motorspeedag;
    } else if (motorspeedag < motorspeedbg) {
      ma = -motorspeedbg;
      mb = motorspeedbg;
    } else {
      ma = 0;
      mb = 0;
      Stop();
      stp = true;
    }

    Drive(ma, mb);
    lastErrorg = errorg;
  }
}

// Turn in place until the heading reaches "Setpoint" (blocking, shortest-path wrap)
void Turn_Gyro_PID_New(float Setpoint)
{
  maxspeedag  = 70;
  maxspeedbg  = 70;
  basespeedag = 50;
  basespeedbg = 50;
  Kpg = 0.7;
  Kdg = 0.7;
  Kig = 0.0;

  bool stp = false;
  while (!stp) {
    angle  = getAngle();
    errorg = Setpoint - angle;

    // Wrap error to [-179, 179]
    if (errorg > 179)       errorg -= 360;
    else if (errorg < -179) errorg += 360;

    Pg = errorg;
    Ig = Ig + errorg;
    Dg = errorg - lastErrorg;

    motorspeedg  = Pg * Kpg + Ig * Kig + Dg * Kdg;
    motorspeedag = basespeedag + motorspeedg;
    motorspeedbg = basespeedbg - motorspeedg;

    if (motorspeedag > maxspeedag) motorspeedag = maxspeedag;
    if (motorspeedbg > maxspeedbg) motorspeedbg = maxspeedbg;

    if (motorspeedag > motorspeedbg) {
      ma = motorspeedag;
      mb = -motorspeedag;
    } else if (motorspeedag < motorspeedbg) {
      ma = -motorspeedbg;
      mb = motorspeedbg;
    } else {
      ma = 0;
      mb = 0;
      Stop();      // Balanced: target reached
      stp = true;
    }

    Drive(ma, mb);
    lastErrorg = errorg;
  }
}

// Drive forward, correcting slightly toward heading x
void Avance_Gyro_Simple(int16_t x, int16_t vitesse)
{
  if (getAngle() > x) {
    Drive(vitesse, vitesse + 5);
  } else {
    Drive(vitesse + 5, vitesse);
  }
}

// Drive backward, correcting slightly toward heading x
void Arriere_Gyro_Simple(int16_t x, int16_t vitesse)
{
  if (getAngle() > x) {
    Drive(-vitesse - 10, -vitesse);
  } else {
    Drive(-vitesse, -vitesse - 10);
  }
}
