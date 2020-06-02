#include "drivers/BMP180.h"

BMP180::BMP180()
{
    bmp.begin();
}

BMP180::BMP180(uint8_t bus_num, uint8_t sda_pin, uint8_t scl_pin)
{
    bmp.begin();
}

BMP180::~BMP180() {}

float BMP180::get_height()
{
    current_height = bmp.readAltitude();
    return current_height;
}
