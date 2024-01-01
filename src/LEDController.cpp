#include "LEDController.h"

const char* MODE_KEY = "mode";
const char* BRIGHTNESS_KEY = "brightness";
const char* PREFS_NAMESPACE = "test";
// const char* PREFS_NAMESPACE = "lampSettings";
const char* TEST_KEY = "testKey";
Preferences preferences;

RGBColor toRGBColor(const CRGB& color) {
    return RGBColor{color.r, color.g, color.b};
}

    LEDController::LEDController(uint8_t pin, uint16_t numLeds) 
    : pin(pin), numLeds(numLeds), currentMode(0), brightness(48), isOn(false), lastChangeTime(millis()) {
        FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, numLeds);
        FastLED.setBrightness(brightness);

            // Initialize modeColorsMap with default colors for each mode
        modeColorsMap[MODE0] = {toRGBColor(CRGB::Green)}; // Example colors for MODE0
        modeColorsMap[MODE1] = {
            toRGBColor(CRGB::Yellow),
            toRGBColor(CRGB::Purple),
            toRGBColor(CRGB::Orange),
            toRGBColor(CRGB::White),
        }; // Example colors for MODE1
        modeColorsMap[MODE2] = {toRGBColor(CRGB::Red)}; // Example colors for MODE2
    }

    void LEDController::update() {
        if (isOn) {
            unsigned long currentTime = millis();
            switch (currentMode) {
                case MODE0:
                    mode0();
                    break;
                case MODE1:
                    mode1(currentTime);  // Continuous update
                    break;
                case MODE2:
                    mode2(currentTime);  // Continuous update
                    break;
            }
        } 
    }

    // Example of saving color data as binary
    void LEDController::saveColorData() {
      preferences.begin(PREFS_NAMESPACE, false);
      for (const auto& entry : modeColorsMap) {
          int mode = entry.first;
          const auto& colors = entry.second;
          std::string key = "mode" + std::to_string(mode) + "_colors";
          size_t dataSize = colors.size() * sizeof(RGBColor);
          
          // Remove const qualifier
          const RGBColor* constData = colors.data();
          uint8_t* data = reinterpret_cast<uint8_t*>(const_cast<RGBColor*>(constData));

          preferences.putBytes(key.c_str(), data, dataSize);
      }
      preferences.end();
  }


    // Example of loading color data as binary
    void LEDController::loadColorData() {
        preferences.begin(PREFS_NAMESPACE, true);
        for (int mode = 0; mode < 3; ++mode) {
            std::string key = "mode" + std::to_string(mode) + "_colors";
            // Get the size of the stored data
            size_t dataSize = preferences.getBytesLength(key.c_str());
            if (dataSize > 0) {
                std::vector<RGBColor> colors(dataSize / sizeof(RGBColor));
                preferences.getBytes(key.c_str(), colors.data(), dataSize);
                modeColorsMap[mode] = colors;
            }
        }
        preferences.end();
    }



    void LEDController::setActiveColor(int activeColorIndex, const RGBColor& newColor) {
        if (currentMode >= 0 && currentMode <= 2) { // Assuming there are 3 modes (0, 1, 2)
            std::vector<RGBColor>& colors = modeColorsMap[currentMode];
            

            // Check if the index is within the range of available colors
            if (activeColorIndex >= 0 && activeColorIndex < colors.size()) {
                colors[activeColorIndex] = newColor; // Update the color at the specified index
                saveColorData();

                // Update LED if the device is on
                // if (isOn) {
                //     updateLEDs();
                // }
            }
        }
    }

  
    std::vector<RGBColor> LEDController::getModeColors(int mode) {
        std::vector<RGBColor> colors;

        // Check if the current mode has associated colors in the map
        auto it = modeColorsMap.find(mode);
        if (it != modeColorsMap.end()) {
            // Return the colors for the specified mode
            colors = it->second;
        } else {
            // If no specific colors are set for the specified mode, return a default value
            // Example: return a vector of 'numLeds' elements, all set to a default color like CRGB::Black
            RGBColor defaultColor = {0, 0, 0}; // Black color
            colors.assign(numLeds, defaultColor);
        }

        return colors;
    }

    void LEDController::setModeColors(int mode, const std::vector<RGBColor>& colors) {
        modeColorsMap[mode] = colors;
        if (currentMode == mode) {
            updateLEDs(); // Update LEDs if the current mode is being modified
        }
    }

    bool LEDController::state(){
      return isOn;
    }

    int LEDController::getMode() {
      return currentMode;
    }

    int LEDController::getBrightness() {
        return brightness;
    }

    void LEDController::turnOn() {
      isOn = true; 
    }

    void LEDController::turnOff() { 
      isOn = false; 
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show();
    }

    void LEDController::nextMode() {
        int newMode = (currentMode + 1) % 3;
        setMode(newMode);
    }

    void LEDController::setMode(int newMode) {
      if (isOn) {
        currentMode = newMode % 3; // Ensure the mode is within the range
        // updateLEDs(); // Update LEDs for the new mode
        // if(currentMode == MODE0) mode0();

        preferences.begin(PREFS_NAMESPACE, false);
        preferences.putInt(MODE_KEY, currentMode);
        preferences.end();
      }
    }

    void LEDController::setBrightness(int newBrightness) {
      if (isOn){
        brightness = newBrightness;
        FastLED.setBrightness(brightness);
        FastLED.show();

        preferences.begin(PREFS_NAMESPACE, false);
        preferences.putInt(BRIGHTNESS_KEY, brightness);
        preferences.end();
      }
    }

    void LEDController::initMode(int mode){
        currentMode = mode % 3; // Ensure the mode is within the range
        setMode(currentMode);
        print_log("Init mode to: ");
        print_log_ln(currentMode);
    }

    void LEDController::initBrightness(int newBrightness){
        brightness = newBrightness;
        print_log("Init brightness to: ");
        print_log_ln(brightness);
        FastLED.setBrightness(brightness);
        FastLED.show();
    }

    void LEDController::mode0() {
        auto& colors = modeColorsMap[currentMode];     
        CRGB color = colors.empty() ? CRGB::Black : CRGB(colors[0].r, colors[0].g, colors[0].b);
        fill_solid(leds, numLeds, color);
        FastLED.show();
    }


    void LEDController::mode1(unsigned long currentTime) {
       if (currentTime - lastChangeTime >= period) {
          lastChangeTime = currentTime;
          auto& colors = modeColorsMap[currentMode];
          colorIndex = (colorIndex + 1) % colors.size();
          fill_solid(leds, numLeds, CRGB(colors[colorIndex].r, colors[colorIndex].g, colors[colorIndex].b));
          FastLED.show();
      }
    }

    void LEDController::mode2(unsigned long currentTime) {
      if (currentTime - lastChangeTime >= period) {
          lastChangeTime = currentTime;

          // Get the colors for the current mode
          auto& colors = modeColorsMap[currentMode];

          // Alternate between the color and black
          if (!colors.empty()) {
              // Toggle the state between the color and black
              isRed = !isRed;

              if (isRed) {
                  // Apply the next color in the sequence
                  colorIndex = (colorIndex + 1) % colors.size();
                  RGBColor& color = colors[colorIndex];
                  fill_solid(leds, numLeds, CRGB(color.r, color.g, color.b));
              } else {
                  // Apply black
                  fill_solid(leds, numLeds, CRGB::Black);
              }
          } else {
              // Default behavior if no colors are defined
              fill_solid(leds, numLeds, CRGB::Black);
          }

          FastLED.show();
      }
  }
