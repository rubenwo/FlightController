#pragma once

#include <Arduino.h>
#include <array>

#include <MPU6050_tockn.h>
#include <Wire.h>

#include "connection.h"
#include "motor.h"

struct controller_config
{
    std::array<uint, 4> motor_pins;
};

struct state
{
    bool on;
    float angleX;
    float angleY;
    float angleZ;
};

class Controller
{
private:
    std::array<Motor *, 4> motors;
    MPU6050 *mpu6050;
    state current_state = {false, 0, 0, 0};
    state desired_state = {false, 0, 0, 0};
    Connection *connection;
    long timer = 0;

public:
    Controller(controller_config cfg);
    ~Controller();

    void init();
    void loop();
};