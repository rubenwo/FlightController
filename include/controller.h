#pragma once

#include <Arduino.h>
#include <array>

#include "motor.h"
#include "mpu6050.h"

struct controller_config
{
    std::array<uint, 4> motor_pins;
    uint sensor_pin;
};

class Controller
{
private:
    std::array<Motor *, 4> motors;
    MPU6050 sensor;

public:
    Controller(controller_config cfg);
    ~Controller();

    void run();
};