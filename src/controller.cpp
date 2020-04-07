#include "controller.h"

#include <thread>

Controller::Controller(controller_config cfg)
{
    pinMode(2, OUTPUT);
    Serial.begin(9600);

    for (int i = 0; i < cfg.motor_pins.size(); i++)
    {
        motors[i] = new Motor(cfg.motor_pins[i]);
    }
}

Controller::~Controller()
{
}

float angle_x_error = 0, angle_y_error = 0;

void Controller::init()
{
    digitalWrite(2, HIGH);
    Serial.println("Arming motors....");
    std::thread initM0([](Motor *m) { m->init(); }, motors[0]);
    std::thread initM1([](Motor *m) { m->init(); }, motors[1]);
    std::thread initM2([](Motor *m) { m->init(); }, motors[2]);
    std::thread initM3([](Motor *m) { m->init(); }, motors[3]);

    initM0.join();
    initM1.join();
    initM2.join();
    initM3.join();

    for (int i = 0; i < motors.size(); i++)
    {
        motors[i]->setThrottle(0);
    }

    Serial.println("All motors are armed!");

    Serial.println("Starting battery monitor...");
    battery = new Battery(36);
    Serial.println("Battery monitor is running");

    Serial.println("Initalizing connection...");
    connection = new Connection({1337, "ESP32-Access-Point", "123456789"});
    connection->set_callback([this](message m) {
        if (m.angleX != desired_state.angleX)
            desired_state.angleX = m.angleX;
        if (m.angleY != desired_state.angleY)
            desired_state.angleY = m.angleY;
        if (m.angleZ != desired_state.angleZ)
            desired_state.angleZ = m.angleZ;
        desired_state.on = m.speed > 15;
        desired_state.throttle = m.speed;
        if (m.speed <= 12)
        {
            for (int i = 0; i < motors.size(); i++)
            {
                motors[i]->setThrottle(m.speed);
            }
        }
    });
    Serial.println("Drone is online!");

    Serial.println("Connecting to mpu6050...");
    mpu6050 = new MPU6050(Wire);

    Wire.begin(21, 22);
    mpu6050->begin();
    mpu6050->calcGyroOffsets();

    float x_err = 0, y_err = 0;
    for (int i = 0; i < 100; i++)
    {
        x_err += mpu6050->getAngleX();
        y_err += mpu6050->getAngleY();
        delay(10);
    }

    angle_x_error = (x_err / 100) * -1;
    angle_y_error = (y_err / 100) * -1;

    Serial.println("mpu6050 connected and calibrated");

    digitalWrite(2, LOW);
}

void Controller::loop()
{
    PID_Algo();
    if (millis() - timer > 2500)
    {
        connection->send_info(
            {static_cast<int>(current_state.angleX),
             static_cast<int>(current_state.angleY),
             static_cast<int>(current_state.angleZ),
             static_cast<int>(mpu6050->getTemp()),
             battery->get_percentage(),
             motors[0]->getThrottle(),
             motors[1]->getThrottle(),
             motors[2]->getThrottle(),
             motors[3]->getThrottle()});
        timer = millis();
    }
    delay(25);
}

long algo_timer = 0;

float total_angle_x;
float total_angle_y;

long elapsedTime, current_time, timePrev;

float PID, pwmLeft, pwmRight, error_x, error_y, previous_error_x, previous_error_y;
float pid_p = 0;
float pid_i = 0;
float pid_d = 0;
/////////////////PID CONSTANTS/////////////////
double kp = 1.0;
double ki = 0.003;
double kd = 1.0;
///////////////////////////////////////////////

void Controller::PID_Algo()
{
    if (!desired_state.on)
        return;
    timePrev = current_time;
    current_time = millis();
    elapsedTime = (current_time - timePrev) / 1000;

    mpu6050->update();

    total_angle_x = mpu6050->getAngleX() + angle_x_error;
    total_angle_y = mpu6050->getAngleY() + angle_y_error;
    current_state.angleX = total_angle_x;
    current_state.angleY = total_angle_y;

    error_y = total_angle_y - desired_state.angleY;
    error_x = total_angle_x - desired_state.angleX;

    pid_p = kp * error_y;
    if (error_y > -3 && error_y < 3)
    {
        error_y = 0;
        pid_i = pid_i + (ki * error_y);
    }

    pid_d = kd * ((error_y - previous_error_y) / elapsedTime);
    PID = pid_p + pid_i + pid_d;

    if (PID < -1000)
    {
        PID = -1000;
    }
    if (PID > 1000)
    {
        PID = 1000;
    }

    float corrected_pid = map(PID, -1000, 1000, -50, 50);
    int throttle_left = desired_state.throttle + corrected_pid;
    int throttle_right = desired_state.throttle - corrected_pid;
    if (throttle_left > 180)
        throttle_left = 180;
    if (throttle_left < desired_state.throttle)
        throttle_left = desired_state.throttle;
    if (throttle_right > 180)
        throttle_right = 180;
    if (throttle_right < desired_state.throttle)
        throttle_right = desired_state.throttle;
    motors[0]->setThrottle(throttle_left);
    motors[1]->setThrottle(throttle_right);
    motors[2]->setThrottle(throttle_right);
    motors[3]->setThrottle(throttle_left);
    previous_error_y = error_y;

    if (millis() - algo_timer > 500)
    {
        Serial.println("===== PID_Algo() =====");

        Serial.printf("Angle_X: %f\n", total_angle_x);
        Serial.printf("Angle_Y: %f\n", total_angle_y);

        Serial.printf("corrected_pid=%f\n", corrected_pid);
        Serial.printf("Motor %d has a throttle of: %d\n", 0, motors[0]->getThrottle());
        Serial.printf("Motor %d has a throttle of: %d\n", 1, motors[1]->getThrottle());
        Serial.printf("Motor %d has a throttle of: %d\n", 2, motors[2]->getThrottle());
        Serial.printf("Motor %d has a throttle of: %d\n", 3, motors[3]->getThrottle());

        Serial.println("===== PID_Algo() =====");

        algo_timer = millis();
    }
}

void Controller::run()
{
    xTaskCreatePinnedToCore(
        [](void *c) {
            Connection *conn = static_cast<Connection *>(c);
            for (;;)
            {
                conn->loop();
            }
        },
        "connection_thread",
        51200,
        static_cast<void *>(connection),
        0,
        &connection_task,
        0);

    xTaskCreatePinnedToCore(
        [](void *c) {
            Controller *controller = static_cast<Controller *>(c);
            for (;;)
            {
                controller->loop();
            };
        },
        "controller_thread",
        51200,
        static_cast<void *>(this),
        0,
        &controller_task,
        1);
}
