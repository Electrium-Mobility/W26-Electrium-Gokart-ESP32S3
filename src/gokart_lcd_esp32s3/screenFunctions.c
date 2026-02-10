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

void displaySpeed() {
  display.fillRect(50,32,70,7,ILI9341_BLACK);
  display.setCursor(50,32);
  display.print(currentSpeed);
  display.setTextSize(0.25);
  display.print("km/h");
  display.setTextSize(1);
}

void wipeScreen() {
  display.fillScreen(ILI9341_BLACK);
}
