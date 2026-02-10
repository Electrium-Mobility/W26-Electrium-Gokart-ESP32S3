#include <SPI.h>
#include <Adafruit_GFC.h>
#include <Adafruit_SSD1306.h>
#include <string.h>
#include "screenFunctions.h"

//#include <daly-bms-uart.h> 

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64

#define SDA_PIN 21
#define SCL_PIN 47

/* Leave undefined for now*/
#define OLED_RESET -1

/* Pedal pin definitions connecting to ESP32S3 */
#define RIGHT_PEDAL_PIN 7
#define LEFT_PEDAL_PIN 8

/* Button pin definitions connecting to ESP32S3 */
#define RIGHT_BUTTON_PIN 9
#define LEFT_BUTTON_PIN 10

/* RX and TX pin definitions on ESP32S3 , labelled as such on the board*/
#define RX_PIN 44
#define TX_PIN 43

/* 6 Channel relay pin definitions, connects to each 3 speed */
#define LEFT_THREE_SPEED_IN1_BLUE 16
#define LEFT_THREE_SPEED_IN2_YELLOW 25
#define RIGHT_THREE_SPEED_IN3_BLUE 14
#define RIGHT_THREE_SPEED_IN4_YELLOW 24
#define LEFT_REVERSE_IN5_BROWN 26
#define RIGHT_REVERSE_IN6_BROWN 29

#define HALL_EFFECT 27 /* unused */
#define REVERSE_LEFT_MOTOR -1 /* configure!! */
#define REVERSE_RIGHT_MOTOR -1 /* configure!! */

/* #define BMS_SERIAL Serial1 */

/* Create display object */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/* Create Daly BMS object */
// Daly_BMS_UART bms(BMS_SERIAL);

/* Booleans */
bool reverseUpdate = false; //signifies update in reversal of gokart
bool gearChange = true; //signifies update in gear change
bool reverse = false; //signifies gokart is in reverse

/* Variables */
int currentScreen = 1; // value from 0-1
int currentSpeed = 0; // This will be replaced with actual speed from ESC later
int currentGear = 2; // values from 0-2 


void setup() {

  pinMode(LEFT_REVERSE_IN5_BROWN, OUTPUT);
  pinMode(RIGHT_REVERSE_IN6_BROWN, OUTPUT);
  /* initial write to reverse pins since they are automatically opposite rotations */
  digitalWrite(LEFT_REVERSE_IN5_BROWN, HIGH);
  digitalWrite(RIGHT_REVERSE_IN6_BROWN, LOW);

  /* pinMode definitions for the threespeed pins, all output */
  pinMode(LEFT_THREE_SPEED_IN1_BLUE, OUTPUT);
  pinMode(LEFT_THREE_SPEED_IN2_YELLOW, OUTPUT);
  pinMode(RIGHT_THREE_SPEED_IN3_BLUE, OUTPUT);
  pinMode(RIGHT_THREE_SPEED_IN4_YELLOW, OUTPUT);

  /* pinmode definitions for pedal pins */
  pinMode(LEFT_PEDAL_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PEDAL_PIN, INPUT_PULLUP);
  pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);

  /* Initialize I2C */
  Wire.begin(SDA_PIN, SCL_PIN);

  /* Initialize serial & bms serial and bms */
  //BMS_SERIAL.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  //bms.Init();

  Serial.begin(9600);

  /* Initialize Display */
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
  }

  /* Basic setup of OLED*/
  display.begin();
  display.setRotation(1); // 0–3 (landscape = 1 or 3)
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);
  display.display();
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
      delay(100);
    } else if(currentGear == 1)  {
      digitalWrite(LEFT_THREE_SPEED_IN1_BLUE, HIGH);
      digitalWrite(LEFT_THREE_SPEED_IN2_YELLOW, LOW);
      digitalWrite(RIGHT_THREE_SPEED_IN3_BLUE, HIGH);
      digitalWrite(RIGHT_THREE_SPEED_IN4_YELLOW, LOW);
      delay(100);
    } else if(currentGear == 0) {
      digitalWrite(LEFT_THREE_SPEED_IN1_BLUE, LOW);
      digitalWrite(LEFT_THREE_SPEED_IN2_YELLOW, HIGH);
      digitalWrite(RIGHT_THREE_SPEED_IN3_BLUE, LOW);
      digitalWrite(RIGHT_THREE_SPEED_IN4_YELLOW, HIGH);
      delay(100);
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
  reverseUpdate = false;
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
  changeGear();
  /* Checking for reverse and updating */
  changeReverse();
  // Reading button presses for screen changes
  updateScreen();
  // Update BMS
  if(currentScreen == 0) {
    wipeScreen();
    display.setCursor(20, 20);
    display.print("Screen 1");
  } else if(currentScreen == 1) {
    wipeScreen();
    displayGear();
  }
  delay(100);
}
