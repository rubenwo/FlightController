#include "logic/controller.h"

Controller::Controller(controller_config cfg) : pid(PID(1, 0, 0, 1, 0, 0))
{
    pinMode(2, OUTPUT);
    Serial.begin(460800);
    
    for (int i = 0; i < cfg.motor_pins.size(); i++)
    {
        motors[i].reset(new Motor(cfg.motor_pins[i]));
    }
}

Controller::~Controller()
{
}

void Controller::init()
{
    digitalWrite(2, HIGH);
    Serial.println("Arming motors....");
    std::thread initM0([](const std::shared_ptr<Motor> &m) { m->init(); }, motors[0]);
    std::thread initM1([](const std::shared_ptr<Motor> &m) { m->init(); }, motors[1]);
    std::thread initM2([](const std::shared_ptr<Motor> &m) { m->init(); }, motors[2]);
    std::thread initM3([](const std::shared_ptr<Motor> &m) { m->init(); }, motors[3]);

    initM0.join();
    initM1.join();
    initM2.join();
    initM3.join();

    for (int i = 0; i < motors.size(); i++)
    {
        motors[i].get()->setThrottle(0);
    }

    Serial.println("All motors are armed!");

    Serial.println("Connecting to MPU6050...");
    mpu6050.reset(new MPU6050(Wire));
    Wire.begin(21, 22);
    mpu6050->begin();
    mpu6050->calcGyroOffsets();
    Serial.println("MPU6050 connected!");

    Serial.println("Connecting to BMP180...");
    bmp180.reset(new BMP180());
    Serial.println("BMP180 connected!");

    Serial.println("Connecting to RC receiver...");
    rc.reset(new RC());
    Serial.println("RC receiver connected!");

    Serial.println("Starting battery monitor...");
    battery.reset(new Battery(36));
    Serial.println("Battery monitor is running");
    digitalWrite(2, LOW);
}

void Controller::loop()
{
    mpu6050->update();
    auto x = mpu6050->getAngleX();
    auto y = mpu6050->getAngleY();
    auto height = bmp180->get_height();

    Serial.print("Angle X: ");
    Serial.println(x);

    Serial.print("Angle Y: ");
    Serial.println(y);

    Serial.print("Altitude: ");
    Serial.println(height);

    auto controller_input = rc->get_input();
    Serial.println(controller_input.ch1);
    auto throttle = map(controller_input.ch1, 1000, 2000, 0, 600);
    // if (throttle <= 1)
    //     return;
    auto desired_roll = map(controller_input.ch2, 1000, 2000, -20, 20);
    auto desired_pitch = map(controller_input.ch3, 1000, 2000, -20, 20);

    auto pid_output = pid.output(0, 0, x, y);

    Serial.print("PID X: ");
    Serial.println(pid_output.output_x);
    Serial.print("PID Y: ");
    Serial.println(pid_output.output_y);

    int baseline = 115;
    motors[0]->setThrottle(baseline + throttle + pid_output.output_y - pid_output.output_y); // Front left
    motors[1]->setThrottle(baseline + throttle - pid_output.output_y - pid_output.output_y); // Front right
    motors[2]->setThrottle(baseline + throttle - pid_output.output_y + pid_output.output_y); // Back right
    motors[3]->setThrottle(baseline + throttle + pid_output.output_y + pid_output.output_y); // Back left

    for (const auto &m : motors)
    {
        Serial.println(m->getThrottle());
    }
}
