#include "EmbeddedDevice.h"
#include <Arduino.h>

ToggableLedDevice::ToggableLedDevice(
    int buttonPin,
    int ledPin,
    int dhtPin,
    bool initialLedState)
    : button(buttonPin, this), led(ledPin, initialLedState, this), dht(dhtPin, 11, this) {}

void ToggableLedDevice::on(Event event) {
    if (event == Button::BUTTON_PRESSED_EVENT) {
        led.handle(Led::TOGGLE_LED_COMMAND);

    }
}

void ToggableLedDevice::handle(Command command) {
    if (command == Led::TOGGLE_LED_COMMAND ||
        command == Led::TURN_ON_COMMAND ||
        command == Led::TURN_OFF_COMMAND) {
        Serial.printf("LED state: %d\n", led.getState());
        }
}

Led& ToggableLedDevice::getLed() {
    return led;
}

void ToggableLedDevice::triggerButtonEvent(Event event) {
    button.on(event);
}