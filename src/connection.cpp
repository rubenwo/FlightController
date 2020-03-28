#include "connection.h"

std::function<void(message m)> c;

Connection::Connection(connection_config cfg)
{
    port = cfg.port;
    WiFi.begin(cfg.ssid.c_str(), cfg.pass.c_str());

    client.begin("broker.hivemq.com", net);
    client.onMessage([](String &topic, String &payload) {
        int speed = atoi(payload.c_str());

        c({speed});
        Serial.println(payload);
        digitalWrite(2, HIGH);
        delay(100);
        digitalWrite(2, LOW);
        delay(100);
        digitalWrite(2, HIGH);
        delay(100);
        digitalWrite(2, LOW);
    });

    Serial.print("checking wifi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
    }

    Serial.print("\nconnecting...");
    while (!client.connect("test12345678"))
    {
        Serial.print(".");
        delay(1000);
    }

    Serial.println("\nconnected!");

    client.subscribe("/hello");
  //  send();
    recv();
}

Connection::~Connection() {}

void Connection::recv()
{
    std::thread recvThread([this]() {
        for (;;)
        {
            client.loop();
            delay(10); // <- fixes some issues with WiFi stability

            if (!client.connected())
            {
                Serial.print("checking wifi...");
                while (WiFi.status() != WL_CONNECTED)
                {
                    Serial.print(".");
                    delay(1000);
                }

                Serial.print("\nconnecting...");
                while (!client.connect("test12345678"))
                {
                    Serial.print(".");
                    delay(1000);
                }

                Serial.println("\nconnected!");

                client.subscribe("/hello");
            }
        }
    });
    recvThread.detach();
}

void Connection::send()
{
    client.publish("/hello", "Drone is online");
}

void Connection::set_callback(std::function<void(message m)> callback)
{
    //this->callback = callback;
    c = callback;
}