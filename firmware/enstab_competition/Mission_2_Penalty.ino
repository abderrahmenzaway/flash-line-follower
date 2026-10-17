// ============================================================
//  Mission 2 - Penalty
// ============================================================

void missionPenalty()
{
  mediumSpeedConfig();

  // Into the 18-yard box
  while (SensorValues[13] < 500) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(-120, 110);
  }
  while (SensorValues[13] > 500) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(-120, 70);
  }
  while (SensorValues[8] < 500) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(-120, 70);
  }
  while (SensorValues[0] < 500) followLine();

  // Loop inside the 18-yard box
  while (SensorValues[13] < 500) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(0, 100);
  }
  while (SensorValues[13] > 500) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(0, 100);
  }
  while (SensorValues[5] < 500) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(0, 100);
  }
  while (SensorValues[13] > 500) followLine();
  while (SensorValues[13] < 500) {
    if (SensorValues[10] < 500) {
      qtr.readLineBlack(SensorValues);
      setMotorSpeeds(120, 0);
    }
    followLine();
  }

  // Last turn on the checkerboard
  while (SensorValues[13] > 500) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(70, 120);
  }
  while (SensorValues[9] < 500) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(0, 120);
  }

  while (SensorValues[13] < 500) followLine();
}
