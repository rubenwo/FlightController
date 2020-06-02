#include "conn/connection.h"

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
        current_client = &client;
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
        current_client = nullptr;
        Serial.println("Client disconnected.");
    }
    delay(10);
}

void Connection::send_info(info_message m)
{
    if (current_client == nullptr)
        return;
    byte data[36];

    data[0] = (m.angleX >> 24) & 0xFF;
    data[1] = (m.angleX >> 16) & 0xFF;
    data[2] = (m.angleX >> 8) & 0xFF;
    data[3] = (m.angleX) & 0xFF;

    data[4] = (m.angleY >> 24) & 0xFF;
    data[5] = (m.angleY >> 16) & 0xFF;
    data[6] = (m.angleY >> 8) & 0xFF;
    data[7] = (m.angleY) & 0xFF;

    data[8] = (m.angleZ >> 24) & 0xFF;
    data[9] = (m.angleZ >> 16) & 0xFF;
    data[10] = (m.angleZ >> 8) & 0xFF;
    data[11] = (m.angleZ) & 0xFF;

    data[12] = (m.temperature >> 24) & 0xFF;
    data[13] = (m.temperature >> 16) & 0xFF;
    data[14] = (m.temperature >> 8) & 0xFF;
    data[15] = (m.temperature) & 0xFF;

    data[16] = (m.battery_level >> 24) & 0xFF;
    data[17] = (m.battery_level >> 16) & 0xFF;
    data[18] = (m.battery_level >> 8) & 0xFF;
    data[19] = (m.battery_level) & 0xFF;

    data[20] = (m.motor_0_speed >> 24) & 0xFF;
    data[21] = (m.motor_0_speed >> 16) & 0xFF;
    data[23] = (m.motor_0_speed >> 8) & 0xFF;
    data[23] = (m.motor_0_speed) & 0xFF;

    data[24] = (m.motor_1_speed >> 24) & 0xFF;
    data[25] = (m.motor_1_speed >> 16) & 0xFF;
    data[26] = (m.motor_1_speed >> 8) & 0xFF;
    data[27] = (m.motor_1_speed) & 0xFF;

    data[28] = (m.motor_2_speed >> 24) & 0xFF;
    data[29] = (m.motor_2_speed >> 16) & 0xFF;
    data[30] = (m.motor_2_speed >> 8) & 0xFF;
    data[31] = (m.motor_2_speed) & 0xFF;

    data[32] = (m.motor_3_speed >> 24) & 0xFF;
    data[33] = (m.motor_3_speed >> 16) & 0xFF;
    data[34] = (m.motor_3_speed >> 8) & 0xFF;
    data[35] = (m.motor_3_speed) & 0xFF;

    current_client->write(data, 36);
    current_client->flush();
}

void Connection::set_callback(std::function<void(message m)> callback)
{
    this->callback = callback;
}

std::function<void(message m)> Connection::get_callback()
{
    return callback;
}
