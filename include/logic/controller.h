#pragma once

#include <Arduino.h>
#include <array>
#include <memory>
#include <thread>
#include <string>

#include "drivers/motor.h"
#include "drivers/battery.h"
#include "drivers/BMP180.h"
#include "drivers/imu.h"
#include "drivers/gps.h"

#include "conn/bluetooth.h"
#include "conn/rc.h"
#include "conn/logger.h"

#include "logic/pid.h"
#include "logic/FSM.h"

enum mode
{
    QUAD_COPTER,
    PLANE,
};

enum states
{
    S_IDLE,
    S_TAKE_OFF,
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
    TaskHandle_t bt_task_handle;

    std::array<std::shared_ptr<Motor>, 4> motors;
    std::unique_ptr<IMU> imu;
    std::unique_ptr<Battery> battery;
    std::unique_ptr<BMP180> bmp180;
    std::unique_ptr<RC> rc;
    std::unique_ptr<Logger> logger;
    std::unique_ptr<BT_Conn> bt;

    PID pid_x;
    PID pid_y;
    PID pid_z;
    RC_Input_Values controller_input;

    states state = S_IDLE;
    float pid_output_x, pid_output_y, pid_output_z = 0;
    int throttle, desired_roll, desired_pitch, desired_yaw = 0;
    int base_speed = 1000;

    long take_off_timeout = 4000, take_off_timer = 0;

public:
    Controller(controller_config cfg);
    ~Controller();

    void init();
    void loop();
};