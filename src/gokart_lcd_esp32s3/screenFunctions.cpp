#include "screenFunctions.h"
#include <Wire.h>

/* Define display ONCE */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initDisplay() {
  Wire.begin(SDA_PIN, SCL_PIN); 

  display.begin();
  display.setRotation(1); // 0–3 (landscape = 1 or 3)
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);
  display.display();

  /* Initialize Display */
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  
}

void wipeScreen() {
  display.clearDisplay();
  display.display();
}

void displayGear() {
  if (gearChange) {
    wipeScreen();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.print("Gear: ");
    display.print(currentGear + 1);
    display.display();
    gearChange = false;
  }
}

void displaySpeed() {
  wipeScreen();
  display.setCursor(0, 32);
  display.setTextSize(1);
  display.print(currentSpeed);
  display.print(" km/h");
  display.display();
}
