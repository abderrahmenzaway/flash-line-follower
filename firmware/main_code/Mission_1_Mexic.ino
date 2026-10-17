// ============================================================
//  Mission 1 - Mexic
// ============================================================

void mexic()
{
  first_step();
  strate_config();

  // Before the checkerboard
  while (SensorValues[0] < 500) PidLine();
  while (SensorValues[0] > 500) PidLine();
  while (SensorValues[0] < 500) PidLine();
  while (SensorValues[0] > 500) PidLine();
  for (int i = 0; i < 10; i++) PidLine();

  strate_configmin();
  while ((SensorValues[0] < 500) || (SensorValues[1] < 500) || (SensorValues[2] < 500)) {
    PidLine();
  }
}
