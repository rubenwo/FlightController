#pragma once

#include <Wire.h>

struct gyro_data
{
};

class Gyro
{
private:
    TwoWire *two_wire;
    gyro_data data;

public:
    Gyro(const uint scl_pin, const uint sda_pin);
    ~Gyro();

    gyro_data get_gyro_data() { return data; }
};