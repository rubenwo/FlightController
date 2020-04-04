#include "connection.h"

Connection::Connection(connection_config cfg)
{

    // Connect to Wi-Fi network with SSID and password
    Serial.print("Setting AP (Access Point)…");
    // Remove the password parameter, if you want the AP (Access Point) to be open
    WiFi.softAP(cfg.ssid.c_str(), cfg.pass.c_str());

    port = cfg.port;
    server = WiFiServer(cfg.port);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    server.begin();
}

Connection::~Connection() {}

void Connection::init()
{
}

int32_t bytes_to_int32(byte data[4])
{
    int n = 0;
    n = (n << 8) + data[0];
    n = (n << 8) + data[1];
    n = (n << 8) + data[2];
    n = (n << 8) + data[3];

    return n;
}

float_t bytes_to_float(byte data[4])
{
    float n = 0;
    n = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];
    return n;
}

void Connection::loop()
{
    auto client = server.accept();

    if (client)
    {
        Serial.println("New Client."); // print a message out in the serial port

        while (client.connected())
        {
            if (client.available())
            {
                int bytes_read = 0;
                byte data[16];
                bytes_read += client.readBytes(data, 16);
                if (bytes_read > 0)
                {
                    byte speed[4], angleX[4], angleY[4], angleZ[4];
                    for (auto i = 0; i < 4; i++)
                        speed[i] = data[i];
                    for (auto i = 0; i < 4; i++)
                        angleX[i] = data[i + 4];
                    for (auto i = 0; i < 4; i++)
                        angleY[i] = data[i + 8];
                    for (auto i = 0; i < 4; i++)
                        angleZ[i] = data[i + 12];
                    callback(
                        {bytes_to_int32(speed),
                         static_cast<float>(bytes_to_int32(angleX)),
                         static_cast<float>(bytes_to_int32(angleY)),
                         static_cast<float>(bytes_to_int32(angleZ))});
                }
            }
        }
        client.stop();
        Serial.println("Client disconnected.");
    }
    delay(10);
}

void Connection::set_callback(std::function<void(message m)> callback)
{
    this->callback = callback;
}

std::function<void(message m)> Connection::get_callback()
{
    return callback;
}
