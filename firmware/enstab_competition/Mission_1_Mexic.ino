// ============================================================
//  Mission 1 - Mexic
// ============================================================

void missionMexico()
{
  exitStartArea();
  straightFastConfig();

  // Before the checkerboard
  while (SensorValues[0] < 500) followLine();
  while (SensorValues[0] > 500) followLine();
  while (SensorValues[0] < 500) followLine();
  while (SensorValues[0] > 500) followLine();
  for (int i = 0; i < 10; i++) followLine();

  straightSlowConfig();
  while ((SensorValues[0] < 500) || (SensorValues[1] < 500) || (SensorValues[2] < 500)) {
    followLine();
  }
}
