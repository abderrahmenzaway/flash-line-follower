// ============================================================
//  Line_Follower.ino - QTR calibration, speed presets, PID line follow
// ============================================================

// ========================== SPEED PRESETS ==========================
void strate_config(void)
{
  Kp = 0.023;
  Kd = 0.15;
  MAXSPEEDR  = 240;
  MAXSPEEDL  = 240;
  BASESPEEDR = MAXSPEEDR - 20;
  BASESPEEDL = MAXSPEEDL - 20;
}

void strate_configmin(void)
{
  Kp = 0.015;
  Kd = 0.12;
  MAXSPEEDR  = 130;
  MAXSPEEDL  = 130;
  BASESPEEDR = MAXSPEEDR - 20;
  BASESPEEDL = MAXSPEEDL - 20;
}

void maxspeed_config(void)
{
  Kp = 0.04;
  Kd = 0.25;
  MAXSPEEDR  = 230;
  MAXSPEEDL  = 230;
  BASESPEEDR = MAXSPEEDR - 20;
  BASESPEEDL = MAXSPEEDL - 20;
}

void mediumspeed_config(void)
{
  Kp = 0.038;
  Kd = 0.2;
  MAXSPEEDR  = 140;
  MAXSPEEDL  = 140;
  BASESPEEDR = MAXSPEEDR - 10;
  BASESPEEDL = MAXSPEEDL - 10;
}

void minspeed_config(void)
{
  Kp = 0.027;
  Kd = 0.15;
  MAXSPEEDR  = 130;
  MAXSPEEDL  = 130;
  BASESPEEDR = MAXSPEEDR - 20;
  BASESPEEDL = MAXSPEEDL - 20;
}

void mediumspeed_config_pro(void)
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
void PidLine(bool ret, char mode)
{
  readLine(mode);

  error = (((SENSORCOUNT - 1) * 1000) / 2) - position;
  P = error;
  D = error - lastError;

  motorspeed  = (P * Kp) + (I * Ki) + (D * Kd);
  motorspeedr = BASESPEEDR + motorspeed;
  motorspeedl = BASESPEEDL - motorspeed;
  limitSpeeds(ret);

  Drive(motorspeedl, motorspeedr);
  lastError = error;
}

// Same as PidLine (with integral), but goes straight when all sensors see white
void PidLineWhite(bool ret, char mode)
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

  if (All('W')) {
    Drive(140, 140);
  } else {
    Drive(motorspeedl, motorspeedr);
  }
  lastError = error;
}

// Forward-only line follow; when the line is lost, keeps turning to one side
void PidLine_LastError(char mode)
{
  readLine(mode);

  if (All('W')) {
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

  Drive(motorspeedl, motorspeedr);
  lastError = error;
}

// =========================== CALIBRATION ===========================
void Calibration(void)
{
  RGB(0, 0, 1);  // Blue LED: calibration running
  for (uint16_t i = 0; i < 100; i++) {
    qtr.calibrate();
  }
  RGB(0, 0, 0);
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
void ManualCalibration(void)
{
  qtr.calibrate();  // Allocates the calibration arrays
  for (uint8_t i = 0; i < SENSORCOUNT; i++) {
    qtr.calibrationOn.minimum[i] = manualMin[i];
    qtr.calibrationOn.maximum[i] = manualMax[i];
  }
}
