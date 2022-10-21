#include <iostream>
#include "Utils.h"

#define PORT_SIZE 8

class Pin {

private:
    std::string label;
    short pin;
    bool is_on;
    short pin_nbr;

public:
    Pin();
    Pin(short pin, bool is_on);
    Pin(short pin, bool is_on, std::string label);

    std::string ToJSONStr();
    Json::Value ToJSON();
    std::string ToString();
    std::string ToString(size_t longest);

    short GetPin();
    bool GetState();
    short GetValue();
    std::string GetLabel();

    void SetState(bool state);
    void ToggleState();

    void SetLabel(std::string label);
};