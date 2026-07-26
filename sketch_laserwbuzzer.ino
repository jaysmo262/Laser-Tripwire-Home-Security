chatconst int laserPin = 8;
const int sensorPin = A0;
const int buzzerPin = 7;
const int ledPin = LED_BUILTIN;

int threshold = 500;

void setup() {
  pinMode(laserPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  // Turn laser on
  digitalWrite(laserPin, HIGH);

  Serial.begin(9600);
}

void loop() {
  int light = analogRead(sensorPin);

  Serial.println(light);

  // Beam is broken
  if (light > threshold) {
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
  }

  // Laser is hitting sensor
  else {
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);
  }

  delay(50);
}