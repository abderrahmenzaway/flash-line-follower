// ============================================================
//  Encoder.ino - wheel encoders and encoder-based moves
// ============================================================

volatile long count1 = 0, count2 = 0;  // Encoder pulse counts
long targetPulses = 0;

// Encoder interrupts
void readEncoder1() { count1++; }
void readEncoder2() { count2++; }

// Trajectory correction PID
double Kp_corr = 1.5, Ki_corr = 0.004, Kd_corr = 0;
double integral_corr = 0, previousError_corr = 0;

double computeCorrection(double error)
{
  integral_corr += error;
  double derivative = error - previousError_corr;
  double correction = (Kp_corr * error) + (Ki_corr * integral_corr) + (Kd_corr * derivative);
  previousError_corr = error;
  return correction;
}

// Drive straight for "rotations" wheel turns, keeping both wheels in sync
void moveForward(int rotations, int baseSpeed)
{
  targetPulses = rotations * 112;  // Turns -> pulses
  Drive(120, 120);
  delay(50);
  count1 = 0;
  count2 = 0;

  while (count1 < targetPulses || count2 < targetPulses) {
    double trajectoryError = count1 - count2;  // Pulse difference between wheels
    double correction = computeCorrection(trajectoryError);
    int motor1Speed = constrain(baseSpeed - correction, 0, baseSpeed + 20);
    int motor2Speed = constrain(baseSpeed + correction, 0, baseSpeed + 20);
    Drive(motor1Speed, motor2Speed);
  }
  Stop();
  Serial.println(F("Mouvement termine !"));
}

// Move one wheel / spin for "rotations" wheel turns
// motor: 'g' = left wheel only, 'l' = spin (left fwd, right back),
//        'd' = right wheel only, other = spin (right fwd, left back)
void moveForward(char motor, float rotations, int baseSpeed)
{
  int targetPulses = rotations * 200;  // Turns -> pulses
  count1 = 0;
  count2 = 0;

  if (motor == 'g') {
    Drive(baseSpeed, 0);
    while (count1 < targetPulses);
  } else if (motor == 'l') {
    Drive(baseSpeed, -baseSpeed);
    while (count1 < targetPulses);
  } else if (motor == 'd') {
    Drive(0, baseSpeed);
    while (count2 < targetPulses);
  } else {
    Drive(-baseSpeed, baseSpeed);
    while (count2 < targetPulses);
  }

  Stop();
  Serial.println(F("Mouvement termine !"));
}
