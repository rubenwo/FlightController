#include "motor.h"
#include <Arduino.h>
Motor::Motor() {}

Motor::Motor(const uint pin)
{
    this->pin = pin;
    ESC.attach(this->pin, 1000, 2000);
}

Motor::~Motor()
{
}

void Motor::init()
{
    ESC.write(180);
    delay(5000);
    ESC.write(0);
    delay(5000);
}

int Motor::getThrottle()
{
    return currentThrottle;
}

void Motor::setThrottle(int amt)
{
    currentThrottle = amt;
    ESC.write(currentThrottle);
}

void Motor::increaseThrottle(int amt)
{
    currentThrottle += amt;
    if (currentThrottle > 180)
        currentThrottle = 180;
}

void Motor::decreaseThrottle(int amt)
{
    currentThrottle -= amt;
    if (currentThrottle < 0)
        currentThrottle = 0;
}