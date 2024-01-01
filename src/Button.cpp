#include "Button.h"

void Button::update() {
    unsigned long currTime = millis();

    if (longPressStartTime && firstTapTime && (currTime - longPressStartTime) > LongPressTimeout) {
        print_log_ln("LONG PRESS");
        this->onLongPress();
        longPressStartTime = 0;
        firstTapTime = 0;
    } else if (firstTapTime && (currTime - firstTapTime) > ButTimeout) {
        firstTapTime = 0;
        longPressStartTime = 0;
        this->onSingleTap();
        print_log_ln("SINGLE TAP");
    }

    int reading = digitalRead(pin);
    if (lastState != reading && (currTime - lastDebounceTime) > debounceDelay) {
    lastState = reading;

        if (LOW == reading) {
            if (firstTapTime) {
                firstTapTime = 0;
                longPressStartTime = 0;
                this->onDoubleTap();
                print_log_ln("DOUBLE TAP");
            } else {
                firstTapTime = 0 == currTime ? 1 : currTime;
            }
        } else {
            longPressStartTime = millis();
        }
    }
}

void Button::onSingleTap() {
    if (ledController.state()) {
        int currentBrightness = ledController.getBrightness();
        int stepIndex = 0;
        for (int i = 0; i < 4; i++) {
            if (abs(currentBrightness - brightnessSteps[i]) < abs(currentBrightness - brightnessSteps[stepIndex])) {
                stepIndex = i;
            }
        }

        // Move to the next step
        stepIndex = (stepIndex + 1) % 4;
        int newBrightness = brightnessSteps[stepIndex];
            ledController.setBrightness(newBrightness);

        // Update BLE characteristic and send notification
    auto brightnessCharacteristic = bleService.getBrightnessCharacteristic();
    if (brightnessCharacteristic) {
        // String brightnessStr = String(newBrightness);

        int brightness = ledController.getBrightness();

        // Ensure brightness is in the range 0-255
        if (newBrightness < 0) newBrightness = 0;
        if (newBrightness > 255) newBrightness = 255;

        uint8_t brightnessByte = static_cast<uint8_t>(newBrightness);
        print_log("brightnessByte: ");
        print_log_ln(brightnessByte);
    
        brightnessCharacteristic->setValue(&brightnessByte, sizeof(brightnessByte));

        // brightnessCharacteristic->setValue(brightnessStr.c_str());
        brightnessCharacteristic->notify();
    }
    }
}

void Button::onDoubleTap() {
    if (ledController.state()) {
    ledController.nextMode();

        // Update BLE characteristic and send notification
    auto pModeCharacteristic = bleService.getModeCharacteristic();
    if (pModeCharacteristic) {
        int mode = ledController.getMode();

        uint8_t modeByte = static_cast<uint8_t>(mode);
        print_log("ModeByte: ");
        print_log_ln(modeByte);
    
        pModeCharacteristic->setValue(&modeByte, sizeof(modeByte));
    
        pModeCharacteristic->notify();
    }
    }
}

void Button::onLongPress() {

    auto pLampStateCharacteristic = this->bleService.getLampStateCharacteristic();
    
    // Action for long press (e.g., turn on/off)
    if (ledController.state()) {
        ledController.turnOff();
        if (pLampStateCharacteristic) {
            pLampStateCharacteristic->setValue("OFF");
        }
    } else {
        ledController.turnOn();
        if (pLampStateCharacteristic) {
            pLampStateCharacteristic->setValue("ON");
        }
    }
    pLampStateCharacteristic->notify();
}