// ============================================================
//  Mission 6 - France
// ============================================================

void france()
{
  Drive(100, 100);
  delay(100);

  // White line section
  while ((SensorValues[1] < 300) || (SensorValues[11] < 300)) PidLine(true, 'W');
  while ((SensorValues[1] > 200) || (SensorValues[11] > 200)) PidLine(true, 'W');

  // Back to black line
  while ((SensorValues[1] < 400) || (SensorValues[12] < 400)) PidLine();

  Drive(100, 100);
  delay(120);
  RGB(0, 0, 0);
}
