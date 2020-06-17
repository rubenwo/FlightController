#pragma once
#include <Arduino.h>

class PID
{
private:
    float error, previous_error;
    float accumulation_error, derivative_error;

    float current_time, previous_time, delta_time;
    float output;
    float Kp, Ki, Kd;

public:
    PID(float p = 1, float i = 0, float d = 0) : Kp(p), Ki(i), Kd(d) {}
    ~PID() {}

    void set_Kp(float p) { Kp = p; }
    void set_Ki(float i) { Ki = i; }
    void set_Kd(float d) { Kd = d; }
    void set_current_time(float t) { current_time = t; }

    float calc_pid(float desired = 0, float actual = 0)
    {
        error = desired - actual;
        current_time = millis();
        delta_time = current_time - previous_time;
        previous_time = current_time;

        accumulation_error += (error * delta_time);
        derivative_error = (error - previous_error) / delta_time;

        output = (error * Kp) + (accumulation_error * Ki) + (derivative_error * Kd);

        previous_error = error;

        return output;
    }

    void reset_values()
    {
        current_time = millis();
        error = 0;
        delta_time = 0;
        previous_time = current_time;
        accumulation_error = 0;
        derivative_error = 0;
        output = 0;
        previous_error = error;
    }
};