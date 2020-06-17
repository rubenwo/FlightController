#pragma once

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <string>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class BT_Conn
{

public:
    BT_Conn(std::string bt_name);
    ~BT_Conn();

    void init();

private:
    std::string bt_name;

    class InternalCallbacks : public BLECharacteristicCallbacks
    {
        void onRead(BLECharacteristic *pCharacteristic)
        {
            auto txValue = pCharacteristic->getValue();
            Serial.printf("onRead() => %s", txValue.c_str());
        }
        void onWrite(BLECharacteristic *pCharacteristic)
        {
            auto rxValue = pCharacteristic->getValue();
            Serial.printf("onWrite() => %s", rxValue.c_str());
        }
    };
};