// ============================================================
//  Line_Follower.ino - QTR calibration, speed presets, PID line follow
// ============================================================

// ========================== SPEED PRESETS ==========================
void straightFastConfig(void)
{
  Kp = 0.023;
  Kd = 0.15;
  MAXSPEEDR  = 240;
  MAXSPEEDL  = 240;
  BASESPEEDR = MAXSPEEDR - 20;
  BASESPEEDL = MAXSPEEDL - 20;
}

void straightSlowConfig(void)
{
  Kp = 0.015;
  Kd = 0.12;
  MAXSPEEDR  = 130;
  MAXSPEEDL  = 130;
  BASESPEEDR = MAXSPEEDR - 20;
  BASESPEEDL = MAXSPEEDL - 20;
}

void maxSpeedConfig(void)
{
  Kp = 0.04;
  Kd = 0.25;
  MAXSPEEDR  = 230;
  MAXSPEEDL  = 230;
  BASESPEEDR = MAXSPEEDR - 20;
  BASESPEEDL = MAXSPEEDL - 20;
}

void mediumSpeedConfig(void)
{
  Kp = 0.038;
  Kd = 0.2;
  MAXSPEEDR  = 140;
  MAXSPEEDL  = 140;
  BASESPEEDR = MAXSPEEDR - 10;
  BASESPEEDL = MAXSPEEDL - 10;
}

void minSpeedConfig(void)
{
  Kp = 0.027;
  Kd = 0.15;
  MAXSPEEDR  = 130;
  MAXSPEEDL  = 130;
  BASESPEEDR = MAXSPEEDR - 20;
  BASESPEEDL = MAXSPEEDL - 20;
}

void mediumSpeedTightConfig(void)
{
  Kp = 0.038;
  Kd = 0.2;
  MAXSPEEDR  = 120;
  MAXSPEEDL  = 120;
  BASESPEEDR = MAXSPEEDR - 20;
  BASESPEEDL = MAXSPEEDL - 20;
}

// ========================= PID LINE FOLLOW =========================
static void readLine(char mode)
{
  if (mode == 'B') {
    position = qtr.readLineBlack(SensorValues);
  } else if (mode == 'W') {
    position = qtr.readLineWhite(SensorValues);
  }
}

// Clamp motor speeds. ret = true allows reverse, false keeps speeds >= 0
static void limitSpeeds(bool ret)
{
  if (motorspeedl > MAXSPEEDL) motorspeedl = MAXSPEEDL;
  if (motorspeedr > MAXSPEEDR) motorspeedr = MAXSPEEDR;

  if (ret) {
    if (motorspeedl < -MAXSPEEDL) motorspeedl = -MAXSPEEDL;
    if (motorspeedr < -MAXSPEEDR) motorspeedr = -MAXSPEEDR;
  } else {
    if (motorspeedl < 0) motorspeedl = 0;
    if (motorspeedr < 0) motorspeedr = 0;
  }
}

// PD line follow (integral disabled)
// ret:  true = allow reverse, false = forward only
// mode: 'B' = black line, 'W' = white line
void followLine(bool ret, char mode)
{
  readLine(mode);

  error = (((SENSORCOUNT - 1) * 1000) / 2) - position;
  P = error;
  D = error - lastError;

  motorspeed  = (P * Kp) + (I * Ki) + (D * Kd);
  motorspeedr = BASESPEEDR + motorspeed;
  motorspeedl = BASESPEEDL - motorspeed;
  limitSpeeds(ret);

  setMotorSpeeds(motorspeedl, motorspeedr);
  lastError = error;
}

// Same as followLine (with integral), but goes straight when all sensors see white
void followLineThroughGaps(bool ret, char mode)
{
  readLine(mode);

  error = (((SENSORCOUNT - 1) * 1000) / 2) - position;
  P = error;
  I += error;
  D = error - lastError;

  motorspeed  = (P * Kp) + (I * Ki) + (D * Kd);
  motorspeedr = BASESPEEDR + motorspeed;
  motorspeedl = BASESPEEDL - motorspeed;
  limitSpeeds(ret);

  if (allSensors('W')) {
    setMotorSpeeds(140, 140);
  } else {
    setMotorSpeeds(motorspeedl, motorspeedr);
  }
  lastError = error;
}

// Forward-only line follow; when the line is lost, keeps turning to one side
void followLineRecover(char mode)
{
  readLine(mode);

  if (allSensors('W')) {
    error = ((SENSORCOUNT - 1) * 1000) / 2;
  } else {
    error = (((SENSORCOUNT - 1) * 1000) / 2) - position;
  }
  P = error;
  I += error;
  D = error - lastError;

  motorspeed  = (P * Kp) + (I * Ki) + (D * Kd);
  motorspeedr = BASESPEEDR + motorspeed;
  motorspeedl = BASESPEEDL - motorspeed;
  limitSpeeds(false);

  setMotorSpeeds(motorspeedl, motorspeedr);
  lastError = error;
}

// =========================== CALIBRATION ===========================
void calibrateSensors(void)
{
  setLed(0, 0, 1);  // Blue LED: calibration running
  for (uint16_t i = 0; i < 100; i++) {
    qtr.calibrate();
  }
  setLed(0, 0, 0);
  delay(1500);

  // Print min / max calibration values
  for (uint8_t i = 0; i < SENSORCOUNT; i++) {
    Serial.print(qtr.calibrationOn.minimum[i]);
    Serial.print(' ');
  }
  Serial.println();
  for (uint8_t i = 0; i < SENSORCOUNT; i++) {
    Serial.print(qtr.calibrationOn.maximum[i]);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println();
  delay(500);
}

// Use the saved manualMin / manualMax values instead of calibrating
void loadManualCalibration(void)
{
  qtr.calibrate();  // Allocates the calibration arrays
  for (uint8_t i = 0; i < SENSORCOUNT; i++) {
    qtr.calibrationOn.minimum[i] = manualMin[i];
    qtr.calibrationOn.maximum[i] = manualMax[i];
  }
}
