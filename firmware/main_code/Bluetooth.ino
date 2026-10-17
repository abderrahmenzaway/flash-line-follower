// ============================================================
//  Bluetooth.ino - remote control over an HC-05 / HC-06 module
// ============================================================
//
//  The module is on SoftwareSerial "bluetooth" (pins BT_RX / BT_TX,
//  see main_code.ino). Set USE_BLUETOOTH to 1 to use it in loop().
//
//  Commands:
//    'F' -> forward     'B' -> backward
//    'R' -> right       'L' -> left
//    'D' -> dance (turn right for 300 ms)
//    'S' -> stop

#define BT_SPEED      100  // Motor speed for Bluetooth moves (0..255)
#define DANCE_TIME_MS 300

void voiceCommande(void)
{
  if (!bluetooth.available()) return;

  char cmd = bluetooth.read();

  switch (cmd) {
    case 'F':  // Forward
      Drive(BT_SPEED, BT_SPEED);
      break;
    case 'B':  // Backward
      Drive(-BT_SPEED, -BT_SPEED);
      break;
    case 'R':  // Turn right
      Drive(BT_SPEED, -BT_SPEED);
      break;
    case 'L':  // Turn left
      Drive(-BT_SPEED, BT_SPEED);
      break;
    case 'D':  // Dance: turn right for 300 ms
      Drive(BT_SPEED, -BT_SPEED);
      delay(DANCE_TIME_MS);
      Stop();
      break;
    case 'S':  // Stop
      Stop();
      break;
    default:   // Ignore anything else (e.g. '\r', '\n')
      break;
  }
}
