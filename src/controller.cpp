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

float angle_x_error, angle_y_error;

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
        desired_state.on = m.speed > 0;
        for (int i = 0; i < motors.size(); i++)
        {
            motors[i]->setThrottle(m.speed);
        }
    });
    Serial.println("Drone is online!");

    Serial.println("Connecting to mpu6050...");
    mpu6050 = new MPU6050(Wire);

    Wire.begin(21, 22);
    mpu6050->begin();
    mpu6050->calcGyroOffsets();

    angle_x_error = mpu6050->getAngleX();
    angle_y_error = mpu6050->getAngleY();
    if (angle_x_error < 0)
        angle_x_error *= -1;
    if (angle_y_error < 0)
        angle_y_error *= -1;

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
int i;
float rad_to_deg = 180 / 3.141592654;

float PID, pwmLeft, pwmRight, error, previous_error;
float pid_p = 0;
float pid_i = 0;
float pid_d = 0;
/////////////////PID CONSTANTS/////////////////
double kp = 3.55;  //3.55
double ki = 0.005; //0.003
double kd = 2.05;  //2.05
///////////////////////////////////////////////

void Controller::PID_Algo()
{

    timePrev = current_time;
    current_time = millis();
    elapsedTime = (current_time - timePrev) / 1000;

    mpu6050->update();

    total_angle_x = mpu6050->getAngleX() + angle_x_error;
    total_angle_y = mpu6050->getAngleY() + angle_y_error;

    error = total_angle_y - desired_state.angleY;

    pid_p = kp * error;
    if (error > -3 && error < 3)
    {
        pid_i = pid_i + (ki * error);
    }

    pid_d = kd * ((error - previous_error) / elapsedTime);
    PID = pid_p + pid_i + pid_d;

    if (PID < -1000)
    {
        PID = -1000;
    }
    if (PID > 1000)
    {
        PID = 1000;
    }

    float corrected_pid = map(PID, -1000, 1000, -90, 90);

    motors[0]->increaseThrottle(corrected_pid);
    motors[1]->decreaseThrottle(corrected_pid);
    motors[2]->decreaseThrottle(corrected_pid);
    motors[3]->increaseThrottle(corrected_pid);
    previous_error = error;

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
