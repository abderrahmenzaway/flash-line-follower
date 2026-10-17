// ============================================================
//  Mission 3 - Australia
// ============================================================

// One Australia lap section (repeated twice in the mission)
static void australiaLoop()
{
  while (SensorValues[6] < 500) {
    qtr.readLineBlack(SensorValues);
    Drive(100, 110);
  }
  while (SensorValues[6] > 500) {
    qtr.readLineBlack(SensorValues);
    Drive(100, 110);
  }
  while (SensorValues[6] < 500) {
    qtr.readLineBlack(SensorValues);
    Drive(-100, 80);
  }
  while (SensorValues[13] < 500) PidLine();
  while (SensorValues[8] > 500) {
    qtr.readLineBlack(SensorValues);
    Drive(110, 80);
  }
  while (SensorValues[8] < 500) {
    qtr.readLineBlack(SensorValues);
    Drive(100, -100);
  }
  while (SensorValues[0] < 500) PidLine();
}

void australia()
{
  // Towards Australia
  while (SensorValues[12] > 500) {
    qtr.readLineBlack(SensorValues);
    Drive(-90, 130);
  }
  while (SensorValues[3] < 500) {
    qtr.readLineBlack(SensorValues);
    Drive(-120, 70);
  }
  while (SensorValues[13] > 500) {
    qtr.readLineBlack(SensorValues);
    PidLine();
  }
  minspeed_config();
  while (SensorValues[13] < 500) PidLine();

  // Australia 1
  while (SensorValues[13] > 500) {
    qtr.readLineBlack(SensorValues);
    Drive(-60, 110);
  }
  while (SensorValues[6] < 500) {
    qtr.readLineBlack(SensorValues);
    Drive(-100, 100);
  }
  while (SensorValues[3] > 500) {
    qtr.readLineBlack(SensorValues);
    Drive(-100, 100);
  }
  while (SensorValues[13] < 500) PidLine();
  while (SensorValues[8] > 500) {
    qtr.readLineBlack(SensorValues);
    Drive(110, 80);
  }
  while (SensorValues[8] < 500) {
    qtr.readLineBlack(SensorValues);
    Drive(100, -100);
  }
  while (SensorValues[0] < 500) PidLine();

  // Australia 2
  australiaLoop();

  // Australia 3
  RGB(0, 1, 0);
  australiaLoop();
  RGB(0, 0, 0);
}
