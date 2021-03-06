#pragma once
#include <Arduino.h>
#include <Adafruit_BMP085.h>

class BMP180
{
private:
    float current_height;
    int pressure_offset;
    Adafruit_BMP085 bmp;

    long timer = 0;

public:
    BMP180();
    ~BMP180();

    float get_altitude();
};