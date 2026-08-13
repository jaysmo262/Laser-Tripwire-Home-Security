#include <WiFi.h>
#include <LiquidCrystal.h>

// ==========================================
// WIFI
// ==========================================

const char* ssid = "MirrorLake";
const char* password = "White-House-Red-Barn";

// Create web server on normal HTTP port 80
WiFiServer server(80);


// ==========================================
// SECURITY SYSTEM PINS
// ==========================================

const int laserPin = D8;
const int sensorPin = A0;
const int buzzerPin = D7;

// LCD:
// RS -> D2
// E  -> D3
// D4 -> D4
// D5 -> D5
// D6 -> D6
// D7 -> D9
LiquidCrystal lcd(D2, D3, D4, D5, D6, D9);


// ==========================================
// SENSOR SETTINGS
// ==========================================

// Your readings:
// Laser hitting = ~270
// Beam blocked  = ~2700

const int threshold = 1500;

int sensorValue = 0;
bool alarmTriggered = false;


// ==========================================
// SETUP
// ==========================================

void setup() {

  Serial.begin(115200);
  delay(1000);

  // --------------------
  // Pins
  // --------------------

  pinMode(laserPin, OUTPUT);
  pinMode(sensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  // TURN LASER ON
  digitalWrite(laserPin, HIGH);

  // Start buzzer OFF
  digitalWrite(buzzerPin, LOW);


  // --------------------
  // LCD
  // --------------------

  lcd.begin(16, 2);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SECURITY SYSTEM");

  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  delay(1500);


  // --------------------
  // WIFI
  // --------------------

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WIFI CONNECTED!");

  Serial.print("IP ADDRESS: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected!");

  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  delay(2500);

  // Start web server
  server.begin();

  Serial.println("Web server started!");
}


// ==========================================
// MAIN LOOP
// ==========================================

void loop() {

  // Read photoresistor
  sensorValue = analogRead(sensorPin);

  Serial.print("Sensor: ");
  Serial.println(sensorValue);


  // ========================================
  // SECURITY SYSTEM
  // ========================================

  if (sensorValue > threshold) {

    alarmTriggered = true;

    // Alarm ON
    digitalWrite(buzzerPin, HIGH);

    lcd.setCursor(0, 0);
    lcd.print("!!! ALERT !!!   ");

    lcd.setCursor(0, 1);
    lcd.print("BEAM BROKEN     ");
  }

  else {

    alarmTriggered = false;

    // Alarm OFF
    digitalWrite(buzzerPin, LOW);

    lcd.setCursor(0, 0);
    lcd.print("SYSTEM ARMED    ");

    lcd.setCursor(0, 1);
    lcd.print("Laser: SAFE       ");
  }


  // ========================================
  // WEB SERVER
  // ========================================

  WiFiClient client = server.available();

  if (client) {

    Serial.println("Web browser connected!");

    String request = "";

    unsigned long timeout = millis();

    while (client.connected() &&
           millis() - timeout < 2000) {

      if (client.available()) {

        char c = client.read();
        request += c;

        // Blank line = end of HTTP request
        if (request.endsWith("\r\n\r\n")) {
          break;
        }
      }
    }


    // ======================================
    // SEND WEB PAGE
    // ======================================

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();


    client.println("<!DOCTYPE html>");
    client.println("<html>");

    client.println("<head>");

    // Automatically refresh page every second
    client.println("<meta http-equiv='refresh' content='1'>");

    client.println(
      "<meta name='viewport' "
      "content='width=device-width, initial-scale=1'>"
    );

    client.println("<title>Laser Security System</title>");

    // --------------------
    // CSS
    // --------------------

    client.println("<style>");

    client.println(
      "body {"
      "font-family: Arial;"
      "background:#111827;"
      "color:white;"
      "text-align:center;"
      "margin-top:60px;"
      "}"
    );

    client.println(
      ".box {"
      "background:#1f2937;"
      "max-width:450px;"
      "margin:auto;"
      "padding:30px;"
      "border-radius:15px;"
      "}"
    );

    client.println(
      ".armed {"
      "color:#4ade80;"
      "font-size:32px;"
      "font-weight:bold;"
      "}"
    );

    client.println(
      ".alert {"
      "color:#f87171;"
      "font-size:32px;"
      "font-weight:bold;"
      "}"
    );

    client.println(
      ".sensor {"
      "font-size:22px;"
      "}"
    );

    client.println("</style>");

    client.println("</head>");


    // ======================================
    // PAGE BODY
    // ======================================

    client.println("<body>");

    client.println("<div class='box'>");

    client.println("<h1>Laser Security System</h1>");


    // STATUS

    if (alarmTriggered) {

      client.println(
        "<div class='alert'>"
        "ALERT - BEAM BROKEN"
        "</div>"
      );

    }

    else {

      client.println(
        "<div class='armed'>"
        "SYSTEM ARMED"
        "</div>"
      );

    }


    // SENSOR READING

    client.print("<p class='sensor'>Sensor Reading: ");

    client.print(sensorValue);

    client.println("</p>");


    // THRESHOLD

    client.print("<p>Alarm Threshold: ");

    client.print(threshold);

    client.println("</p>");


    // LASER

    client.println("<p>Laser: ON</p>");


    // WIFI

    client.println("<p>WiFi: Connected</p>");


    client.println("</div>");

    client.println("</body>");

    client.println("</html>");


    // End connection
    client.stop();

    Serial.println("Browser disconnected.");
  }


  delay(100);
}