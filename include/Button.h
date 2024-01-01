#ifndef BUTTON_H
#define BUTTON_H

#include "Logging.h"
#include "LEDController.h"
#include "BLELampService.h"
#include <BLEService.h>

class Button {
public:
    Button(uint8_t pin, LEDController& ledController, BLELampService& bleService) 
    : pin(pin), ledController(ledController), bleService(bleService){
        pinMode(pin, INPUT_PULLUP);
    }

    void update();

private:
    uint8_t pin;
    LEDController& ledController;
    BLELampService& bleService;
    int lastState = LOW;
    unsigned long lastDebounceTime = 0;
    unsigned long debounceDelay = 50;
    unsigned long firstTapTime = 0;
    unsigned long longPressStartTime =0;

    void onSingleTap();
    void onDoubleTap();
    void onLongPress();

    const unsigned long ButTimeout = 500;
    const unsigned long LongPressTimeout = 1500;
};

#endif