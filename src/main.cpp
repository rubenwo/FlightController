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