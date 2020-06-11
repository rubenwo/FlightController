#pragma once
#include <Arduino.h>
#include <ESP32Servo.h>

class Motor
{
private:
    Servo ESC;
    int current_speed;

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
        current_speed = microseconds;
        ESC.write(current_speed);
    }

    int getThrottle()
    {
        return current_speed;
    }
};