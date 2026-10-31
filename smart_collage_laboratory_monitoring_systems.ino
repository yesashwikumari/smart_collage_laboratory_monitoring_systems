#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <ESP32Servo.h>

// =====================================================
//                    OLED
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// =====================================================
//                    DHT11
// =====================================================

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// =====================================================
//                    LEDs
// =====================================================

#define GREEN_LED 2
#define RED_LED 18

// =====================================================
//                    SENSORS
// =====================================================

#define MQ2_PIN 34
#define ZMPT_PIN 35
#define ACS_PIN 32

#define FLAME_PIN 27
#define PIR_PIN 25

// =====================================================
//                    BUZZER
// =====================================================

#define BUZZER_PIN 26

// =====================================================
//                    SERVO
// =====================================================

#define SERVO_PIN 19

Servo doorServo;

// =====================================================
//                  THRESHOLDS
// =====================================================

#define TEMP_THRESHOLD 45.0
#define MQ2_THRESHOLD 2000

// =====================================================
//                OLED PAGE CONTROL
// =====================================================

int currentPage = 0;

unsigned long lastPageChange = 0;

const unsigned long PAGE_TIME = 2500;

// =====================================================
//                     SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  // DHT
  dht.begin();

  // OLED
  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {

    Serial.println("OLED NOT FOUND!");

    while (true) {
      delay(1000);
    }
  }

  // Pins
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  pinMode(FLAME_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);

  // Servo
  doorServo.attach(SERVO_PIN);

  // Initial state
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  doorServo.write(0);

  // Starting screen
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(5, 5);
  display.println("SMART LAB");

  display.setTextSize(1);
  display.setCursor(15, 30);
  display.println("MONITORING SYSTEM");

  display.setCursor(25, 48);
  display.println("STARTING...");

  display.display();

  delay(2500);

  Serial.println();
  Serial.println("=================================");
  Serial.println("     SMART LAB MONITORING");
  Serial.println("=================================");
  Serial.println("SYSTEM READY");
}

// =====================================================
//                      LOOP
// =====================================================

void loop() {

  // ===================================================
  //                 READ DHT11
  // ===================================================

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // ===================================================
  //                 READ ANALOG
  // ===================================================

  int mq2Value = analogRead(MQ2_PIN);
  int zmptValue = analogRead(ZMPT_PIN);
  int acsValue = analogRead(ACS_PIN);

  // ===================================================
  //                 READ DIGITAL
  // ===================================================

  int flameValue = digitalRead(FLAME_PIN);
  int pirValue = digitalRead(PIR_PIN);

  // ===================================================
  //                 EMERGENCY LOGIC
  // ===================================================

  bool emergency = false;

  // High temperature
  if (!isnan(temperature) &&
      temperature >= TEMP_THRESHOLD) {

    emergency = true;
  }

  // MQ-2
  if (mq2Value >= MQ2_THRESHOLD) {

    emergency = true;
  }

  // Flame
  // Common flame modules:
  // LOW = flame detected

  if (flameValue == LOW) {

    emergency = true;
  }

  // ===================================================
  //                 EMERGENCY OUTPUT
  // ===================================================

  if (emergency) {

    // Green OFF
    digitalWrite(GREEN_LED, LOW);

    // Red ON
    digitalWrite(RED_LED, HIGH);

    // Buzzer ON
    digitalWrite(BUZZER_PIN, HIGH);

  }

  else {

    // Green ON
    digitalWrite(GREEN_LED, HIGH);

    // Red OFF
    digitalWrite(RED_LED, LOW);

    // Buzzer OFF
    digitalWrite(BUZZER_PIN, LOW);
  }

  // ===================================================
  //                     DOOR
  // ===================================================

  if (emergency) {

    // Emergency -> door open
    doorServo.write(90);

  }

  else if (pirValue == HIGH) {

    // Motion -> door open
    doorServo.write(90);

  }

  else {

    // No motion + no emergency
    doorServo.write(0);
  }

  // ===================================================
  //               CHANGE OLED PAGE
  // ===================================================

  if (millis() - lastPageChange >= PAGE_TIME) {

    currentPage++;

    if (currentPage > 4) {
      currentPage = 0;
    }

    lastPageChange = millis();
  }

  // ===================================================
  //                  OLED DISPLAY
  // ===================================================

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  // ---------------------------------------------------
  // PAGE 0 - MAIN STATUS
  // ---------------------------------------------------

  if (currentPage == 0) {

    display.setTextSize(2);
    display.setCursor(5, 0);

    if (emergency) {
      display.println("EMERGENCY!");
    }
    else {
      display.println("NORMAL");
    }

    display.drawLine(0, 18, 127, 18, SSD1306_WHITE);

    display.setTextSize(1);

    display.setCursor(0, 23);
    display.print("Green LED: ");
    display.println(emergency ? "OFF" : "ON");

    display.setCursor(0, 34);
    display.print("Red LED  : ");
    display.println(emergency ? "ON" : "OFF");

    display.setCursor(0, 45);
    display.print("Buzzer   : ");
    display.println(emergency ? "ON" : "OFF");

    display.setCursor(0, 56);
    display.print("Door     : ");

    if (emergency || pirValue == HIGH) {
      display.println("OPEN");
    }
    else {
      display.println("CLOSED");
    }
  }

  // ---------------------------------------------------
  // PAGE 1 - DHT11 + MQ2
  // ---------------------------------------------------

  else if (currentPage == 1) {

    display.setTextSize(1);

    display.setCursor(0, 0);
    display.println("ENVIRONMENT");

    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

    display.setCursor(0, 15);
    display.print("Temperature: ");

    if (isnan(temperature)) {
      display.println("ERROR");
    }
    else {
      display.print(temperature, 1);
      display.println(" C");
    }

    display.setCursor(0, 28);
    display.print("Humidity   : ");

    if (isnan(humidity)) {
      display.println("ERROR");
    }
    else {
      display.print(humidity, 1);
      display.println(" %");
    }

    display.setCursor(0, 41);
    display.print("MQ-2       : ");
    display.println(mq2Value);

    display.setCursor(0, 54);
    display.print("Gas Status : ");

    if (mq2Value >= MQ2_THRESHOLD) {
      display.println("ALERT");
    }
    else {
      display.println("NORMAL");
    }
  }

  // ---------------------------------------------------
  // PAGE 2 - VOLTAGE + CURRENT
  // ---------------------------------------------------

  else if (currentPage == 2) {

    display.setTextSize(1);

    display.setCursor(0, 0);
    display.println("ELECTRICAL MONITOR");

    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

    display.setCursor(0, 17);
    display.print("ZMPT101B : ");
    display.println(zmptValue);

    display.setCursor(0, 32);
    display.print("ACS712   : ");
    display.println(acsValue);

    display.setCursor(0, 47);
    display.println("Status: MONITORING");

    display.setCursor(0, 58);
    display.println("Raw ADC values");
  }

  // ---------------------------------------------------
  // PAGE 3 - FLAME + PIR
  // ---------------------------------------------------

  else if (currentPage == 3) {

    display.setTextSize(1);

    display.setCursor(0, 0);
    display.println("SAFETY SENSORS");

    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

    display.setCursor(0, 18);
    display.print("Flame: ");

    if (flameValue == LOW) {
      display.println("DETECTED");
    }
    else {
      display.println("NO FLAME");
    }

    display.setCursor(0, 33);
    display.print("PIR  : ");

    if (pirValue == HIGH) {
      display.println("MOTION");
    }
    else {
      display.println("NO MOTION");
    }

    display.setCursor(0, 48);
    display.print("Door : ");

    if (emergency || pirValue == HIGH) {
      display.println("OPEN");
    }
    else {
      display.println("CLOSED");
    }
  }

  // ---------------------------------------------------
  // PAGE 4 - COMPLETE STATUS
  // ---------------------------------------------------

  else if (currentPage == 4) {

    display.setTextSize(1);

    display.setCursor(0, 0);
    display.println("SYSTEM STATUS");

    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

    display.setCursor(0, 15);
    display.print("DHT11 : ");
    display.println(isnan(temperature) ? "ERROR" : "OK");

    display.setCursor(0, 27);
    display.print("MQ-2  : ");
    display.println(mq2Value >= MQ2_THRESHOLD ? "ALERT" : "OK");

    display.setCursor(0, 39);
    display.print("Flame : ");
    display.println(flameValue == LOW ? "ALERT" : "OK");

    display.setCursor(0, 51);
    display.print("PIR   : ");
    display.println(pirValue == HIGH ? "MOTION" : "CLEAR");

  }

  display.display();

  // ===================================================
  //                 SERIAL MONITOR
  // ===================================================

  Serial.println();
  Serial.println("=================================");

  if (emergency) {
    Serial.println("!!! EMERGENCY DETECTED !!!");
  }
  else {
    Serial.println("SYSTEM STATUS: NORMAL");
  }

  Serial.println("---------------------------------");

  Serial.print("Temperature : ");

  if (isnan(temperature)) {
    Serial.println("ERROR");
  }
  else {
    Serial.print(temperature);
    Serial.println(" C");
  }

  Serial.print("Humidity    : ");

  if (isnan(humidity)) {
    Serial.println("ERROR");
  }
  else {
    Serial.print(humidity);
    Serial.println(" %");
  }

  Serial.print("MQ-2       : ");
  Serial.println(mq2Value);

  Serial.print("ZMPT101B   : ");
  Serial.println(zmptValue);

  Serial.print("ACS712     : ");
  Serial.println(acsValue);

  Serial.print("Flame      : ");

  if (flameValue == LOW) {
    Serial.println("DETECTED");
  }
  else {
    Serial.println("NO FLAME");
  }

  Serial.print("PIR        : ");

  if (pirValue == HIGH) {
    Serial.println("MOTION");
  }
  else {
    Serial.println("NO MOTION");
  }

  Serial.print("Buzzer     : ");

  if (emergency) {
    Serial.println("ON");
  }
  else {
    Serial.println("OFF");
  }

  Serial.print("Door       : ");

  if (emergency || pirValue == HIGH) {
    Serial.println("OPEN");
  }
  else {
    Serial.println("CLOSED");
  }

  Serial.println("=================================");

  delay(100);
}