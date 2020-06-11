#pragma once

#include <Arduino.h>

class Battery
{
private:
    uint adc_pin;

    int battery_voltage = 0;
    int adc_value = 0;

    long timer = 0;

public:
    Battery(const uint adc_pin) : adc_pin(adc_pin) {}
    ~Battery() {}

    int get_voltage()
    {
        if (millis() - timer > 1000)
        {
            adc_value = analogRead(adc_pin);

            battery_voltage = map(adc_value, 0, 4096, 0, 13);
            timer = millis();
        }
        return battery_voltage;
    }
};