// Author: Devon Schimming

#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 rf95(8, 3); // Adafruit Feather M0 with RFM9

int inPin = 15; // pin A1

// Sets the inital values for the display
void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
    while (!Serial)
        ; // Wait for serial port to be available
    if (!rf95.init())
        Serial.println("init failed");
}

void loop()
{
    if (digitalRead(inPin) == HIGH) {
        uint8_t data[] = "PIR";
        rf95.send(data, sizeof(data));
        Serial.println("Sent data");

        rf96.waitPacketSent();
        delay(1000);
    }
}
