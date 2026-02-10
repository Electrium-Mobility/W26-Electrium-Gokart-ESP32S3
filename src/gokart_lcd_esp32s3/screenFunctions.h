#ifndef SCREEN_FUNCTIONS_H
#define SCREEN_FUNCTIONS_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* Screen config */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SDA_PIN 21
#define SCL_PIN 47

/* Shared display object */
extern Adafruit_SSD1306 display;

/* Shared state coming from main.ino */
extern bool gearChange;
extern int currentGear;
extern int currentSpeed;

/* INIT*/
void initDisplay();
/* Function that updates gear display on LCD monitor */
void displayGear();
/* Function that updates speed display on LCD monitor */
void displaySpeed();
/* Function that completely wipes screen by printing black rect*/
void wipeScreen();

#endif
