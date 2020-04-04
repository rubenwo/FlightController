#include <Arduino.h>
#include "controller.h"

Controller controller({12, 13, 14, 15});

void setup()
{
  controller.init();
  controller.run();
}

void loop()
{
  delay(5000);
}