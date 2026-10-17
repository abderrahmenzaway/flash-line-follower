// ============================================================
//  Mission 6 - France
// ============================================================

void missionFrance()
{
  setMotorSpeeds(100, 100);
  delay(100);

  // White line section
  while ((SensorValues[1] < 300) || (SensorValues[11] < 300)) followLine(true, 'W');
  while ((SensorValues[1] > 200) || (SensorValues[11] > 200)) followLine(true, 'W');

  // Back to black line
  while ((SensorValues[1] < 400) || (SensorValues[12] < 400)) followLine();

  setMotorSpeeds(100, 100);
  delay(120);
  setLed(0, 0, 0);
}
