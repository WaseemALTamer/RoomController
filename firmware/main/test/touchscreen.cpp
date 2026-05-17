#include <Wire.h>
#include <TAMC_GT911.h>

#define TOUCH_SDA 33
#define TOUCH_SCL 32
#define TOUCH_INT 21
#define TOUCH_RST 25

#define TOUCH_WIDTH 320
#define TOUCH_HEIGHT 480

TAMC_GT911 ts = TAMC_GT911(
  TOUCH_SDA,
  TOUCH_SCL,
  TOUCH_INT,
  TOUCH_RST,
  TOUCH_WIDTH,
  TOUCH_HEIGHT
);

void setup() {
  Serial.begin(115200);

  Wire.begin(TOUCH_SDA, TOUCH_SCL);

  ts.begin();
  ts.setRotation(1); // adjust if orientation is wrong

  Serial.println("Touch ready!");
}

void loop() {
  ts.read();

  if (ts.isTouched) {
    for (int i = 0; i < ts.touches; i++) {
      Serial.print("X: ");
      Serial.print(ts.points[i].x);
      Serial.print(" Y: ");
      Serial.println(ts.points[i].y);
    }
  }

  delay(50);
}