#pragma once

#include <Arduino.h>
#include <array>

#include <MPU6050_tockn.h>
#include <Wire.h>

#include "connection.h"
#include "motor.h"
#include "battery.h"

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
    Connection *connection;
    Battery *battery;

    state current_state = {false, 0, 0, 0};
    state desired_state = {false, 0, 0, 0};
    long timer = 0;

    TaskHandle_t connection_task;
    TaskHandle_t controller_task;

public:
    Controller(controller_config cfg);
    ~Controller();

    void PID_Algo();

    void init();
    void loop();
    void run();
};