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

class Connection
{
private:
    uint port;
    WiFiServer server;

    std::function<void(message m)> callback;

public:
    Connection(connection_config cfg);
    ~Connection();

    void set_callback(std::function<void(message m)> callback);
    std::function<void(message m)> get_callback();

    void init();
    void loop();
};