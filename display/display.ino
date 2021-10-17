// Author: Devon Schimming

#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 rf95(8, 3); // Adafruit Feather M0 with RFM95

int delta_signal = 0; // timestamp from last signal in milliseconds

// Each avaliable state
enum STATE
{
    IDLE = 0,
    WARNING = 1,
};

// default state
STATE current_state = IDLE;

// Sets the display state to its next state based on its current.
STATE update_signal()
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
    Serial.begin(9600);
    current_state = IDLE;
    while (!Serial)
        ; // Wait for serial port to be available
    if (!rf95.init())
        Serial.println("init failed");
}

// state:   The state the display is going to represent
void handel_state(STATE state)
{
    switch (state)
    {
    case IDLE:
        Serial.print("idle. ");
        digitalWrite(LED_BUILTIN, HIGH);
        break;

    case WARNING:
        Serial.print("warning, ");
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
            Serial.print("Signal recvied. ");
            Serial.println((int *)buf);
            uint8_t data[] = (int* )buf;

            if (data == "PIR") {
                update_signal();
            }
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
        current_state = STATE((current_state + WARNING) % 2);

        // reset timestamp to current time
        delta_signal = millis();
    }

    // sets display to the correct mode based on current_state
    handel_state(current_state);
}
