#include <SPI.h>
#include <mcp_can.h>

#define CAN_CS 10  // Chip Select pin

MCP_CAN CAN(CAN_CS);

void setup() {
    Serial.begin(115200);

    while (CAN.begin(MCP_ANY, CAN_125KBPS, MCP_8MHZ) != CAN_OK) {
        Serial.println("CAN Init Failed! Retrying...");
        delay(100);
    }

    Serial.println("CAN Initialized at 250 kbps!");
}

void loop() {
    unsigned char msg[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

    if (CAN.sendMsgBuf(0x200, 0, 8, msg) == CAN_OK) {
        Serial.println("Message Sent!");
    } else {
        Serial.println("Send Failed!");
    }

    delay(1000);
}
