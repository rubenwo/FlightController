#pragma once

#include <Arduino.h>
#include "channel.h"

struct data
{
};

struct connection_config
{
    uint port;
};

class Connection
{
private:
    uint port;

    void recv(void *parameter);
    void send();

public:
    Connection(connection_config &cfg);
    ~Connection();

    buffered_channel<data> *run();
};