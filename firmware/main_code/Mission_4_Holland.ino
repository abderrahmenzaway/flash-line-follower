// ============================================================
//  Mission 4 - Holland (Netherlands)
// ============================================================

void haland()
{
  // Towards the Netherlands
  while (SensorValues[0] > 500) {
    qtr.readLineBlack(SensorValues);
    Drive(-40, 120);
  }
  while (SensorValues[2] > 500) {
    qtr.readLineBlack(SensorValues);
    Drive(-40, 120);
  }

  mediumspeed_config_pro();
  while (SensorValues[13] < 500) PidLine();
  while (SensorValues[12] > 500) PidLine();

  while ((SensorValues[4] > 500) || (SensorValues[5] > 500) || (SensorValues[6] > 500)) {
    qtr.readLineBlack(SensorValues);
    Drive(110, -60);
  }
  while (SensorValues[9] < 500) {
    qtr.readLineBlack(SensorValues);
    Drive(110, -110);
  }
  while (SensorValues[0] < 500) PidLine();

  while (SensorValues[2] > 500) {
    qtr.readLineBlack(SensorValues);
    Drive(80, 110);
  }
  while (SensorValues[5] < 300) {
    qtr.readLineBlack(SensorValues);
    Drive(-120, 90);
  }
  while (SensorValues[1] > 300) {
    qtr.readLineBlack(SensorValues);
    Drive(-120, 120);
  }
  while (SensorValues[2] > 300) {
    qtr.readLineBlack(SensorValues);
    Drive(-120, 120);
  }
  while (SensorValues[0] < 500) PidLine();

  while (SensorValues[0] > 500) {
    qtr.readLineBlack(SensorValues);
    Drive(120, 0);
  }
  while (SensorValues[13] < 500) {
    qtr.readLineBlack(SensorValues);
    Drive(120, 90);
  }
  while (SensorValues[0] < 500) {
    qtr.readLineBlack(SensorValues);
    Drive(120, -80);
  }
  while (SensorValues[0] > 500) {
    qtr.readLineBlack(SensorValues);
    Drive(120, -80);
  }
  while (SensorValues[7] < 500) {
    qtr.readLineBlack(SensorValues);
    Drive(120, -80);
  }
  while (SensorValues[0] < 500) PidLine();

  while (SensorValues[2] > 500) {
    qtr.readLineBlack(SensorValues);
    Drive(80, 110);
  }
  while (SensorValues[5] < 200) {
    qtr.readLineBlack(SensorValues);
    Drive(-120, 90);
  }
  while (SensorValues[13] < 600 || SensorValues[1] < 600) PidLine();
}
