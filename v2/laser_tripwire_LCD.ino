#include <LiquidCrystal.h>

// LCD pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int laserPin = 8;
const int sensorPin = A0;
const int buzzerPin = 7;
const int ledPin = LED_BUILTIN;

int threshold = 500;

// Keeps track of whether we're currently in an alarm state
bool alarmActive = false;

void setup() {
  pinMode(laserPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  digitalWrite(laserPin, HIGH);

  Serial.begin(9600);

  lcd.begin(16, 2);

  // Startup screen
  lcd.setCursor(0, 0);
  lcd.print("LASER SYSTEM");
  lcd.setCursor(0, 1);
  lcd.print("Booting...");
  delay(2000);

  lcd.clear();
}

void loop() {
  int light = analogRead(sensorPin);

  Serial.println(light);

  if (light > threshold) {

    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);

    // Only update the LCD once when the alarm first triggers
    if (!alarmActive) {
      alarmActive = true;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("!!! ALERT !!!");
      lcd.setCursor(0, 1);
      lcd.print("Beam Broken!");
    }

  } else {

    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);

    // Only update the LCD once when returning to safe
    if (alarmActive) {
      alarmActive = false;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("LASER SYSTEM");
      lcd.setCursor(0, 1);
      lcd.print("Status: SAFE");
    }
  }

  delay(50);
}
