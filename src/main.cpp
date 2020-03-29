#include <Arduino.h>
#include "controller.h"

Controller controller({12, 13, 14, 15});

void setup()
{

  controller.init();
}

void loop()
{
  controller.loop();
}

// #include <Arduino.h>

// const uint LED = 2;
// const int analogInPin = 36;

// int sensorValue = 0; // value read from the pot
// int outputValue = 0; // value to output to a PWM pin

// void setup()
// {
//   // put your setup code here, to run once:
//   Serial.begin(9600);
//   pinMode(LED, OUTPUT);
// }

// void loop()
// {
//   // read the analog in value
//   sensorValue = analogRead(analogInPin);

//   // map it to the range of the PWM out
//   outputValue = map(sensorValue, 0, 4096, 0, 3300);

//   // print the readings in the Serial Monitor
//   Serial.print("sensor = ");
//   Serial.print(sensorValue);
//   Serial.print("\t output = ");
//   Serial.println(outputValue);

//   delay(1000);
// }