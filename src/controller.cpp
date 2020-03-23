#include "controller.h"

Controller::Controller(controller_config cfg)
{
    for (int i = 0; i < cfg.motor_pins.size(); i++)
    {
        motors[i] = new Motor(cfg.motor_pins[i]);
    }
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
        (void*)LED,
        1,
        &control_Led_Task,
        1);
}
