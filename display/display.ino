#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 rf95(8, 3); // Adafruit Feather M0 with RFM95

int led = 9;
int delta_signal = 0; // timestamp from last signal in milliseconds

enum STATE
{
    WAITING = 0,
    WARNING = 1,
    EMERGENCY = 2,
    IDLE = 3,
};

enum SIGNAL
{
    PIR = 0,
    BREAK_BEAM = 1
};

STATE current_state = WAITING;

STATE handel_signal(SIGNAL signal)
{
    switch (current_state)
    {
    case WAITING:
        switch (signal)
        {
        case PIR:
            current_state = WARNING;
            break;
        case BREAK_BEAM:
            current_state = WARNING;
            break;
        }
        break;
    case WARNING:
        switch (signal)
        {
        case PIR:
            current_state = WARNING;
            break;
        case BREAK_BEAM:
            current_state = EMERGENCY;
            break;
        }
        break;
    case EMERGENCY:
        switch (signal)
        {
        case PIR:
            current_state = EMERGENCY;
            break;
        case BREAK_BEAM:
            current_state = EMERGENCY;
            break;
        }
        break;
    }
}

bool state_timeout(int delta_time)
{
    int timeout = 60000; // 1 min in milliseconds
    return delta_time > timeout;
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
    current_state = WARNING;
    while (!Serial)
        ; // Wait for serial port to be available
    if (!rf95.init())
        Serial.println("init failed");
}

void update()
{
    // move sensor handel here
}

void handel_state(STATE state)
{
    // based on current_state display the correct led output
    switch (state)
    {
    case WAITING:
        Serial.print("waiting, ");
        digitalWrite(LED_BUILTIN, HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW);
        delay(1000);
        break;

    case WARNING:
        Serial.print("warning, ");
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);

        break;

    case EMERGENCY:
        Serial.print("emergency, ");
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
        break;

    case IDLE:
        Serial.print("idle.");
        digitalWrite(LED_BUILTIN, LOW);
        break;
    }
}

void loop()
{
    // if (rf95.available())
    // {
    //     // uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    //     // uint8_t len = sizeof(buf);
    //     // if (rf95.recv(buf, &len))
    //     // {
    //     //     // digitalWrite(led, HIGH);
    //     //     // Serial.print("got singal: ");
    //     //     // Serial.println((int *)buf);
    //     //     // handel_signal((int *)buf)
    //     //     // if (!state_timeout(delta_signal))
    //     //     // {
    //     //     //     delta_signal = milli() - delta_signal; // calcs how much time passed sense last signal
    //     //     // } else {
    //     //     //     current_state = WAITING;
    //     //     //     delta_signal = 0;
    //     //     // }
    //     //     update();
    //     //     display(current_state);
    //     // }
    //     // else
    //     // {
    //     //     Serial.println("recv failed");
    //     // }
    // }
    update();
    handel_state(current_state);
}
