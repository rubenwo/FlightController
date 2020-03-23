#pragma once

#include <Arduino.h>

struct sensor_data
{
    float angle_pitch_output;
    float angle_roll_output;

    int accelX;
    int accelY;
    int accelZ;

    int gyroX;
    int gyroY;
    int gyroZ;

    int temperature;
};

class MPU6050
{
private:
    sensor_data data;

public:
    MPU6050();
    ~MPU6050();

    void startCollectData(uint pinToCore);
    void collectData();
    sensor_data getData();

    int getAccelX();
    int getAccelY();
    int getAccelZ();

    int getGyroX();
    int getGyroY();
    int getGyroZ();

    int getTemperature();
};