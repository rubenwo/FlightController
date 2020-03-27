#pragma once
#include <Arduino.h>
#include <ESP32Servo.h>

class Motor
{
private:
    int currentThrottle;
    uint pin;
    Servo ESC;

public:
    Motor();

    Motor(const uint pin);
    ~Motor();

    void init();
    void setThrottle(int amt);
    int getThrottle();
    void increaseThrottle(int amt);
    void decreaseThrottle(int amt);
};