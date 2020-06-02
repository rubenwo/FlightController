#pragma once
#include <Arduino.h>
#include <ESP32Servo.h>

class Motor
{
private:
    Servo ESC;

public:
    Motor() {}

    Motor(const uint pin)
    {
        ESC.attach(pin, 1000, 2000);
    }
    ~Motor() {}

    void init()
    {
        ESC.write(2000); //Set new high
        delay(5000);
        ESC.write(1000); //Set new low
        delay(5000);
    }
    void setThrottle(int microseconds)
    {
        ESC.write(microseconds);
    }
};