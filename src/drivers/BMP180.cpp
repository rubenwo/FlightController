#include "drivers/BMP180.h"

BMP180::BMP180()
{
    bmp.begin();
    pressure_offset = bmp.readSealevelPressure();
}

BMP180::~BMP180() {}

float BMP180::get_height()
{
    current_height = bmp.readAltitude(pressure_offset);
    return current_height;
}
