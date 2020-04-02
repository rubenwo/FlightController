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

    Serial.println("Connecting to mpu6050....");
    mpu6050 = new MPU6050(Wire);

    Wire.begin(21, 22);
    mpu6050->begin();
    mpu6050->calcGyroOffsets(true);
    Serial.println("mpu6050 connected and calibrated");

    Serial.println("Initalizing connection...");
    connection = new Connection({1337, "ESP32-Access-Point", "123456789"});
    connection->set_callback([this](message m) {
        Serial.println(m.speed);
        for (int i = 0; i < motors.size(); i++)
        {
            motors[i]->setThrottle(m.speed);
        }
    });
    Serial.println("Drone is online!");

    digitalWrite(2, LOW);
}

void Controller::loop()
{

    mpu6050->update();
    current_state = {mpu6050->getAngleX(), mpu6050->getAngleY(), mpu6050->getAngleZ()};
    if (desired_state.on && millis() - timer > 250)
    {
        if (desired_state.angleX > current_state.angleX)
        {
            motors[0]->increaseThrottle(1);
            motors[1]->increaseThrottle(1);
            motors[2]->decreaseThrottle(1);
            motors[3]->decreaseThrottle(1);
        }
        if (desired_state.angleX < current_state.angleX)
        {
            motors[0]->decreaseThrottle(1);
            motors[1]->decreaseThrottle(1);
            motors[2]->increaseThrottle(1);
            motors[3]->increaseThrottle(1);
        }

        if (desired_state.angleY > current_state.angleY)
        {
            motors[0]->increaseThrottle(1);
            motors[1]->decreaseThrottle(1);
            motors[2]->decreaseThrottle(1);
            motors[3]->increaseThrottle(1);
        }
        if (desired_state.angleY < current_state.angleY)
        {
            motors[0]->decreaseThrottle(1);
            motors[1]->increaseThrottle(1);
            motors[2]->increaseThrottle(1);
            motors[3]->decreaseThrottle(1);
        }

        if (desired_state.angleZ > current_state.angleZ)
        {
            motors[0]->decreaseThrottle(1);
            motors[1]->increaseThrottle(1);
            motors[2]->decreaseThrottle(1);
            motors[3]->increaseThrottle(1);
        }
        if (desired_state.angleZ < current_state.angleZ)
        {
            motors[0]->increaseThrottle(1);
            motors[1]->decreaseThrottle(1);
            motors[2]->increaseThrottle(1);
            motors[3]->decreaseThrottle(1);
        }
        timer = millis();
    }
}
