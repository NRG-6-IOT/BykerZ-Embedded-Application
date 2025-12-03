#ifndef EMBEDDED_DEVICE_H
#define EMBEDDED_DEVICE_H

#include "Device.h"
#include "Button.h"
#include "Led.h"
#include "DHTSensor.h"

class ToggableLedDevice : public Device {
private:
    Button button; ///< Button sensor instance from the framework.
    Led led; ///< LED actuator instance from the framework.
    DHTSensor dht;

public:
    static const int BUTTON_PIN = 26; ///< Default GPIO pin for the button.
    static const int LED_PIN = 27; ///< Default GPIO pin for the LED.
    static const int DHT_PIN = 8;

    ToggableLedDevice(
        int buttonPin = BUTTON_PIN,
        int ledPin = LED_PIN,
        int dhtPin = DHT_PIN,
        bool initialLedState = false
    ); ///< Constructor with configurable pins and initial LED state.
    void on(Event event) override; ///< Handles button press events.
    void handle(Command command) override; ///< Handles LED command responses (e.g., state reporting).
    Led& getLed(); ///< Provides access to the LED instance.
    void triggerButtonEvent(Event event); ///< Triggers a button event externally (e.g., from ISR).
};

#endif //EMBEDDED_DEVICE_H