#include <Arduino.h>
#include "Button.h"
#include "BLELampService.h"
#include "Logging.h"
#include "driver/rtc_io.h"
#include "BluetoothSerial.h"
// const char* PREFS_NAMESPACE = "lampSettings";

LEDController ledController(LED_PIN, NUM_LEDS);
BLELampService bleService(ledController);
Button button(BUTTON_PIN, ledController, bleService);

hw_timer_t *My_timer = NULL;

void IRAM_ATTR onTimer(){
  if(!bleService.isAnyClientConnected() && ledController.state() == false){
    // print_log_ln("Sleep");
    // timerDetachInterrupt(My_timer);
    rtc_gpio_pullup_dis(GPIO_NUM_13);
    rtc_gpio_pulldown_en(GPIO_NUM_13);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_13,1); //1 = High, 0 = Low
    esp_deep_sleep_start();
  }
}

void setup() {
    sleep_timeout_start_timestamp = millis();
    Serial.begin(115200);
    delay(1000);
    print_log_ln("Esp32 start");

    My_timer = timerBegin(0, 80, true);
    timerAttachInterrupt(My_timer, &onTimer, true);
    timerAlarmWrite(My_timer, 10000000, true);
    timerAlarmEnable(My_timer);


    ledController.loadColorData();

    preferences.begin(PREFS_NAMESPACE, true);
    int savedMode = preferences.getInt(MODE_KEY, 0); // Default to mode 0
    int savedBrightness = preferences.getInt(BRIGHTNESS_KEY, 48); // Default brightness
    preferences.end();

    print_log_ln("savedMode: ");
    print_log_ln(savedMode);
    print_log_ln("savedBrightness");
    print_log_ln(savedBrightness);

    ledController.turnOn();
    ledController.initMode(savedMode);
    ledController.initBrightness(savedBrightness);

    ledController.loadColorData();
    ledController.saveColorData();

    bleService.setup();
}

void loop() {
  button.update();
  ledController.update();
}
