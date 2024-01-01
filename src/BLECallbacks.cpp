#include "BLECallbacks.h"

unsigned int sleep_timeout_start_timestamp;

void printColorData(const std::vector<uint8_t>& colorData) {
    for (uint8_t byte : colorData) {
        print_log(byte);
        print_log(" ");
    }
    print_log_ln(); // Add a newline to separate from other output
}


void MyServerCallbacks::onConnect(BLEServer* pServer) {
    print_log_ln("Connected");
}

void MyServerCallbacks::onDisconnect(BLEServer* pServer) {
    sleep_timeout_start_timestamp = millis();
    print_log_ln("Disconnected");
    BLEDevice::startAdvertising();
}


void LampStateCallback::onRead(BLECharacteristic* pCharacteristic) {
        print_log("Lamp State Read: ");
        std::string value = ledController->state() ? "ON" : "OFF";
        print_log_ln(value.c_str());
        pCharacteristic->setValue(value);
}
    

void LampStateCallback::onWrite(BLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    print_log("Lamp State Written: ");
    print_log_ln(value.c_str());

    if (value == "ON") {
        ledController->turnOn();
    } else if (value == "OFF") {
        ledController->turnOff();
    }
    pCharacteristic->notify();
}

void BrightnessCallback::onRead(BLECharacteristic* pCharacteristic) {
    int brightness = ledController->getBrightness();

    // Ensure brightness is in the range 0-255
    if (brightness < 0) brightness = 0;
    if (brightness > 255) brightness = 255;

    uint8_t brightnessByte = static_cast<uint8_t>(brightness);
    print_log("brightnessByte: ");
    print_log_ln(brightnessByte);
    
    pCharacteristic->setValue(&brightnessByte, sizeof(brightnessByte));
}


void BrightnessCallback::onWrite(BLECharacteristic* pCharacteristic) {
    if (ledController->state()) {
        std::string value = pCharacteristic->getValue();
        print_log("Brightness Written: ");

        if (!value.empty()) {
            // Interpret the first byte of the value as the brightness level
            uint8_t newBrightness = static_cast<uint8_t>(value[0]);
            print_log_ln((int)newBrightness);

            ledController->setBrightness(newBrightness);
            pCharacteristic->notify();
        } else {
            print_log_ln("Invalid brightness value received");
        }
    }
}

void ModeCallback::onRead(BLECharacteristic* pCharacteristic) {
    int mode = ledController->getMode();
    uint8_t modeByte = static_cast<uint8_t>(mode);
    print_log("modeByte: ");
    print_log_ln(modeByte);
    pCharacteristic->setValue(&modeByte, sizeof(modeByte));
}

void ModeCallback::onWrite(BLECharacteristic* pCharacteristic) {
    if (ledController->state()) {
        print_log("Mode Written: ");
        std::string value = pCharacteristic->getValue();
        if (!value.empty()) {
            // Interpret the first byte of the value as the brightness level
            uint8_t newMode = static_cast<uint8_t>(value[0]);
            print_log_ln((int)newMode);

            ledController->setMode(newMode);
            pCharacteristic->notify();
        } else {
            print_log_ln("Invalid brightness value received");
        }
    }
}

void ColorCallback::onRead(BLECharacteristic* pCharacteristic) {
    // Number of modes
    const int modeCount = 3; // Assuming there are 3 modes (MODE0, MODE1, MODE2)

    // Prepare the data in the required format
    std::vector<uint8_t> colorData;
    colorData.push_back(static_cast<uint8_t>(modeCount));

    // Add color data for each mode
    for (int modeIndex = 0; modeIndex < modeCount; ++modeIndex) {
        // Add the current mode index
        colorData.push_back(static_cast<uint8_t>(modeIndex));

        // Get active colors for each mode
        std::vector<RGBColor> currentColors = ledController->getModeColors(modeIndex);
        int colorCount = currentColors.size();

        // Add count of active colors for the mode
        colorData.push_back(static_cast<uint8_t>(colorCount));

        // Add RGB values for active colors
        for (const RGBColor& color : currentColors) {
            colorData.push_back(color.r);
            colorData.push_back(color.g);
            colorData.push_back(color.b);
        }
    }
    
    printColorData(colorData);
    // Set the value of the characteristic
    pCharacteristic->setValue(&colorData[0], colorData.size());
    print_log_ln("Color Data Read");
}

void ColorCallback::onWrite(BLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();

    // Print the entire string value for initial check
    print_log("Received Data: ");
    for (char c : value) {
        print_log(static_cast<unsigned>(c));
        print_log(" ");
    }
    print_log_ln();

    if (value.size() >= 5) { // Ensure there's at least mode, active color index, and one RGB set
        int mode = value[0];
        int activeColorIndex = value[1];

        // Print received mode and active color index
        print_log("Mode: ");
        print_log_ln(mode);
        print_log("Active Color Index: ");
        print_log_ln(activeColorIndex);

        // Extract RGB values
        uint8_t red = value[2];
        uint8_t green = value[3];
        uint8_t blue = value[4];

        // Print extracted RGB values
        print_log("Color: R=");
        print_log(static_cast<unsigned>(red));
        print_log(", G=");
        print_log(static_cast<unsigned>(green));
        print_log(", B=");
        print_log_ln(static_cast<unsigned>(blue));

        // Assuming setModeColors method exists in LEDController to update the mode and color
        RGBColor newColor = {red, green, blue};
        ledController->setActiveColor(activeColorIndex, newColor);
    }
}