// ============================================================
//  Utils.ino - motors, RGB LED, buzzer, sensors, OLED display
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

void Stop(void)
{
  MotorR.brake();
  MotorL.brake();
}

// ============================ RGB LED ==============================
// RGB(1,0,0) red   | RGB(0,1,0) green   | RGB(0,0,1) blue
// RGB(0,1,1) cyan  | RGB(1,0,1) magenta | RGB(1,1,0) yellow | RGB(1,1,1) white
void RGB(uint8_t red_value, uint8_t green_value, uint8_t blue_value)
{
  digitalWrite(LED_R, red_value);
  digitalWrite(LED_G, green_value);
  digitalWrite(LED_B, blue_value);
}

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

// Shows gyro angle + sensor bars
void Afficher_Tout(void)
{
  display.setCursor(0, 5);
  display.print("Angle: ");
  display.setTextSize(2);
  display.setCursor(40, 5);
  display.print(getAngle());
  display.setTextSize(1);
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

void Affichage_Text(String message)
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(20, 30);
  display.print(message);
  display.display();
}
