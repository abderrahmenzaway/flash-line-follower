// ============================================================
//  Utils.ino - motors, RGB LED, buzzer, sensors, OLED display,
//              line sensor helpers
// ============================================================

// ============================= MOTORS ==============================
// Speeds from -255 to 255. 0 = brake.
void Drive(int16_t left, int16_t right)
{
  if (left == 0) MotorL.brake();
  else           MotorL.drive(left);

  if (right == 0) MotorR.brake();
  else            MotorR.drive(right);
}

// Brake both motors, then wait "time" ms
void Stop(int time)
{
  MotorR.brake();
  MotorL.brake();
  delay(time);
}

// Turn using the encoders.
// rot:  'r' = right, 'l' = left
// meth: "adi" = spin in place, anything else = pivot on one wheel
void turn(char rot, String meth)
{
  if (meth == "adi") {
    if (rot == 'r')      moveForward('l', 0.8, 180);
    else if (rot == 'l') moveForward('r', 0.8, 180);
  } else {
    if (rot == 'r')      moveForward('g', 1.1, 180);
    else if (rot == 'l') moveForward('d', 1.1, 180);
  }
}

// Drive forward until both outer sensors see white
void first_step()
{
  while (1) {
    Drive(100, 100);
    qtr.readLineBlack(SensorValues);
    if (sens(0, 'W') && sens(SENSORCOUNT - 1, 'W')) {
      break;
    }
  }
  Stop();
}

// ============================ RGB LED ==============================
// RGB(1,0,0) red   | RGB(0,1,0) green   | RGB(0,0,1) blue
// RGB(0,1,1) cyan  | RGB(1,0,1) magenta | RGB(1,1,0) yellow | RGB(1,1,1) white
void RGB(uint8_t red_value, uint8_t green_value, uint8_t blue_value)
{
  digitalWrite(RED, red_value);
  digitalWrite(GREEN, green_value);
  digitalWrite(BLUE, blue_value);
}

// ============================= BUZZER ==============================
// Blink red LED + buzzer "nb" times, "timing" ms on / off
void Alerte(uint8_t nb, uint16_t timing)
{
  for (uint8_t i = 0; i < nb; i++) {
    RGB(1, 0, 0);
    digitalWrite(BUZZER, HIGH);
    delay(timing);

    RGB(0, 0, 0);
    digitalWrite(BUZZER, LOW);
    delay(timing);
  }
}

// Short buzzer beeps: "n" beeps of "t" ms
void Beep(int n, float t)
{
  for (int i = 0; i < n; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(t);
    digitalWrite(BUZZER, LOW);
    delay(30);
  }
}

// ============================ SENSORS ==============================
uint8_t Valeur_PROXL(void)
{
  return digitalRead(PROX_L);
}

uint8_t Valeur_PROXR(void)
{
  return digitalRead(PROX_R);
}

// Ultrasonic distance in cm
float Distance(void)
{
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  float duration_us = pulseIn(ECHO, HIGH);
  return 0.017 * duration_us;
}

// ======================= LINE SENSOR HELPERS =======================
// Uses the last values read into SensorValues.

// True if every sensor sees black ('B') or every sensor sees white ('W')
bool All(char m)
{
  for (uint8_t i = 0; i < SENSORCOUNT; i++) {
    if (m == 'B' && SensorValues[i] <= 500) return false;
    if (m == 'W' && SensorValues[i] >= 500) return false;
  }
  return (m == 'B' || m == 'W');
}

// True if sensor "index" sees black ('B') or white ('W')
bool sens(int index, char color)
{
  if (color == 'B') return SensorValues[index] >= 500;
  if (color == 'W') return SensorValues[index] <= 400;
  return false;
}

// Reads the sensors, then checks that all listed sensors are
// above ('>') or below ('<') the threshold
bool myfunc(int indexes[], int size, char s, int threshold)
{
  qtr.readLineBlack(SensorValues);
  for (int i = 0; i < size; i++) {
    if (s == '>' && SensorValues[indexes[i]] <= threshold) return false;
    if (s == '<' && SensorValues[indexes[i]] >= threshold) return false;
  }
  return (s == '>' || s == '<');
}

// ============================ DISPLAY ==============================
// Draws one bar per QTR sensor
static void drawSensorBars(void)
{
  uint16_t x_pos = 0;
  qtr.readLineBlack(SensorValues);
  for (uint8_t i = 0; i < SENSORCOUNT; i++) {
    uint16_t h = map(SensorValues[i], 0, 1000, 0, 25);
    display.fillRoundRect(x_pos, 30, 6, h, 2, WHITE);
    x_pos += 8;
  }
}

// Shows a message + sensor bars
void Afficher_Tout(void)
{
  display.setTextSize(1);
  display.setCursor(10, 5);
  display.print("god with you");
  drawSensorBars();
  display.display();
  display.clearDisplay();
}

// Shows sensor bars only
void Afficher_Calibrage(void)
{
  drawSensorBars();
  display.display();
  display.clearDisplay();
}

void Affichage_Text(String m)
{
  display.clearDisplay();
  display.setCursor(10, 30);
  display.setTextSize(1);
  display.print(m);
  display.display();
}

// Stop and show sensor bars until the button is pressed, then released
void click()
{
  Stop();
  while (digitalRead(BTN) == 0) {
    Afficher_Calibrage();
  }
  Affichage_Text("wanna release ?");
  while (digitalRead(BTN));
  Affichage_Text("");
}

// Shows the arrow type from the two Raspberry Pi inputs
void arr(bool a, bool b)
{
  if (a && !b) {
    Affichage_Text("Hunting Arrow");
    RGB(0, 0, 1);
  } else if (a && b) {
    Affichage_Text("Incendiary Arrow");
    RGB(1, 1, 0);
  } else if (!a && b) {
    Affichage_Text("Explosive Arrow");
    RGB(1, 0, 0);
  }
}
