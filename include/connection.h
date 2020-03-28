#pragma once

#include <Arduino.h>
#include <WiFi.h>

#include "channel.h"

struct data
{
};

struct connection_config
{
    uint port;
    std::string ssid;
    std::string pass;
};

class Connection
{
private:
    uint port;
    WiFiClient net;

    void recv(void *parameter);
    void send();

public:
    Connection(connection_config &cfg);
    ~Connection();

    buffered_channel<data> *run();
};