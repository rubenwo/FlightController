#include "connection.h"
std::function<void(message m)> c;

Connection::Connection(connection_config cfg)
{
    port = cfg.port;
    WiFi.begin(cfg.ssid.c_str(), cfg.pass.c_str());
    server = WiFiServer(cfg.port);
    // Connect to Wi-Fi network with SSID and password
    Serial.print("Setting AP (Access Point)…");
    // Remove the password parameter, if you want the AP (Access Point) to be open
    WiFi.softAP(cfg.ssid.c_str(), cfg.pass.c_str());

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    server.begin();
    recv();
}

Connection::~Connection() {}

message Connection::parse(uint8_t* data, size_t size)
{
    
}

void Connection::recv()
{

    std::thread recvThread([this]() {
        for (;;)
        {

            auto client = server.accept();

            if (client)
            {
                while (client.connected())
                {

                    uint8_t prefix[4];
                    int bytes_read = 0;
                    bytes_read += client.read(prefix, 4);
                    if (bytes_read > 0)
                    {
                        int size = 0;
                        size = (size << 8) + prefix[0];
                        size = (size << 8) + prefix[1];
                        size = (size << 8) + prefix[2];
                        size = (size << 8) + prefix[3];

                        uint8_t data[size];

                        bytes_read += client.read(data, size);

                        get_callback()(parse(data, size));
                    }
                }
                client.stop();
            }
        }
    });
    recvThread.detach();
}

void Connection::send()
{
}

void Connection::set_callback(std::function<void(message m)> callback)
{
    this->callback = callback;
    c = callback;
}

std::function<void(message m)> Connection::get_callback()
{
    return callback;
}
