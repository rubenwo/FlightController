#include "conn/rc.h"

RC::RC()
{
    IBus.begin(Serial2, 1);
}

RC::~RC() {}

RC_Input_Values RC::get_input()
{
    values.ch0 = IBus.readChannel(0);
    values.ch1 = IBus.readChannel(1);
    values.ch2 = IBus.readChannel(2);
    values.ch3 = IBus.readChannel(3);
    return values;
}