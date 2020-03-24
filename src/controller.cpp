#include "controller.h"

Controller::Controller(controller_config cfg)
{
    mpu6050 = new MPU6050(Wire);
    Serial.begin(9600);
    for (int i = 0; i < cfg.motor_pins.size(); i++)
    {
        motors[i] = new Motor(cfg.motor_pins[i]);
    }
    Wire.begin();
    mpu6050->begin();
    mpu6050->calcGyroOffsets(true);
}

Controller::~Controller()
{
}

TaskHandle_t control_Led_Task;

void controlLed(void *parameter)
{
    uint LED = (uint)parameter;
    for (;;)
    {
        digitalWrite(LED, LOW);
        delay(500);
        digitalWrite(LED, HIGH);
        delay(500);
    }
}

void Controller::run()
{
    uint LED = 2;
    pinMode(LED, OUTPUT);

    xTaskCreatePinnedToCore(
        controlLed,
        "control_led",
        1000,
        (void *)LED,
        1,
        &control_Led_Task,
        1);

    for (;;)
    {
        mpu6050->update();
        Serial.println(mpu6050->getTemp());
        delay(2000);
    }
}
