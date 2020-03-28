#include "connection.h"

Connection::Connection(connection_config &cfg)
{
    port = cfg.port;
    WiFi.begin(cfg.ssid.c_str(), cfg.pass.c_str());
}

Connection::~Connection() {}

buffered_channel<data> *Connection::run()
{
    auto *input = new buffered_channel<data>();

    return input;
}

void Connection::recv(void *parameter)
{
}
void Connection::send()
{
}