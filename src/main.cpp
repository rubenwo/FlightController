#include <Arduino.h>

#include "controller.h"

Controller controller({0, 0, 0, 0, 0});

void setup()
{
  controller.run();
}

void loop()
{
  delay(10000);
}