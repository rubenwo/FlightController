#pragma once

#include <Arduino.h>
#include <array>

#include <MPU6050_tockn.h>
#include <Wire.h>

#include "motor.h"

struct controller_config
{
    std::array<uint, 4> motor_pins;
};

class Controller
{
private:
    std::array<Motor *, 4> motors;
    MPU6050 *mpu6050;

public:
    Controller(controller_config cfg);
    ~Controller();
    
    void init();
    void run();
};