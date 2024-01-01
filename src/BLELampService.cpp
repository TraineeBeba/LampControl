#include "BLELampService.h"

void BLELampService::setup() {  
    BLEDevice::init("LampControl");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Lamp State Characteristic
    pLampStateCharacteristic = pService->createCharacteristic(
        LAMP_STATE_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
    );
    pLampStateCharacteristic->setCallbacks(new LampStateCallback(&ledController));
    pLampStateCharacteristic->addDescriptor(new BLE2902());
    pLampStateCharacteristic->setNotifyProperty(true);

    // Brightness Characteristic
    pBrightnessCharacteristic = pService->createCharacteristic(
        BRIGHTNESS_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
    );
    pBrightnessCharacteristic->setCallbacks(new BrightnessCallback(&ledController));
    pBrightnessCharacteristic->addDescriptor(new BLE2902());
    pBrightnessCharacteristic->setNotifyProperty(true);

    // Mode Characteristic
    pModeCharacteristic = pService->createCharacteristic(
        MODE_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
    );
    pModeCharacteristic->setCallbacks(new ModeCallback(&ledController));
    pModeCharacteristic->addDescriptor(new BLE2902());
    pModeCharacteristic->setNotifyProperty(true);

    BLECharacteristic *pColorCharacteristic;
    pColorCharacteristic = pService->createCharacteristic(
        COLOR_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
    );
    pColorCharacteristic->setCallbacks(new ColorCallback(&ledController));
    pColorCharacteristic->addDescriptor(new BLE2902());

    pLampStateCharacteristic->setValue("OFF"); // Assuming the lamp is off initially

    String initialBrightness = String(ledController.getBrightness());
    pBrightnessCharacteristic->setValue(initialBrightness.c_str());

    String initialMode = String(ledController.getMode());
    pModeCharacteristic->setValue(initialMode.c_str());

    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->setAppearance(0x1234); // Example appearance
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // Suggested value for connection parameters
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    pAdvertising->start();
}


bool BLELampService::isAnyClientConnected(){
    u_int32_t connectedCount = pServer->getConnectedCount();
    // print_log("Connected cleints count: ");
    // print_log_ln(connectedCount);
    return connectedCount > 0;               
}