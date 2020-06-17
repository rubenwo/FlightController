#pragma once

#include <Arduino.h>
#include <array>
#include <memory>
#include <thread>
#include <string>
#include <MPU6050_tockn.h>

#include "drivers/motor.h"
#include "drivers/battery.h"
#include "drivers/BMP180.h"
#include "conn/rc.h"
#include "logic/pid.h"
#include "conn/logger.h"
#include "logic/FSM.h"
#include "conn/bluetooth.h"

enum mode
{
    QUAD_COPTER,
    PLANE,
};

enum states
{
    S_IDLE,
    S_FLYING,
    S_LANDING,
    S_ERROR,
    NUM_STATES,
};

enum alphabet
{
    IDLE,
    FLYING,
    LANDING,
    ERROR,
    TIMER,
    NUM_SYMBOLS,
};

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
    std::unique_ptr<Logger> logger;
    std::unique_ptr<BT_Conn> bt;

    PID pid_x;
    PID pid_y;
    PID pid_z;

    FSM<states, alphabet, NUM_STATES, NUM_SYMBOLS> fsm;

    states state = S_IDLE;
    float pid_output_x, pid_output_y = 0;
    int throttle, desired_roll, desired_pitch = 0;
    int base_speed = 1000;

public:
    Controller(controller_config cfg);
    ~Controller();

    void init();
    void loop();
};