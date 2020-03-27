#include <Arduino.h>
//#include "controller.h"
//Controller controller({0, 0, 0, 0});
#include <thread>

#include <WiFi.h>
#include <MQTT.h>
#include <ESP32Servo.h>
#include "motor.h"
const char *ssid = "Exogenesis_2.4";
const char *pass = "maanDag2018";

WiFiClient net;
MQTTClient client;

Motor *motor;
Motor *motor2;
const uint LED = 2;

void connect()
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
  // client.unsubscribe("/hello");
}

int parse(String &payload)
{
  return atoi(payload.c_str());
}

void runCommand(int speed)
{
  motor->setThrottle(speed);
}

void messageReceived(String &topic, String &payload)
{
  digitalWrite(LED, HIGH);
  Serial.println("incoming: " + topic + " - " + payload);
  runCommand(parse(payload));
  digitalWrite(LED, LOW);
}

void initM(Motor *m)
{
  m->init();
}

void setup()
{
  Serial.begin(115200);

  pinMode(2, OUTPUT);

  motor = new Motor(15);
  motor2 = new Motor(13);

  std::thread initT(initM, motor);
  std::thread initTT(initM, motor2);

  initT.join();
  initTT.join();

  WiFi.begin(ssid, pass);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported by Arduino.
  // You need to set the IP address directly.
  client.begin("broker.hivemq.com", net);
  client.onMessage(messageReceived);

  connect();
}

void loop()
{
  client.loop();
  delay(10); // <- fixes some issues with WiFi stability

  if (!client.connected())
  {
    connect();
  }
}