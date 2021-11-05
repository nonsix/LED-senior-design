// Author: Devon Schimming

#include <SPI.h>
#include <RH_RF95.h>

// for feather m0 RFM9x
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

int delta_signal = 0; // timestamp from last signal in milliseconds

// Each avaliable state
enum STATE
{
    IDLE = 0, //default state, no active display
    WARNING = 1, //Active state, displays light code for PIR detection
};

// default state
STATE current_state = IDLE;

// Sets the display state to its next state based on its current.
void update_signal()
{
    switch (current_state)
    {
    case IDLE:
        current_state = WARNING;
        break;
    case WARNING:
        current_state = WARNING;
        break;
    }
    delta_signal = millis();
}

// delta_time:  Time sense last signal received
// timeout:     Maximum time a state is allowed to display
bool state_timeout(int delta_time, int timeout)
{
    return (millis() - delta_time) > timeout;
}

// Sets the inital values for the display
void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, HIGH);

    Serial.begin(115200);

    current_state = IDLE;
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

    // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
    if (!rf95.setFrequency(RF95_FREQ))
    {
        Serial.println("setFrequency failed");
        while (1)
            ;
    }
    Serial.print("Set Freq to: ");
    Serial.println(RF95_FREQ);

    rf95.setTxPower(23, false);
}

// state:   The state the display is going to represent
void handel_state(STATE state)
{
    switch (state)
    {
    case IDLE:
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        break;

    case WARNING:
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        break;
    }
}

void loop()
{
    if (rf95.available())
    {
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);
        if (rf95.recv(buf, &len))
        {
            // char data[RH_RF95_MAX_MESSAGE_LEN] = (char *)buf;
            char const *PIR = "PIR";
            char const *data = (char*)buf;
            Serial.print("Signal recvied. ");
            Serial.println((char *)buf);
            if (strcmp(PIR, data) == 0)
            {
                Serial.println("TO");
                update_signal();
            }

            // Send a reply
            uint8_t message[] = "And hello back to you";
            rf95.send(message, sizeof(message));
            rf95.waitPacketSent();
            Serial.println("Sent a reply");
        }
        else
        {
            Serial.println("Signal failed. ");
        }
    }

    const int millis_timeout = 2500;
    if (state_timeout(delta_signal, millis_timeout))
    {
        // cycles through each state
        // current_state = STATE((current_state + WARNING) % 2);
        current_state = IDLE;
        // reset timestamp to current time
        delta_signal = millis();
    }

    // sets display to the correct mode based on current_state
    handel_state(current_state);
}
