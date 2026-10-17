/*
 * main_code - Flash, fast line follower robot (Arduino Mega 2560 Pro)
 *
 * Sketch files (the Arduino IDE merges them in this order):
 *   main_code.ino           -> includes, pin config, globals, setup() and loop()
 *   Bluetooth.ino           -> Bluetooth remote control (voiceCommande)
 *   Encoder.ino             -> wheel encoders, encoder-based moves
 *   Gyro.ino                -> MPU6050 init, angle reading, gyro-based moves
 *   Line_Follower.ino       -> QTR calibration, speed presets, PID line following
 *   Mission_1_Mexic.ino     -> mission 1
 *   Mission_2_Penalty.ino   -> mission 2
 *   Mission_3_Australia.ino -> mission 3
 *   Mission_4_Holland.ino   -> mission 4
 *   Mission_5_Modric.ino    -> mission 5
 *   Mission_6_France.ino    -> mission 6
 *   Utils.ino               -> motors, LED, buzzer, sensors, OLED, sensor helpers
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SparkFun_TB6612.h>
#include <QTRSensors.h>
#include <MPU6050_6Axis_MotionApps612.h>
#include <SoftwareSerial.h>

// ============================== RUN MODE ==============================
// 0 = run the missions (competition)
// 1 = Bluetooth remote control (see Bluetooth.ino)
#define USE_BLUETOOTH 0

// ============================== BLUETOOTH =============================
#define BT_RX 13  // Arduino RX  <- module TX
#define BT_TX 11  // Arduino TX  -> module RX
SoftwareSerial bluetooth(BT_RX, BT_TX);

// ============================== MPU6050 ===============================
// Default I2C address is 0x68 (AD0 low). Use MPU6050 mpu(0x69) if AD0 is high.
MPU6050 mpu;

// ======================== TB6612FNG MOTOR DRIVER ======================
#define AIN1    14
#define AIN2    4
#define BIN1    12
#define BIN2    10
#define PWMA    6   // Motor A PWM
#define PWMB    8   // Motor B PWM
#define STBY    -1  // Standby pin (not used)
#define OFFSETL 1
#define OFFSETR -1

Motor MotorR = Motor(AIN1, AIN2, PWMA, OFFSETL, STBY);
Motor MotorL = Motor(BIN1, BIN2, PWMB, OFFSETR, STBY);

// ============================== ENCODERS ==============================
#define ENCODER_A1 2   // Motor 1 encoder
#define ENCODER_B1 3
#define ENCODER_A2 18  // Motor 2 encoder
#define ENCODER_B2 19

// ============================ RASPBERRY PI ============================
#define TO_RASP 5  // Output to the Raspberry Pi
#define IN_A    7  // Input from the Raspberry Pi
#define IN_B    9  // Input from the Raspberry Pi

// ============================ OLED DISPLAY ============================
#define SCREEN_WIDTH  128  // OLED width, in pixels
#define SCREEN_HEIGHT 64   // OLED height, in pixels
#define OLED_RESET    -1   // Reset pin (-1 = shared with Arduino reset)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// =============================== DEVICES ==============================
constexpr uint8_t RED    = 39;
constexpr uint8_t GREEN  = 37;
constexpr uint8_t BLUE   = 35;

constexpr uint8_t BUZZER = 41;
constexpr uint8_t BTN    = 33;

constexpr uint8_t ECHO   = 15;
constexpr uint8_t TRIG   = 17;

constexpr uint8_t PROX_L = 22;
constexpr uint8_t PROX_R = 11;
constexpr uint8_t PROX_a = 19;

// ============================= QTR SENSORS ============================
constexpr uint8_t SENSORCOUNT = 14;
#define PIN_SENSOR A0

uint16_t SensorValues[SENSORCOUNT];

const uint8_t Analog_Pins[] = {
  PIN_SENSOR
};

QTRSensors qtr;

// Manual calibration values (see ManualCalibration() in Line_Follower.ino)
uint16_t manualMin[SENSORCOUNT] = { 946, 932, 929, 927, 927, 926, 934, 938, 934, 927, 931, 931, 933, 940 };  // Black
uint16_t manualMax[SENSORCOUNT] = { 990, 987, 986, 977, 985, 982, 976, 985, 985, 977, 984, 986, 980, 980 };  // White

// ========================== LINE FOLLOWER PID =========================
uint16_t position;
int16_t P;
int16_t I;
int16_t D;
int16_t lastError   = 0;
int16_t error       = 0;
int16_t motorspeed  = 0;
int16_t motorspeedl = 0;  // Left
int16_t motorspeedr = 0;  // Right
int16_t MAXSPEEDR   = 0;  // Max speed, right motor
int16_t MAXSPEEDL   = 0;  // Max speed, left motor
int16_t BASESPEEDR  = 0;  // Base speed, right motor
int16_t BASESPEEDL  = 0;  // Base speed, left motor
float Kp = 0.0;
float Ki = 0.0;
float Kd = 0.0;

// ===================== PROTOTYPES (default arguments) =================
void Stop(int time = 0);
bool All(char m = 'B');
bool sens(int index, char color = 'B');
bool myfunc(int indexes[], int size, char s = '>', int threshold = 500);
void turn(char rot, String meth = "adi");
void PidLine(bool ret = true, char mode = 'B');
void PidLineWhite(bool ret = true, char mode = 'B');
void PidLine_LastError(char mode = 'B');

// =============================== SETUP ================================
void setup(void)
{
  Serial.begin(9600);
  bluetooth.begin(9600);

  pinMode(BUZZER, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BTN, INPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(PROX_R, INPUT);
  pinMode(PROX_L, INPUT);
  pinMode(PROX_a, INPUT);

  pinMode(TO_RASP, OUTPUT);
  pinMode(IN_A, INPUT);
  pinMode(IN_B, INPUT);

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);

  pinMode(ENCODER_A1, INPUT_PULLUP);
  pinMode(ENCODER_B1, INPUT_PULLUP);
  pinMode(ENCODER_A2, INPUT_PULLUP);
  pinMode(ENCODER_B2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A1), readEncoder1, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A2), readEncoder2, RISING);

  Beep(1, 40);
  RGB(0, 0, 0);

  qtr.setTypeAnalog();
  qtr.setSensorPins(Analog_Pins, SENSORCOUNT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (1);  // Stop here
  }
  display.setTextColor(SSD1306_WHITE);
  delay(500);
  display.clearDisplay();

  Serial.println(F("restart"));
  start();
  mediumspeed_config();
}

// ================================ LOOP ================================
void loop(void)
{
#if USE_BLUETOOTH
  voiceCommande();
#else
  mexic();
  penaltie();
  australia();
  haland();
  modric();
  france();
  Stop();
  while (digitalRead(BTN) == 0);  // Wait for button to run again
  qtr.readLineBlack(SensorValues);
#endif
}

// ========================== STARTUP SEQUENCE ==========================
static void start(void)
{
  // QTR sensor calibration
  Affichage_Text("Calib QTR");
  while (digitalRead(BTN) == 0);  // Wait for press
  Affichage_Text("wait");
  RGB(0, 0, 0);
  while (digitalRead(BTN));       // Wait for release
  Affichage_Text("Calibrating QTR ...");
  Calibration();                  // or ManualCalibration();
  Affichage_Text("Calib QTR Terminee");

  // Gyro calibration (disabled)
  // Affichage_Text("Calib Gyro");
  // while (digitalRead(BTN) == 0);
  // Affichage_Text("wait");
  // while (digitalRead(BTN));
  // RGB(0, 1, 0);
  // InitGyro();
  // RGB(0, 0, 0);
  // Beep(2, 30);
  // Affichage_Text("Calibration Gyro Terminee");

  delay(1000);

  // Show sensor bars until the button is pressed
  while (digitalRead(BTN) == 0) {
    Afficher_Tout();
  }
  Affichage_Text("let's go ?");
}
