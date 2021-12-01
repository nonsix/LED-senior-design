// Author: Devon Schimming

#include <SPI.h>
#include <RH_RF95.h>

// for feather m0 RFM9x
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// stores timestamp from last signal in milliseconds
int delta_signal = 0;

// Amount of time in miliseconds the display will stay a state with no input
const int MILLIS_TIMEOUT = 5000;

// Array of expected signals
// This is where you add more expected signals
// ANOTHER_SIGNAL is a placeholder and could be anything else
char const *signals[] = {"PIR", "ANOTHER_SIGNAL"};

// Each avaliable state
// This is where you could add different display states
enum STATE
{
    IDLE = 0,    //default state, no active display
    WARNING = 1, //Active state, displays light code for PIR detection
};

// default state
STATE current_state = IDLE;

// Sets the display state to its next state based on its current.
// IE. IDLE -> WARNING -> WARNING -> ...
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

    // After changing state we will want to refresh the timer
    delta_signal = millis();
}

// state_timeout
//
// delta_time:  Time sense last signal received
// timeout:     Maximum time a state is allowed to display
//
// returns:     True of False determented from elapsed time
bool state_timeout(int delta_time, int timeout)
{
    return (millis() - delta_time) > timeout;
}

// Sets the inital values for the display
void setup()
{
    // The pin connected to the led must be set as an output
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

// handel_state
//
// state:   The state the display is going to represent
//
// return:  no return value
void handel_state(STATE state)
{
    // in milliseconds
    const int BLINK_DELAY = 500;

    // Every state needs to be setup in here if you want the display to reflect it.
    // If not it will also be cause by default including IDEL
    switch (state)
    {
    case WARNING:
        digitalWrite(LED_BUILTIN, LOW);
        delay(BLINK_DELAY);
        digitalWrite(LED_BUILTIN, HIGH);
        break;
    default:
        digitalWrite(LED_BUILTIN, HIGH);
        break;
    }

    delay(BLINK_DELAY);
}

// returns index of searched char
int find(char const *str, uint8_t len, char search)
{
    for (int i = 0; i < len; i++)
    {
        if (str[i] == search)
            return i;
    }
    return -1;
}

void loop()
{
    if (rf95.available())
    {
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);
        if (rf95.recv(buf, &len))
        {
            Serial.println("Signal recvied. ");

            // This next line counts how many signal types are in the array
            int const signals_length = sizeof(signals) / sizeof(signals[0]);

            // Stores and interprets the input
            char const *data = (char *)buf;
            String packet = String(data);

            for (int i = 0; i < signals_length; i++)
            {
                int const delimiter = find(data, len, ':');
                String signal = packet.substring(0, delimiter);
                String location = packet.substring(delimiter+1, len);
                
                if (signal == signals[i])
                {
                    Serial.println("Got Signal from " + location + " of type " + signal);
                    update_signal();
                    break;
                }
                 
            }
        }
        else
        {
            Serial.println("Signal failed. ");
        }
    }

    if (state_timeout(delta_signal, MILLIS_TIMEOUT))
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
