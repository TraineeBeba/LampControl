#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <FastLED.h>
#include <Preferences.h>
#include "Logging.h"
#include <vector>
#include <map>

extern const char* MODE_KEY;
extern const char* BRIGHTNESS_KEY;
extern const char* PREFS_NAMESPACE;
extern const char* TEST_KEY;
extern Preferences preferences;

#define BUTTON_PIN  2  // Button pin
#define NUM_LEDS    20 // Number of LEDs
#define LED_PIN     4  // Example pin number

struct RGBColor {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

enum LEDMode { MODE0, MODE1, MODE2 };

const int brightnessSteps[4] = {25, 100, 175, 250};

RGBColor toRGBColor(const CRGB& color);

class LEDController {
public:
    LEDController(uint8_t pin, uint16_t numLeds);

    void update();
    void saveColorData();
    void loadColorData();
    void setActiveColor(int activeColorIndex, const RGBColor& newColor);
    std::vector<RGBColor> getModeColors(int mode);
    void setModeColors(int mode, const std::vector<RGBColor>& colors);
    bool state();
    int getMode();
    void setMode(int newMode);
    int getBrightness();
    void setBrightness(int newBrightness) ;
    void turnOn();
    void turnOff();
    void nextMode();
    void initMode(int mode);
    void initBrightness(int newBrightness);


private:
    std::map<int, std::vector<RGBColor>> modeColorsMap; // Map to store colors for each mode
    bool needUpdate;
    uint8_t pin;
    uint16_t numLeds;
    CRGB leds[NUM_LEDS];
    int currentMode;
    int brightness;
    bool isOn;
    unsigned long lastChangeTime;
    bool isRed;
    int colorIndex;
    unsigned long period = 1000; // Time interval for mode changes

    void updateLEDs();
    void mode0();
    void mode1(unsigned long currentTime);
    void mode2(unsigned long currentTime);

};

#endif