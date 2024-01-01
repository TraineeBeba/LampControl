#ifndef BLE_CALLBACKS_H
#define BLE_CALLBACKS_H

#include "LEDController.h"
#include "Logging.h"
#include <BLEDevice.h>

extern unsigned int sleep_timeout_start_timestamp;

void printColorData(const std::vector<uint8_t>& colorData);

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) override;
  void onDisconnect(BLEServer* pServer) override;
};

class LampStateCallback : public BLECharacteristicCallbacks {
    LEDController* ledController;

public:
    LampStateCallback(LEDController* ledController) : ledController(ledController) {}
    void onRead(BLECharacteristic* pCharacteristic) override;
    void onWrite(BLECharacteristic* pCharacteristic) override;
};

class BrightnessCallback : public BLECharacteristicCallbacks {
    LEDController* ledController;

public:
    BrightnessCallback(LEDController* ledController): ledController(ledController) {}
    void onRead(BLECharacteristic* pCharacteristic) override;
    void onWrite(BLECharacteristic* pCharacteristic) override;
};

class ModeCallback : public BLECharacteristicCallbacks {
    LEDController* ledController;

public:
    ModeCallback(LEDController* ledController): ledController(ledController) {}
    void onRead(BLECharacteristic* pCharacteristic) override;
    void onWrite(BLECharacteristic* pCharacteristic) override;
};

class ColorCallback : public BLECharacteristicCallbacks {
    LEDController* ledController;

public:
    ColorCallback(LEDController* ledController): ledController(ledController) {}
    void onRead(BLECharacteristic* pCharacteristic) override;
    void onWrite(BLECharacteristic* pCharacteristic) override;
};

#endif
