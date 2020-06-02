#pragma once
#include <Arduino.h>

struct output_values
{
    int output_x, output_y;
};

class PID
{
private:
    float error_x, error_y, previous_error_x, previous_error_y;
    float accumulation_error_x, accumulation_error_y, derivative_error_x, derivative_error_y;

    float current_time, previous_time, delta_time;

    output_values output_value;

    float Kp_x, Ki_x, Kd_x;
    float Kp_y, Ki_y, Kd_y;

public:
    PID(float p_x = 1, float i_x = 0, float d_x = 0, float p_y = 1, float i_y = 0, float d_y = 0) : Kp_x(p_x), Ki_x(i_x), Kd_x(d_x), Kp_y(p_y), Ki_y(i_y), Kd_y(d_y) {}
    ~PID() {}

    void set_Kp_x(float p) { Kp_x = p; }
    void set_Ki_x(float i) { Kp_x = i; }
    void set_Kd_x(float d) { Kp_x = d; }
    void set_Kp_y(float p) { Kp_y = p; }
    void set_Ki_y(float i) { Kp_y = i; }
    void set_Kd_y(float d) { Kp_y = d; }

    void set_current_time(float t) { current_time = t; }

    output_values output(float desired_x = 0, float desired_y = 0, float actual_x = 0, float actual_y = 0)
    {
        error_x = desired_x - actual_x;
        error_y = desired_y - actual_y;

        current_time = micros();
        delta_time = current_time - previous_time;
        previous_time = current_time;

        accumulation_error_x += (error_x * delta_time);
        accumulation_error_y += (error_y * delta_time);
        derivative_error_x = (error_x - previous_error_x) / delta_time;
        derivative_error_y = (error_y - previous_error_y) / delta_time;

        output_value.output_x = (error_x * Kp_x) + (accumulation_error_x * Ki_y) + (derivative_error_x * Kd_y);
        output_value.output_y = (error_y * Kp_y) + (accumulation_error_y * Ki_y) + (derivative_error_y * Kd_y);

        previous_error_x = error_x;
        previous_error_y = error_y;

        return output_value;
    }
};