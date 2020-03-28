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
    connection = new Connection({1337, "Exogenesis_2.4", "maanDag2018"});
    connection->set_callback([this](message m) {
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

    if (millis() - timer > 1000)
    {

        Serial.println("=======================================================");
        Serial.print("temp : ");
        Serial.println(mpu6050->getTemp());
        Serial.print("accX : ");
        Serial.print(mpu6050->getAccX());
        Serial.print("\taccY : ");
        Serial.print(mpu6050->getAccY());
        Serial.print("\taccZ : ");
        Serial.println(mpu6050->getAccZ());

        Serial.print("gyroX : ");
        Serial.print(mpu6050->getGyroX());
        Serial.print("\tgyroY : ");
        Serial.print(mpu6050->getGyroY());
        Serial.print("\tgyroZ : ");
        Serial.println(mpu6050->getGyroZ());

        Serial.print("accAngleX : ");
        Serial.print(mpu6050->getAccAngleX());
        Serial.print("\taccAngleY : ");
        Serial.println(mpu6050->getAccAngleY());

        Serial.print("gyroAngleX : ");
        Serial.print(mpu6050->getGyroAngleX());
        Serial.print("\tgyroAngleY : ");
        Serial.print(mpu6050->getGyroAngleY());
        Serial.print("\tgyroAngleZ : ");
        Serial.println(mpu6050->getGyroAngleZ());

        Serial.print("angleX : ");
        Serial.print(mpu6050->getAngleX());
        Serial.print("\tangleY : ");
        Serial.print(mpu6050->getAngleY());
        Serial.print("\tangleZ : ");
        Serial.println(mpu6050->getAngleZ());
        Serial.println("=======================================================\n");
        timer = millis();
    }
}
