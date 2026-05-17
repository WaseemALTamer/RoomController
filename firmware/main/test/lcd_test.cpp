#include <TFT_eSPI.h>




TFT_eSPI tft = TFT_eSPI();

void setup() {
  tft.init();
  tft.setRotation(1);

  // Fill screen white
  tft.fillScreen(TFT_WHITE);

  pinMode(27, OUTPUT);
  digitalWrite(27, HIGH);

  
}

void loop() {
}