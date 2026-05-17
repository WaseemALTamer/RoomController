#include <Arduino.h>




void setup() {

    pinMode(4, OUTPUT);  // red led
    pinMode(16, OUTPUT); // green led
    pinMode(17, OUTPUT); // blue led

    
    digitalWrite(4, LOW);
    digitalWrite(16, HIGH);
    digitalWrite(17, LOW);
}

void loop() {

}


