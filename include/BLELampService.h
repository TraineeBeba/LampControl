#ifndef BLE_LAMP_SERVICE_H
#define BLE_LAMP_SERVICE_H

#include <BLEDevice.h>
#include <BLE2902.h>
#include "LEDController.h"
#include "BLECallbacks.h"
#include "Logging.h"

class BLELampService {
public:
    BLELampService(LEDController& ledController) : ledController(ledController) {
    }

    void setup();
    bool isAnyClientConnected();

    BLECharacteristic* getLampStateCharacteristic() {
        return pLampStateCharacteristic;
    }

    BLECharacteristic* getBrightnessCharacteristic() {
        return pBrightnessCharacteristic;
    }

    BLECharacteristic* getModeCharacteristic() {
        return pModeCharacteristic;
    }

    private:
    LEDController& ledController;
    BLEServer* pServer;
    BLECharacteristic *pLampStateCharacteristic;
    BLECharacteristic *pBrightnessCharacteristic;
    BLECharacteristic *pModeCharacteristic;

    const char* SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
    const char* LAMP_STATE_CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
    const char* BRIGHTNESS_CHARACTERISTIC_UUID = "c8caddf7-38c7-4b6e-891e-5550e4776a79";
    const char* MODE_CHARACTERISTIC_UUID = "1f9df20a-949d-4278-8051-72a01b6a7ca3";
    const char* COLOR_CHARACTERISTIC_UUID = "6421350a-fe82-480f-af3f-5c24925ec0ae";
};

#endif