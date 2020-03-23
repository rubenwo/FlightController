#include "connection.h"

Connection::Connection(connection_config &cfg)
{
    this->port = cfg.port;
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