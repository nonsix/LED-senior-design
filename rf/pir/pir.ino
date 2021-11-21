// Author: Devon Schimming

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

#define RF95_FREQ 915.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);

#define inPin 15 // pin A1

// Sets the inital values for the display
void setup()
{
    pinMode(inPin, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);
    while (!Serial)
    {
        delay(1);
    }
    // manual reset
    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);
    if (!rf95.init())
        Serial.println("init failed");
    if (!rf95.setFrequency(RF95_FREQ))
    {
        Serial.println("setFrequency failed");
        while (1)
            ;
    }
    Serial.print("Set Freq to: ");
    Serial.println(RF95_FREQ);
    rf95.setTxPower(20, false);
}

void loop()
{
    if (digitalRead(inPin) == LOW)
    {
        uint8_t data[] = "PIR";
        rf95.send(data, sizeof(data));
        Serial.println("Sent signal");

        rf95.waitPacketSent();
        delay(1000);
    }
}
