#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <daly-bms-uart.h>

// Display resolution
#define SCREEN_WIDTH 128 // adjust if your display is 128x64, 128x32, or 240x240
#define SCREEN_HEIGHT 64

// Reset pin (not used for I2C)
#define OLED_RESET -1

// I2C pins for ESP32-S3
#define SDA_PIN 21
#define SCL_PIN 47

// Pedal pin definitions connecting to ESP32S3
#define RIGHT_PEDAL_PIN 7
#define LEFT_PEDAL_PIN 8

// Button pin definitions connecting to ESP32S3
#define RIGHT_BUTTON_PIN 9
#define LEFT_BUTTON_PIN 10

// 6 Channel relay pin definitions, connects to each 3 speed
#define LEFT_THREE_SPEED_IN1_BLUE 16
#define LEFT_THREE_SPEED_IN2_YELLOW 25
#define RIGHT_THREE_SPEED_IN3_BLUE 14
#define RIGHT_THREE_SPEED_IN4_YELLOW 24
#define LEFT_REVERSE_IN5_BROWN 26
#define RIGHT_REVERSE_IN6_BROWN 29
#define HALL_EFFECT 27
#define REVERSE_LEFT_MOTOR -1
#define REVERSE_RIGHT_MOTOR -1

// Create display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Booleans
bool reverseUpdate = false; //signifies update in reversal of gokart
bool gearChange = true; //signifies update in gear change
bool reverse = false; //signifies gokart is in reverse

// Variables
int currentScreen = 1; // value from 0-1
int currentSpeed = 0; // This will be replaced with actual speed from ESC later
int currentGear = 2; // values from 0-2 according to gearInfo typedef

//typedef for gear

typedef enum {
  LOW,
  MEDIUM,
  HIGH,
  NUM_OF_GEARS,
} gearInfo;

void setup() {
  // Initialize I2C
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 0x3C is the common I2C address
    Serial.println(F("SSD1306 allocation failed"));
    while(true);
  }

  //basic setup
  display.clearDisplay();
  display.setTextSize(0.75);
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);
  display.display();

  //pinmode setup
  pinMode(LEFT_PEDAL_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PEDAL_PIN, INPUT_PULLUP);
  pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);
}

// Basic function that updates gear value 
void updateGear() {
 // Later will fetch gear from ESP input from pedals
  currentGear += 1;
  if(currentGear > 3) {
    currentGear = 1;
  }

}

// Basic function that updates speed value
void updateSpeed() {
  // Later this function will fetch the speed from the ESC
  currentSpeed += 5; // debugging, changing speed
  if(currentSpeed > 100) {
    currentSpeed = 25;
  }
}

// Basic function that updates gear value on LCD screen
void displayGear() {
  if (gearChange){
    display.display();
    display.setCursor(65,5);
    display.setTextWrap(false);
    display.print("Gear: ");
    display.setCursor(95,5);
    display.fillRect(95,5,5,7,SSD1306_BLACK);
    display.print(currentGear);
    gearChange = false;
  } 
}

// Basic function that updates speed value on LCD screen
void displaySpeed() {
  display.display();
  display.fillRect(50,32,70,7,SSD1306_BLACK);
  display.setCursor(50,32);
  display.print(currentSpeed);
  display.setTextSize(0.25);
  display.print("km/h");
  display.setTextSize(0.75);
}

// Basic function that clears LCD screen of all that is currently being displayed
void wipeScreen() {
  display.display();
  display.fillRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,SSD1306_BLACK);
}

// Function that checks for pedal inputs and updates gear + reversal accordingly
void checkPedalInputs() {
  // Reading for pedal presses for gear change + changing reversal of direction
  if(!reverse) {
    if (digitalRead(RIGHT_PEDAL_PIN) == LOW){
      if (currentGear < 3){
        gearChange = true;
        currentGear++;
      }
    } else if (digitalRead(LEFT_PEDAL_PIN) == LOW) {
      if (currentGear > 1){
        gearChange = true;
        currentGear--;
      } else if(currentGear == 1) {
        reverseUpdate = true;
        reverse = true;
      }
    } 
  } else if (reverse) {
    if (digitalRead(RIGHT_PEDAL_PIN) == LOW){
      if (currentGear > 1){
        gearChange = true;
        currentGear--;
      } else if(currentGear == 1) {
        reverseUpdate = true;
        reverse = true;
      }
    } else if (digitalRead(LEFT_PEDAL_PIN) == LOW) {
      if (currentGear < 3){
        gearChange = true;
        currentGear++;
      }
    }
  }
}


// Checks if gear must be updated, and communicates to relay accordingly.
void changeGear() {
  if(gearChange){
    if(gear == HIGH) { 
      digitalWrite(LEFT_THREE_SPEED_IN1_BLUE, LOW);
      digitalWrite(LEFT_THREE_SPEED_IN2_YELLOW, LOW);
      digitalWrite(RIGHT_THREE_SPEED_IN1_BLUE, LOW);
      digitalWrite(RIGHT_THREE_SPEED_IN2_YELLOW, LOW);
    } else if(gear == MEDIUM)  {
      digitalWrite(LEFT_THREE_SPEED_IN1_BLUE, HIGH);
      digitalWrite(LEFT_THREE_SPEED_IN2_YELLOW, LOW);
      digitalWrite(RIGHT_THREE_SPEED_IN1_BLUE, HIGH);
      digitalWrite(RIGHT_THREE_SPEED_IN2_YELLOW, LOW);
    } else if(gear == LOW) {
      digitalWrite(LEFT_THREE_SPEED_IN1_BLUE, LOW);
      digitalWrite(LEFT_THREE_SPEED_IN2_YELLOW, HIGH);
      digitalWrite(RIGHT_THREE_SPEED_IN1_BLUE, LOW);
      digitalWrite(RIGHT_THREE_SPEED_IN2_YELLOW, HIGH);
    }
}

// Reads button press inputs and updates currentScreen variable
void updateScreen() {
  if(digitalRead(LEFT_BUTTON_PIN == LOW)) {
    if(currentScreen > 1) {
      currentScreen -= 1;
    }
  } else if (digitalRead(RIGHT_BUTTON_PIN == LOW)) {
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
  //Reading button presses for screen changes
  updateScreen();

  if(currentScreen == 0) {
    //display something
  } else if(currentScreen == 1) {
    //displays gear
    wipeScreen();
    updateGear();
    displayGear();
  }
  delay(100);
}
