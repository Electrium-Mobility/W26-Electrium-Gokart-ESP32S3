#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Display resolution
#define SCREEN_WIDTH 128 // adjust if your display is 128x64, 128x32, or 240x240
#define SCREEN_HEIGHT 64

// Reset pin (not used for I2C)
#define OLED_RESET -1

// I2C pins for ESP32-S3
#define SDA_PIN 21
#define SCL_PIN 47
#define rightPedal 7
#define leftPedal 8
#define rightButton 9
#define leftButton 10

// Create display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Constants
bool gearChange = true;

// Variables
int currentScreen = 2; // value from 1-3
int currentSpeed = 0; // This will be replaced with actual speed from ESC later
int currentGear = 1;

void setup() {
  // Initialize I2C
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 0x3C is the common I2C address
    Serial.println(F("SSD1306 allocation failed"));
    while(true);
  }

  display.clearDisplay();
  display.setTextSize(0.75);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(65,5);
  display.setTextWrap(false);
  display.print("Gear: ");
  display.display();

  pinMode(leftPedal, INPUT_PULLUP);
  pinMode(rightPedal, INPUT_PULLUP);
  pinMode(leftButton, INPUT_PULLUP);
  pinMode(rightButton, INPUT_PULLUP);
}

void updateGear() {
 // Later will fetch gear from ESP input from pedals
  currentGear += 1;
  if(currentGear > 3) {
    currentGear = 1;
  }

}

void updateSpeed() {
  // Later this function will fetch the speed from the ESC
  currentSpeed += 5; // debugging, changing speed
  if(currentSpeed > 100) {
    currentSpeed = 25;
  }
}

void displayGear() {
  if (gearChange){
    display.display();
    display.setCursor(95,5);
    display.fillRect(95,5,5,7,SSD1306_BLACK);
    display.print(currentGear);
  } 
}

void displaySpeed() {
  display.display();
  display.fillRect(50,32,70,7,SSD1306_BLACK);
  display.setCursor(50,32);
  display.print(currentSpeed);
  display.setTextSize(0.25);
  display.print("km/h");
  display.setTextSize(0.75);
}

void wipeScreen() {
  display.display();
  display.fillRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,SSD1306_BLACK);
}

void loop() {

  if (digitalRead(rightPedal) == LOW){
    if (currentGear < 3){
      gearChange = true;
      currentGear++;
    }
  }
  
  if (digitalRead(leftPedal) == LOW) {
    if (currentGear > 1){
      gearChange = true;
      currentGear--;
    }
  }

  if(digitalRead(rightButton == LOW)) {
    if(currentScreen < 3) {
      currentScreen += 1;
    }
  }

  if(digitalRead(leftButton == LOW)) {
    if(currentScreen > 1) {
      currentScreen -= 1;
    }
  }

  if(currentScreen == 1) {
    
  } else if(currentScreen == 2) {
    wipeScreen();
    updateGear();
    displayGear();
  } else if(currentScreen == 3) {

  }
  display.display();
  delay(200);
}
