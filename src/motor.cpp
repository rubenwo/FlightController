#include "motor.h"
#include <Arduino.h>
Motor::Motor() {}

Motor::Motor(const uint pin)
{
    ESC.attach(pin, 1000, 2000);
}

Motor::~Motor()
{
}

void Motor::init()
{
    ESC.write(180); //Set new high
    delay(5000);
    ESC.write(0); //Set new low
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
    if (currentThrottle < 0)
        currentThrottle = 0;
    ESC.write(currentThrottle);
}

void Motor::decreaseThrottle(int amt)
{
    currentThrottle -= amt;
    if (currentThrottle > 180)
        currentThrottle = 180;
    if (currentThrottle < 0)
        currentThrottle = 0;
    ESC.write(currentThrottle);
}