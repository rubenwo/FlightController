#include "logic/controller.h"

Controller::Controller(controller_config cfg) : pid_x(PID(1, 0, 0)), pid_y(PID(1, 0, 0))
{
    pinMode(2, OUTPUT);
    Serial.begin(460800);
    logger.reset(new Logger(Serial));

    for (int i = 0; i < cfg.motor_pins.size(); i++)
    {
        motors[i].reset(new Motor(cfg.motor_pins[i]));
    }
}

Controller::~Controller()
{
}
long timer = 0;
long counter = 0;
void Controller::init()
{
    digitalWrite(2, HIGH);
    logger->stage_msg("Arming motors....", true, true);
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

    logger->stage_msg("All motors are armed!", true, true);

    logger->stage_msg("Connecting to MPU6050...", true, true);
    mpu6050.reset(new MPU6050(Wire));
    Wire.begin(21, 22);
    mpu6050->begin();
    mpu6050->calcGyroOffsets();
    logger->stage_msg("MPU6050 connected!", true, true);

    logger->stage_msg("Connecting to BMP180...", true, true);
    bmp180.reset(new BMP180());
    logger->stage_msg("BMP180 connected!", true, true);

    logger->stage_msg("Connecting to RC receiver...", true, true);
    rc.reset(new RC());
    logger->stage_msg("RC receiver connected!", true, true);

    logger->stage_msg("Starting battery monitor...", true, true);
    battery.reset(new Battery(36));
    logger->stage_msg("Battery monitor is running", true, true);
    digitalWrite(2, LOW);
    timer = millis();
}

void Controller::loop()
{
    counter += 1;

    if (millis() - timer > 1000)
    {
        logger->stage_msg("Counter: " + Logger::to_string(counter), true, true);
        counter = 0;
        timer = millis();
    }
    mpu6050->update();
    auto x = mpu6050->getAngleX();
    auto y = mpu6050->getAngleY();
    //auto height = bmp180->get_height();

    auto controller_input = rc->get_input();
    auto throttle = map(controller_input.ch1, 1000, 2000, 0, 600);
    // if (throttle <= 1)
    //     return;
    auto desired_roll = map(controller_input.ch2, 1000, 2000, -20, 20);
    auto desired_pitch = map(controller_input.ch3, 1000, 2000, -20, 20);

    auto pid_output_x = pid_x.calc_pid(0, x);
    auto pid_output_y = pid_y.calc_pid(0, y);

    throttle = 100;
    int baseline = 115;
    motors[0]->setThrottle(baseline + throttle + pid_output_y - pid_output_x); // Front left
    motors[1]->setThrottle(baseline + throttle - pid_output_y - pid_output_x); // Front right
    motors[2]->setThrottle(baseline + throttle - pid_output_y + pid_output_x); // Back right
    motors[3]->setThrottle(baseline + throttle + pid_output_y + pid_output_x); // Back left

    for (int i = 0; i < motors.size(); i++)
        logger->stage_msg("Motor " + Logger::to_string(i) + ": " + Logger::to_string(motors[i]->getThrottle()));
    logger->push();
}
