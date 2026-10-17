// ============================================================
//  Mission 3 - Australia
// ============================================================

// One Australia lap section (repeated twice in the mission)
static void australiaLap()
{
  while (SensorValues[6] < 500) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(100, 110);
  }
  while (SensorValues[6] > 500) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(100, 110);
  }
  while (SensorValues[6] < 500) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(-100, 80);
  }
  while (SensorValues[13] < 500) followLine();
  while (SensorValues[8] > 500) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(110, 80);
  }
  while (SensorValues[8] < 500) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(100, -100);
  }
  while (SensorValues[0] < 500) followLine();
}

void missionAustralia()
{
  // Towards Australia
  while (SensorValues[12] > 500) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(-90, 130);
  }
  while (SensorValues[3] < 500) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(-120, 70);
  }
  while (SensorValues[13] > 500) {
    qtr.readLineBlack(SensorValues);
    followLine();
  }
  minSpeedConfig();
  while (SensorValues[13] < 500) followLine();

  // Australia 1
  while (SensorValues[13] > 500) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(-60, 110);
  }
  while (SensorValues[6] < 500) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(-100, 100);
  }
  while (SensorValues[3] > 500) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(-100, 100);
  }
  while (SensorValues[13] < 500) followLine();
  while (SensorValues[8] > 500) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(110, 80);
  }
  while (SensorValues[8] < 500) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(100, -100);
  }
  while (SensorValues[0] < 500) followLine();

  // Australia 2
  australiaLap();

  // Australia 3
  setLed(0, 1, 0);
  australiaLap();
  setLed(0, 0, 0);
}
