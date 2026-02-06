#include <SPI.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include <daly-bms-uart.h>
#include <string.h>

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

// SPI pins (ESP32-S3)
#define TFT_MOSI 11   // SDI (MOSI)
#define TFT_MISO 13   // SDO (optional, can be unused)
#define TFT_SCK  12   // SCK

// Control pins
#define TFT_CS   5
#define TFT_DC   6
#define TFT_RST  4    // or -1 if tied to ESP reset

// Backlight
#define TFT_LED  15   // optional PWM later

// Pedal pin definitions connecting to ESP32S3
#define RIGHT_PEDAL_PIN 7
#define LEFT_PEDAL_PIN 8

// Button pin definitions connecting to ESP32S3
#define RIGHT_BUTTON_PIN 9
#define LEFT_BUTTON_PIN 10

#define RX_PIN 44
#define TX_PIN 43

// 6 Channel relay pin definitions, connects to each 3 speed
#define LEFT_THREE_SPEED_IN1_BLUE 16
#define LEFT_THREE_SPEED_IN2_YELLOW 25
#define RIGHT_THREE_SPEED_IN3_BLUE 14
#define RIGHT_THREE_SPEED_IN4_YELLOW 24
#define LEFT_REVERSE_IN5_BROWN 26
#define RIGHT_REVERSE_IN6_BROWN 29
#define HALL_EFFECT 27 //unused
#define REVERSE_LEFT_MOTOR -1 //configure!!
#define REVERSE_RIGHT_MOTOR -1 //configure!!

#define BMS_SERIAL Serial1

// Create display object
Adafruit_ILI9341 display(TFT_CS, TFT_DC, TFT_RST);

// Create Daly BMS object
Daly_BMS_UART bms(BMS_SERIAL);

// Booleans
bool reverseUpdate = false; //signifies update in reversal of gokart
bool gearChange = true; //signifies update in gear change
bool reverse = false; //signifies gokart is in reverse

// Variables
int currentScreen = 1; // value from 0-1
int currentSpeed = 0; // This will be replaced with actual speed from ESC later
int currentGear = 2; // values from 0-2 according to gearInfo typedef


void setup() {

  digitalWrite(LEFT_REVERSE_IN5_BROWN, HIGH);
  digitalWrite(RIGHT_REVERSE_IN6_BROWN, LOW);

  pinMode(LEFT_THREE_SPEED_IN1_BLUE, OUTPUT);
  pinMode(LEFT_THREE_SPEED_IN2_YELLOW, OUTPUT);
  pinMode(RIGHT_THREE_SPEED_IN3_BLUE, OUTPUT);
  pinMode(RIGHT_THREE_SPEED_IN4_YELLOW, OUTPUT);
  pinMode(LEFT_REVERSE_IN5_BROWN, OUTPUT);
  pinMode(RIGHT_REVERSE_IN6_BROWN, OUTPUT);

  // Initialize serial & bms serial and bms
  BMS_SERIAL.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.begin(9600);
  bms.Init();

  SPI.begin(TFT_SCK, TFT_MISO, TFT_MOSI);
  display.begin();
  display.setRotation(1); // 0–3 (landscape = 1 or 3)
  display.fillScreen(ILI9341_BLACK);

  //basic setup
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(ILI9341_WHITE);
  display.setTextWrap(false);

  //pinmode setup
  pinMode(LEFT_PEDAL_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PEDAL_PIN, INPUT_PULLUP);
  pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);
}


// Basic function that updates gear value on LCD screen
void displayGear() {
  if (gearChange) {
    display.fillRect(0, 0, 240, 40, ILI9341_BLACK);

    display.setCursor(20, 10);
    display.setTextSize(2);
    display.setTextColor(ILI9341_WHITE);
    display.print("Gear: ");
    display.print(currentGear + 1);

    gearChange = false;
  }
}

// Basic function that updates speed value on LCD screen
void displaySpeed() {
  display.fillRect(50,32,70,7,ILI9341_BLACK);
  display.setCursor(50,32);
  display.print(currentSpeed);
  display.setTextSize(0.25);
  display.print("km/h");
  display.setTextSize(1);
}

// Basic function that clears LCD screen of all that is currently being displayed
void wipeScreen() {
  display.fillScreen(ILI9341_BLACK);
}

// Function that checks for pedal inputs and updates gear + reversal accordingly
void checkPedalInputs() {
  // Reading for pedal presses for gear change + changing reversal of direction
  if(!reverse) {
    if (digitalRead(RIGHT_PEDAL_PIN) == 0){
      if (currentGear < 2){
        gearChange = true;
        currentGear++;
      }
    } else if (digitalRead(LEFT_PEDAL_PIN) == 0) {
      if (currentGear > 0){
        gearChange = true;
        currentGear--;
      } else if(currentGear == 0) {
        reverseUpdate = true;
        reverse = true;
        delay(300);
      }
    } 
  } else if (reverse) {
    if (digitalRead(RIGHT_PEDAL_PIN) == 0){
      if (currentGear > 0){
        gearChange = true;
        currentGear--;
      } else if(currentGear == 0) {
        reverseUpdate = true;
        reverse = false;
      }
    } else if (digitalRead(LEFT_PEDAL_PIN) == 0) {
      if (currentGear < 2){
        gearChange = true;
        currentGear++;
      }
    }
  }
}


// Checks if gear must be updated, and communicates to relay accordingly.
void changeGear() {
  if(gearChange){
    if(currentGear == 2) { 
      digitalWrite(LEFT_THREE_SPEED_IN1_BLUE, LOW);
      digitalWrite(LEFT_THREE_SPEED_IN2_YELLOW, LOW);
      digitalWrite(RIGHT_THREE_SPEED_IN3_BLUE, LOW);
      digitalWrite(RIGHT_THREE_SPEED_IN4_YELLOW, LOW);
    } else if(currentGear == 1)  {
      digitalWrite(LEFT_THREE_SPEED_IN1_BLUE, HIGH);
      digitalWrite(LEFT_THREE_SPEED_IN2_YELLOW, LOW);
      digitalWrite(RIGHT_THREE_SPEED_IN3_BLUE, HIGH);
      digitalWrite(RIGHT_THREE_SPEED_IN4_YELLOW, LOW);
    } else if(currentGear == 0) {
      digitalWrite(LEFT_THREE_SPEED_IN1_BLUE, LOW);
      digitalWrite(LEFT_THREE_SPEED_IN2_YELLOW, HIGH);
      digitalWrite(RIGHT_THREE_SPEED_IN3_BLUE, LOW);
      digitalWrite(RIGHT_THREE_SPEED_IN4_YELLOW, HIGH);
    }
  }
}

void changeReverse() {
  if(reverseUpdate){
    if(reverse) { 
      digitalWrite(LEFT_REVERSE_IN5_BROWN, LOW);
      digitalWrite(RIGHT_REVERSE_IN6_BROWN, HIGH);
    } else if(!reverse)  {
      digitalWrite(LEFT_REVERSE_IN5_BROWN, HIGH);
      digitalWrite(RIGHT_REVERSE_IN6_BROWN, LOW);
    }
  }
}

// Reads button press inputs and updates currentScreen variable
void updateScreen() {
  if(digitalRead(LEFT_BUTTON_PIN) == LOW) {
    if(currentScreen > 0) {
      currentScreen -= 1;
    }
  } else if (digitalRead(RIGHT_BUTTON_PIN) == LOW) {
    if(currentScreen < 1) {
      currentScreen += 1;
    }
  } 
}

void loop() {
  // Check Pedal Inputs
  checkPedalInputs();
  // If gear is changed, communicate with 2 channel relay.
  if (gearChange) {
  changeGear();
  gearChange = false;
  delay(300);
}
  changeReverse();
  // Reading button presses for screen changes
  updateScreen();
  // Update BMS
  if(currentScreen == 0) {
    wipeScreen();

    display.setCursor(20, 20);
    display.print("Screen 1");
  } else if(currentScreen == 1) {
    //displays gear
    wipeScreen();
    //updateGear();
    displayGear();
  }
  reverseUpdate = false;
  delay(50);
}
