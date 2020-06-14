#include "drivers/BMP180.h"

BMP180::BMP180()
{
    bmp.begin();
    pressure_offset = bmp.readSealevelPressure();
    timer = millis();
}

BMP180::~BMP180() {}

float BMP180::get_altitude()
{
    if (millis() - timer > 2000)
    {
        current_height = bmp.readAltitude(pressure_offset);
        timer = millis();
    }
    return current_height;
}
