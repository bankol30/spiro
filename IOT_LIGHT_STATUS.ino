#include <WiFi.h>
#include <HTTPClient.h>

// Wi-Fi network credentials
//const char* ssid = "MTN_2.4G_351941";
//const char* password = "5295AC9F";

// Wi-Fi network credentials
//const char* ssid = "jedtronix";
//const char* password = "4252321etech";

// Wi-Fi network credentials
const char* ssid = "MTN_4G_9694FD";
const char* password = "3UC266J272";



const int analogPin = 34; // ADC pin (ensure it's a valid ADC pin for ESP32)
const float voltageDividerRatio = 517.0 / 3.3; // Adjust based on your divider circuit


// Server URL to send the POST request
const char* serverURL = "https://www.futatab.com/light_update.php";

// Example data for ID and voltage
int id = 1;             // Replace with your actual device ID
int dataled = 13;
int errorled = 12;

// Timer variables
unsigned long previousMillis = 0;  // Stores the last time the POST request was sent
const unsigned long interval = 60000;  // 1 minute interval in milliseconds

void setup() {
  // Start Serial Monitor
  Serial.begin(115200);

  // Configure LED pin
  pinMode(dataled, OUTPUT);
   pinMode(errorled, OUTPUT);
 
    analogReadResolution(12); // Set ADC resolution to 12 bits
        digitalWrite(dataled, HIGH);
        digitalWrite(errorled, HIGH);
    delay(2000);
      digitalWrite(dataled, LOW);
        digitalWrite(errorled, LOW);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}

void loop() {
  // Get the current time in milliseconds
  unsigned long currentMillis = millis();
  int rawADC = analogRead(analogPin); // Read raw ADC value
  float measuredVoltage = (rawADC / 4095.0) * 3.3; // Convert ADC value to voltage
  int  voltage = measuredVoltage * voltageDividerRatio; // Scale to grid voltage

  // Check if 1 minute has passed
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Update the timer
    sendPOSTRequest(id, (unsigned int)voltage);   // Send the POST request
  }

  // Maintain Wi-Fi connection
  if (WiFi.status() != WL_CONNECTED) {
    reconnectWiFi();
  }
}

void sendPOSTRequest(int id, float voltage) {
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(dataled, HIGH);
    delay(200);
    digitalWrite(dataled, LOW);

    HTTPClient http;

    // Specify the URL
    http.begin(serverURL);

    // Set the content type to JSON
    http.addHeader("Content-Type", "application/json");

    // Create the JSON payload
    String jsonData = "{\"id\":" + String(id) + ", \"voltage\":" + String(voltage, 2) + "}";

    // Debug: Print JSON data
    Serial.println("Sending JSON payload: " + jsonData);

    // Send the POST request
    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Response Code: " + String(httpResponseCode));
      Serial.println("Response: " + response);

      // Indicate success with a short LED blink
      digitalWrite(dataled, HIGH);
      delay(100);
      digitalWrite(dataled, LOW);
    } else {
      Serial.print("Error sending POST:");
      Serial.println(httpResponseCode);

      // Indicate failure with a long LED blink
      digitalWrite(errorled, HIGH);
      delay(500);
       digitalWrite(errorled, LOW);
      delay(500);
    }

    http.end();  // End the HTTP connection
  } else {
    Serial.println("WiFi not connected");
    reconnectWiFi();
  }
}

void reconnectWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    Serial.print("Reconnecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(errorled, HIGH);
      delay(500);
          digitalWrite(errorled, LOW);
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nReconnected to WiFi");
      digitalWrite(errorled, LOW);
  }
}
