#include "gyro.h"

Gyro::Gyro(const uint scl_pin, const uint sda_pin)
{
    two_wire = &Wire;
    two_wire->begin(scl_pin, sda_pin);

    
}

Gyro::~Gyro()
{
}