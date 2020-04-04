#pragma once

#include <Arduino.h>
#include <WiFi.h>

struct connection_config
{
    uint port;
    std::string ssid;
    std::string pass;
};

struct message
{
    int speed;
    float angleX;
    float angleY;
    float angleZ;
};

struct info_message
{
    int angleX;
    int angleY;
    int angleZ;

    int temperature;

    int battery_level;

    int motor_0_speed;
    int motor_1_speed;
    int motor_2_speed;
    int motor_3_speed;
};

class Connection
{
private:
    uint port;
    WiFiServer server;
    WiFiClient *current_client = nullptr;

    std::function<void(message m)> callback;

public:
    Connection(connection_config cfg);
    ~Connection();

    void set_callback(std::function<void(message m)> callback);
    std::function<void(message m)> get_callback();

    void send_info(info_message m);

    void init();
    void loop();
};