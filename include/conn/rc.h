#pragma once
#include <memory>
#include <IBusBM.h>
#include <HardwareSerial.h>

struct RC_Input_Values
{
    int ch1, ch2, ch3, ch4, ch5, ch6;
};

class RC
{
private:
    IBusBM IBus;

    RC_Input_Values current_values;

public:
    RC();
    ~RC();

    RC_Input_Values get_input();
};
