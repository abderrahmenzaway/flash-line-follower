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

void missionModric()
{
  setLed(1, 0, 0);
  while (SensorValues[10] > 500) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(100, 80);
  }
  while (SensorValues[6] < 500) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(80, -100);
  }

  mediumSpeedConfig();
  for (int i = 0; i < 40; i++) followLine();

  // Full speed until the line ends (all sensors white)
  maxSpeedConfig();
  setLed(0, 0, 0);
  while (anySensorAbove(300)) followLine();

  setLed(1, 0, 0);
  setMotorSpeeds(180, 180);
  delay(120);
  setLed(0, 0, 0);

  mediumSpeedConfig();
  for (int i = 0; i < 100; i++) followLine();
  while (SensorValues[0] < 400) followLine();

  while (SensorValues[0] > 200) {
    qtr.readLineBlack(SensorValues);
    setMotorSpeeds(80, 100);
  }
  setMotorSpeeds(-90, 90);
  delay(180);

  setLed(0, 1, 0);
}
