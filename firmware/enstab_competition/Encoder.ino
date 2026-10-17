// ============================================================
//  Encoder.ino - wheel encoders and encoder-based moves
// ============================================================

volatile long count1 = 0, count2 = 0;  // Encoder pulse counts
long targetPulses = 0;

// Encoder interrupts
void isrEncoder1() { count1++; }
void isrEncoder2() { count2++; }

// Trajectory correction PID
double Kp_corr = 1.5, Ki_corr = 0.004, Kd_corr = 0;
double integral_corr = 0, previousError_corr = 0;

double computeSyncCorrection(double error)
{
  integral_corr += error;
  double derivative = error - previousError_corr;
  double correction = (Kp_corr * error) + (Ki_corr * integral_corr) + (Kd_corr * derivative);
  previousError_corr = error;
  return correction;
}

// Drive straight for "rotations" wheel turns, keeping both wheels in sync
void driveStraightEncoders(int rotations, int baseSpeed)
{
  targetPulses = rotations * 112;  // Turns -> pulses
  setMotorSpeeds(120, 120);
  delay(50);
  count1 = 0;
  count2 = 0;

  while (count1 < targetPulses || count2 < targetPulses) {
    double trajectoryError = count1 - count2;  // Pulse difference between wheels
    double correction = computeSyncCorrection(trajectoryError);
    int motor1Speed = constrain(baseSpeed - correction, 0, baseSpeed + 20);
    int motor2Speed = constrain(baseSpeed + correction, 0, baseSpeed + 20);
    setMotorSpeeds(motor1Speed, motor2Speed);
  }
  stopMotors();
  Serial.println(F("Move done"));
}

// Move one wheel / spin for "rotations" wheel turns
// motor: 'g' = left wheel only, 'l' = spin (left fwd, right back),
//        'd' = right wheel only, other = spin (right fwd, left back)
void rotateWheels(char motor, float rotations, int baseSpeed)
{
  int targetPulses = rotations * 200;  // Turns -> pulses
  count1 = 0;
  count2 = 0;

  if (motor == 'g') {
    setMotorSpeeds(baseSpeed, 0);
    while (count1 < targetPulses);
  } else if (motor == 'l') {
    setMotorSpeeds(baseSpeed, -baseSpeed);
    while (count1 < targetPulses);
  } else if (motor == 'd') {
    setMotorSpeeds(0, baseSpeed);
    while (count2 < targetPulses);
  } else {
    setMotorSpeeds(-baseSpeed, baseSpeed);
    while (count2 < targetPulses);
  }

  stopMotors();
  Serial.println(F("Move done"));
}
