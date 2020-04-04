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

    Serial.println("Starting battery monitor...");
    battery = new Battery(36);
    Serial.println("Battery monitor is running");

    Serial.println("Connecting to mpu6050...");
    mpu6050 = new MPU6050(Wire);

    Wire.begin(21, 22);
    mpu6050->begin();
    mpu6050->calcGyroOffsets(true);
    Serial.println("mpu6050 connected and calibrated");

    Serial.println("Initalizing connection...");
    connection = new Connection({1337, "ESP32-Access-Point", "123456789"});
    connection->set_callback([this](message m) {
        // Serial.println("===== Begin of message =====");
        // Serial.print("speed: ");
        // Serial.println(m.speed);
        // Serial.print("angleX: ");
        // Serial.println(m.angleX);
        // Serial.print("angleY: ");
        // Serial.println(m.angleY);
        // Serial.print("angleZ: ");
        // Serial.println(m.angleZ);
        // Serial.println("===== End of message =====");

        if (m.angleX != desired_state.angleX)
            desired_state.angleX = m.angleX;
        if (m.angleY != desired_state.angleY)
            desired_state.angleY = m.angleY;
        if (m.angleZ != desired_state.angleZ)
            desired_state.angleZ = m.angleZ;
        if (!desired_state.on)
            desired_state.on = true;
        for (int i = 0; i < motors.size(); i++)
        {
            motors[i]->setThrottle(m.speed);
        }
    });
    Serial.println("Drone is online!");

    digitalWrite(2, LOW);
}

void Controller::PID_Algo()
{

    //Serial.print("Controller::PID_Algo() running in thread: ");
    //Serial.println(xPortGetCoreID());
    Serial.print("Battery level: ");
    Serial.println(battery->get_percentage());
    delay(500);
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
                controller->PID_Algo();
            };
        },
        "controller_thread",
        51200,
        static_cast<void *>(this),
        0,
        &controller_task,
        1);
}
