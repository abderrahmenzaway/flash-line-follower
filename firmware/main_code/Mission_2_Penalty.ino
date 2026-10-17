// ============================================================
//  Mission 2 - Penalty
// ============================================================

void penaltie()
{
  mediumspeed_config();

  // Into the 18-yard box
  while (SensorValues[13] < 500) {
    qtr.readLineBlack(SensorValues);
    Drive(-120, 110);
  }
  while (SensorValues[13] > 500) {
    qtr.readLineBlack(SensorValues);
    Drive(-120, 70);
  }
  while (SensorValues[8] < 500) {
    qtr.readLineBlack(SensorValues);
    Drive(-120, 70);
  }
  while (SensorValues[0] < 500) PidLine();

  // Loop inside the 18-yard box
  while (SensorValues[13] < 500) {
    qtr.readLineBlack(SensorValues);
    Drive(0, 100);
  }
  while (SensorValues[13] > 500) {
    qtr.readLineBlack(SensorValues);
    Drive(0, 100);
  }
  while (SensorValues[5] < 500) {
    qtr.readLineBlack(SensorValues);
    Drive(0, 100);
  }
  while (SensorValues[13] > 500) PidLine();
  while (SensorValues[13] < 500) {
    if (SensorValues[10] < 500) {
      qtr.readLineBlack(SensorValues);
      Drive(120, 0);
    }
    PidLine();
  }

  // Last turn on the checkerboard
  while (SensorValues[13] > 500) {
    qtr.readLineBlack(SensorValues);
    Drive(70, 120);
  }
  while (SensorValues[9] < 500) {
    qtr.readLineBlack(SensorValues);
    Drive(0, 120);
  }

  while (SensorValues[13] < 500) PidLine();
}
