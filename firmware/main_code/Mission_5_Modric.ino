// ============================================================
//  Mission 5 - Modric
// ============================================================

// True if at least one sensor reads above "threshold"
static bool anySensorAbove(uint16_t threshold)
{
  for (uint8_t i = 0; i < SENSORCOUNT; i++) {
    if (SensorValues[i] > threshold) return true;
  }
  return false;
}

void modric()
{
  RGB(1, 0, 0);
  while (SensorValues[10] > 500) {
    qtr.readLineBlack(SensorValues);
    Drive(100, 80);
  }
  while (SensorValues[6] < 500) {
    qtr.readLineBlack(SensorValues);
    Drive(80, -100);
  }

  mediumspeed_config();
  for (int i = 0; i < 40; i++) PidLine();

  // Full speed until the line ends (all sensors white)
  maxspeed_config();
  RGB(0, 0, 0);
  while (anySensorAbove(300)) PidLine();

  RGB(1, 0, 0);
  Drive(180, 180);
  delay(120);
  RGB(0, 0, 0);

  mediumspeed_config();
  for (int i = 0; i < 100; i++) PidLine();
  while (SensorValues[0] < 400) PidLine();

  while (SensorValues[0] > 200) {
    qtr.readLineBlack(SensorValues);
    Drive(80, 100);
  }
  Drive(-90, 90);
  delay(180);

  RGB(0, 1, 0);
}
