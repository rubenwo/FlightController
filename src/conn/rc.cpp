#include "conn/rc.h"

RC::RC()
{
    IBus.begin(Serial2, 1);
}

RC::~RC() {}

RC_Input_Values RC::get_input()
{
    auto new_values = current_values;
    for (auto i = 0; i < 6; i++)
    {
        int val = IBus.readChannel(i);
        switch (i)
        {
        case 0:
            if (current_values.ch1 != val)
                new_values.ch1 = val;
            break;
        case 1:
            if (current_values.ch2 != val)
                new_values.ch2 = val;
            break;
        case 2:
            if (current_values.ch3 != val)
                new_values.ch3 = val;
            break;
        case 3:
            if (current_values.ch4 != val)
                new_values.ch4 = val;
            break;
        case 4:
            if (current_values.ch5 != val)
                new_values.ch5 = val;
            break;
        case 5:
            if (current_values.ch6 != val)
                new_values.ch6 = val;
            break;
        }
    }
    current_values = new_values;
    return new_values;
}