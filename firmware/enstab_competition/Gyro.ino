// ============================================================
//  Gyro.ino - MPU6050 initialisation and angle reading
//  Library: MPU6050_light (rfetick), install it from the Library Manager
// ============================================================

float angle;

// ========================= INITIALISATION ========================
// Keep the robot still and flat while this runs (about 1 s).
void initGyro(void)
{
  Wire.begin();
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);  // 0 = OK
  if (status != 0) {
    Serial.println(F("MPU6050 not found, check the wiring"));
    return;
  }

  Serial.println(F("Calculating offsets, do not move the robot..."));
  delay(1000);
  mpu.calcOffsets();  // Gyro and accelerometer offsets
  Serial.println(F("Gyro ready"));
}

// ========================== ANGLE READING ========================
// Returns the current heading (yaw) in degrees, wrapped to -180 .. 180.
// MPU6050_light integrates the gyro on each update(), so call this often
// (every loop iteration while moving).
float getAngle(void)
{
  mpu.update();
  float a = fmod(mpu.getAngleZ() + 180.0, 360.0);
  if (a < 0) a += 360.0;
  return a - 180.0;
}
