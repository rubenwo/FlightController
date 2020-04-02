#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <MQTT.h>
#include <thread>

struct connection_config
{
    uint port;
    std::string ssid;
    std::string pass;
};

struct message
{
    int speed;
};

class Connection
{
private:
    uint port;
    MQTTClient client;
    WiFiServer server;

    std::function<void(message m)> callback;

    void recv();
    void send();
    void messageReceived(String &topic, String &payload);
    message parse(uint8_t *data, size_t size);

public:
    Connection(connection_config cfg);
    ~Connection();

    void set_callback(std::function<void(message m)> callback);
    std::function<void(message m)> get_callback();
};