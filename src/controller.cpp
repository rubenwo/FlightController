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
    delay(10);
}

long algo_timer = 0;

float total_angle_x;
float total_angle_y;

long elapsedTime, current_time, timePrev;

float x_PID, y_PID, pwm_R_F, pwm_R_B, pwm_L_B, pwm_L_F, error_x, error_y, previous_error_x, previous_error_y;
float x_pid_p = 0;
float x_pid_i = 0;
float x_pid_d = 0;
float y_pid_p = 0;
float y_pid_i = 0;
float y_pid_d = 0;
/////////////////X PID CONSTANTS/////////////////
double x_kp = 0.5;
double x_ki = 0;
double x_kd = 1.0;
///////////////////////////////////////////////
/////////////////Y PID CONSTANTS/////////////////
double y_kp = 0.5;
double y_ki = 0;
double y_kd = 1.0;
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

    y_pid_p = y_kp * error_y;
    x_pid_p = x_kp * error_x;

    if (error_y > -3 && error_y < 3)
    {
        y_pid_i = y_pid_i + (y_ki * error_y);
    }

    if (error_x > -3 && error_y < 3)
    {
        x_pid_i = x_pid_i + (x_ki * error_x);
    }

    y_pid_d = y_kd * ((error_y - previous_error_y) / elapsedTime);
    x_pid_d = x_kd * ((error_x - previous_error_x) / elapsedTime);

    y_PID = y_pid_p + y_pid_i + y_pid_d;
    x_PID = x_pid_p + x_pid_i + x_pid_d;

    if (y_PID < -400)
    {
        y_PID = -400;
    }
    if (y_PID > 400)
    {
        y_PID = 400;
    }

    if (x_PID < -400)
    {
        x_PID = -400;
    }
    if (x_PID > 400)
    {
        x_PID = 400;
    }
    int input = map(desired_state.throttle, 0, 100, 1000, 2000);
    pwm_R_F = 115 + input - y_PID - x_PID;
    pwm_R_B = 115 + input - y_PID + x_PID;
    pwm_L_B = 115 + input + y_PID + x_PID;
    pwm_L_F = 115 + input + y_PID - x_PID;

    if (pwm_R_F < 1100)
    {
        pwm_R_F = 1100;
    }
    if (pwm_R_F > 2000)
    {
        pwm_R_F = 2000;
    }

    //Left front
    if (pwm_L_F < 1100)
    {
        pwm_L_F = 1100;
    }
    if (pwm_L_F > 2000)
    {
        pwm_L_F = 2000;
    }

    //Right back
    if (pwm_R_B < 1100)
    {
        pwm_R_B = 1100;
    }
    if (pwm_R_B > 2000)
    {
        pwm_R_B = 2000;
    }

    //Left back
    if (pwm_L_B < 1100)
    {
        pwm_L_B = 1100;
    }
    if (pwm_L_B > 2000)
    {
        pwm_L_B = 2000;
    }

    motors[0]->setThrottle(map(pwm_L_F, 1000, 2000, 20, 100));
    motors[1]->setThrottle(map(pwm_R_F, 1000, 2000, 20, 100));
    motors[2]->setThrottle(map(pwm_R_B, 1000, 2000, 20, 100));
    motors[3]->setThrottle(map(pwm_L_B, 1000, 2000, 20, 100));

    previous_error_y = error_y;
    previous_error_x = error_x;

    if (millis() - algo_timer > 500)
    {
        Serial.println("===== PID_Algo() =====");

        Serial.printf("Angle_X: %f\n", total_angle_x);
        Serial.printf("Angle_Y: %f\n", total_angle_y);

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
