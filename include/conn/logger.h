#pragma once
#include <Arduino.h>
#include <string>
#include <sstream>
#include <memory>
class Logger
{
public:
    Logger(HardwareSerial &serial) { stream.reset(&serial); }
    ~Logger() { buffer.clear(); }

    void stage_msg(const std::string &s, bool new_line = true, bool immediate = false)
    {
        buffer << s;
        if (new_line)
            buffer << "\n";
        if (immediate)
            push();
    }
    void push()
    {
        auto str = buffer.str().c_str();
        stream->print(str);
        stream->flush();
        buffer.str("");
    }

    template <typename T>
    static inline std::string to_string(const T &n)
    {
        std::ostringstream stm;
        stm << n;
        return stm.str();
    }

private:
    std::unique_ptr<HardwareSerial> stream;
    std::stringstream buffer;
};