#include "battery.h"

Battery::Battery(const uint adc_pin)
{
    this->adc_pin = adc_pin;
}

Battery::~Battery() {}

int calculate_battery_level(int raw_value, int low, int high)
{
    return raw_value;
}

int Battery::get_percentage()
{
    if (millis() - timer > 5000)
    {
        int sensor_value = analogRead(adc_pin);
        battery_level = calculate_battery_level(sensor_value, 0, 4096);
        timer = millis();
    }

    return battery_level;
}
