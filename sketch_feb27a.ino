#include <SPI.h>

#define CS_PIN 10

void setup() {
    Serial.begin(115200);
    SPI.begin();
    pinMode(CS_PIN, OUTPUT);
    digitalWrite(CS_PIN, HIGH);
    
    Serial.println("Testing SPI communication...");

    digitalWrite(CS_PIN, LOW);
    byte response = SPI.transfer(0xFF);  // Send dummy data
    digitalWrite(CS_PIN, HIGH);
    
    Serial.print("MCP2515 Response: 0x");
    Serial.println(response, HEX);

    if (response == 0x00 || response == 0xFF) {
        Serial.println("ERROR: MCP2515 not responding!");
    } else {
        Serial.println("MCP2515 detected!");
    }
}

void loop() {}
