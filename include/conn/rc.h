#pragma once
#include <memory>
#include <IBusBM.h>
#include <HardwareSerial.h>

struct RC_Input_Values
{
    int ch0, ch1, ch2, ch3;
};

class RC
{
private:
    IBusBM IBus;

    RC_Input_Values values;

public:
    RC();
    ~RC();

    RC_Input_Values get_input();
};
