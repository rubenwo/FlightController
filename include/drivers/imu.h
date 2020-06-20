#pragma once
#include "i2cdevlib/I2Cdev.h"
#include "i2cdevlib/MPU6050_6Axis_MotionApps20.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

class IMU
{

public:
    IMU(const uint interrupt_pin);
    ~IMU();

    void update();

    float getYaw() { return ypr[0] * 180 / M_PI; }
    float getPitch() { return ypr[1] * 180 / M_PI; }
    float getRoll() { return ypr[2] * 180 / M_PI; }
    static volatile bool mpuInterrupt; // indicates whether MPU interrupt pin has gone high

private:
    uint interrupt_pin;
    MPU6050 mpu;

    bool dmpReady = false; // set true if DMP init was successful

    uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
    uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
    uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
    uint16_t fifoCount;     // count of all bytes currently in FIFO
    uint8_t fifoBuffer[64]; // FIFO storage buffer

    // orientation/motion vars
    Quaternion q;        // [w, x, y, z]         quaternion container
    VectorInt16 aa;      // [x, y, z]            accel sensor measurements
    VectorInt16 aaReal;  // [x, y, z]            gravity-free accel sensor measurements
    VectorInt16 aaWorld; // [x, y, z]            world-frame accel sensor measurements
    VectorFloat gravity; // [x, y, z]            gravity vector
    float euler[3];      // [psi, theta, phi]    Euler angle container
    float ypr[3];        // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
};