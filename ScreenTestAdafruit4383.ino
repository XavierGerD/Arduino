
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#if defined(ARDUINO_FEATHER_ESP32) // Feather Huzzah32
#define TFT_CS         14
#define TFT_RST        15
#define TFT_DC         32

#elif defined(ESP8266)
#define TFT_CS         4
#define TFT_RST        16
#define TFT_DC         5

#else
// For the breakout board, you can use any 2 or 3 pins.
// These pins will also work for the 1.8" TFT shield.
#define TFT_CS        18
#define TFT_RST        19 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         20
#endif

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

byte currentMenu = 0;
byte selectedItemBackground = 0x1335;
byte menuTextSize = 3;
byte menuItemOffset = 45;

void changeSetting(byte setting, byte value) {
  Serial.println("Changing setting");
}

void navigateToSubmenu(byte target) {
  currentMenu = target;
  arraySize = sizeof(allMenus[currentMenu]);
  drawMenu(ST77XX_WHITE);
}

void changeScale(int scale) {
  Serial.println("Changing scale");
}

class Menu {
  public:
    const char* menuName;
    byte submenuTarget;
    void(*onAction)(byte target);
    Menu(const char* menuName, byte submenuTarget, void(*onAction)(byte target)) {
      this -> menuName = menuName;
      this -> submenuTarget = submenuTarget;
      this -> onAction = onAction;
    }

};

Menu settingsMenuItems[2] = {Menu("Scale", 1, navigateToSubmenu), Menu("Sensor Mode", 2, navigateToSubmenu)};
Menu sensorModeMenuItems[3] = {Menu("Velocity", 0, changeSetting), Menu("Mod Wheel", 0, changeSetting), Menu("Octave Shift", 0, changeSetting)};
Menu scaleMenuItems[8] = {
  Menu("Chromatic", 0, changeScale),
  Menu("Major", 0, changeScale),
  Menu("Minor", 0, changeScale),
  Menu("Pentatonic", 0, changeScale),
  Menu("Lydian", 0, changeScale),
  Menu("Mixolydian", 0, changeScale),
  Menu("Aeolian", 0, changeScale),
  Menu("Whole Tone", 0, changeScale)
};
Menu* allMenus[3] = {settingsMenuItems, sensorModeMenuItems, scaleMenuItems};

byte arraySize = sizeof(allMenus[currentMenu]);
byte firstMenuItemIndex = 0;
byte currentSelectorPosition = 0;

boolean lastButtonState1 = false;
boolean newButtonState1 = false;

boolean lastButtonState2 = false;
boolean newButtonState2 = false;

boolean lastButtonState3 = false;
boolean newButtonState3 = false;

void setup() {
  Serial.begin(9600);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  tft.init(135, 240);
  tft.setRotation(1);
  drawMenu(ST77XX_WHITE);
}

void drawSelector() {
  tft.fillRect(0, currentSelectorPosition, 240, 45, selectedItemBackground);
  tft.drawRect(0, currentSelectorPosition, 240, 45, selectedItemBackground);
}

void drawBackground(uint16_t color) {
  tft.fillRect(-1, -1, 242, 137, color);
  tft.drawRect(-1, -1, 242, 137, color);
}

void drawMenu(uint16_t color) {
  drawBackground(ST77XX_BLACK);
  drawSelector();
  byte offsetCounter = 0;

  byte startingPoint = firstMenuItemIndex + 2 <= arraySize ?  firstMenuItemIndex  : arraySize - 3;

  for (byte i = startingPoint; i < arraySize; i++) {
    tft.setCursor(5, offsetCounter * menuItemOffset + 10);
    tft.setTextColor(color);
    tft.setTextSize(menuTextSize);
    tft.print(allMenus[currentMenu][i].menuName);
    offsetCounter++;
  }
}

void loop() {
  newButtonState1 = digitalRead(5);
  if (newButtonState1 != lastButtonState1) {
    if (newButtonState1) {
      if (currentSelectorPosition == menuItemOffset * 2 && firstMenuItemIndex < arraySize - 3) {
        firstMenuItemIndex++;
      }

      if (currentSelectorPosition < menuItemOffset * 2) {
        currentSelectorPosition += menuItemOffset;
      }

      drawMenu(ST77XX_WHITE);
    }

    lastButtonState1 = newButtonState1;
  }

  newButtonState2 = digitalRead(4);
  if (newButtonState2 != lastButtonState2) {
    if (newButtonState2) {
      Menu currentItem = allMenus[currentMenu][firstMenuItemIndex + 1];
      currentItem.onAction(currentItem.submenuTarget);
    }

    lastButtonState2 = newButtonState2;
  }


  newButtonState3 = digitalRead(3);
  if (newButtonState3 != lastButtonState3) {
    if (newButtonState3) {
      if (currentSelectorPosition == 0 && firstMenuItemIndex > 0) {
        firstMenuItemIndex--;
      }

      if (currentSelectorPosition > 0) {
        currentSelectorPosition -= menuItemOffset;
      }

      drawMenu(ST77XX_WHITE);
    }
    lastButtonState3 = newButtonState3;
  }
}
