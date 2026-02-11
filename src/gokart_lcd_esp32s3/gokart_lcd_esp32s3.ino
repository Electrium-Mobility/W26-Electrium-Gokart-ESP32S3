#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string.h>
#include <stdlib.h>
#include <driver/twai.h>
#include "screenFunctions.h"
#include "hardware.h"

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

  /* Initialize Display */
  initDisplay();

  Serial.begin(9600);
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

/* Receive CAN Message */
void receiveCANMessage()
{
  twai_message_t message;
  esp_err_t err = twai_receive(&message, pdMS_TO_TICKS(1000));
  if (err == ESP_OK) {
      Serial.println("Message received");
      // process message...
  } else {
      Serial.print("Failed to receive message: ");
      Serial.println(esp_err_to_name(err));
      return;
  }

  // Process received message
  if (message.extd)
  {
    printf("Message is in Extended Format\n");
  }
  else
  {
    printf("Message is in Standard Format\n");
  }
  printf("ID is %d\n", message.identifier);
  if (!(message.rtr))
  {
    for (int i = 0; i < message.data_length_code; i++)
    {
      printf("Data byte %d = %d\n", i, message.data[i]);
      speed = message.data[i];
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
  } else if(currentScreen == 1) {
    wipeScreen();
    displayGear();
  }
  delay(100);
}
