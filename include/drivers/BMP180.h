#pragma once
#include <Arduino.h>
#include <Adafruit_BMP085.h>

class BMP180
{
private:
    float current_height;
    Adafruit_BMP085 bmp;

public:
    BMP180();
    BMP180(uint8_t bus_num, uint8_t sda_pin, uint8_t scl_pin);
    ~BMP180();

    float get_height();
};