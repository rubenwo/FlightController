#include "motor.h"

Motor::Motor() {}

Motor::Motor(const uint pin)
{
    this->pin = pin;
    this->ESC.attach(this->pin, 1000, 2000);

    digitalWrite(2, HIGH);
    ESC.write(180);
    delay(5000);
    ESC.write(0);
    delay(5000);
    digitalWrite(2, LOW);
}

Motor::~Motor()
{
}

int Motor::getThrottle()
{
    return this->currentThrottle;
}

void Motor::setThrottle(int amt)
{
    this->currentThrottle = amt;
    this->ESC.write(this->currentThrottle);
}

void Motor::increaseThrottle(int amt)
{
    this->currentThrottle += amt;
    if (this->currentThrottle > 180)
        this->currentThrottle = 180;
}

void Motor::decreaseThrottle(int amt)
{
    this->currentThrottle -= amt;
    if (this->currentThrottle < 0)
        this->currentThrottle = 0;
}