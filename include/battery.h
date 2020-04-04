#pragma once

#include <Arduino.h>

class Battery
{
private:
    uint adc_pin;
    int battery_level = 0;
    long timer = 0;

public:
    Battery(const uint adc_pin);
    ~Battery();

    int get_percentage();
};