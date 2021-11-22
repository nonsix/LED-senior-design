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
    pinMode(inPin, INPUT); // sets pin to digital input
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
        // We don't want to continue
        while (1)
            ;
    }
    Serial.print("Set Freq to: ");
    Serial.println(RF95_FREQ);
    // Tx power defaults to 13 but can be set in the range of 2 to 20
    // Higher the number the more range
    rf95.setTxPower(20, false);
}

void loop()
{
    // When the inPin is not receiving a signal transmit to display
    if (digitalRead(inPin) == LOW)
    {
        char const *signal = "PIR";
        char const *location = "NORTH";
        char const *data = signal + ":" + location;
        rf95.send(data, sizeof(data));
        Serial.println("Sent signal");

        rf95.waitPacketSent();
        delay(1000);
    }
}
