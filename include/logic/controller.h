#pragma once

#include <Arduino.h>
#include <array>
#include <memory>
#include <thread>
#include <MPU6050_tockn.h>

#include "drivers/motor.h"
#include "drivers/battery.h"
#include "drivers/BMP180.h"
#include "conn/rc.h"
#include "logic/pid.h"

struct controller_config
{
    std::array<uint, 4> motor_pins;
};

class Controller
{
private:
    std::array<std::shared_ptr<Motor>, 4> motors;
    std::unique_ptr<MPU6050> mpu6050;
    std::unique_ptr<Battery> battery;
    std::unique_ptr<BMP180> bmp180;
    std::unique_ptr<RC> rc;
    PID pid;

public:
    Controller(controller_config cfg);
    ~Controller();

    void init();
    void loop();
};