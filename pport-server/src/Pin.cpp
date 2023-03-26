#include "Pin.h"

Pin::Pin() {
    this->pin = -1;
    this->is_on = false;
    this->label = "Pin " + std::to_string(pin);
    this->pin_nbr = -1;
}

Pin::Pin(short pin, bool is_on) {
    if (pin < 2 || pin > PORT_SIZE + 1) {
        throw std::runtime_error("Invalid pin number");
    }
    this->pin = pin;
    this->is_on = is_on;
    this->label = "Pin " + std::to_string(pin);
    this->pin_nbr = 1 << (this->pin - 2);
}

Pin::Pin(short pin, bool is_on, std::string label) {
    if (pin < 2 || pin > PORT_SIZE + 1) {
        throw std::runtime_error("Invalid pin number");
    }

    if (label == "") {
        label = "Pin " + std::to_string(pin);
    }

    this->pin = pin;
    this->is_on = is_on;
    this->label = label;
    this->pin_nbr = 1 << (this->pin - 2);
}

std::string Pin::ToJSONStr() {
    return toUnstyledString(this->ToJSON());
}

Json::Value Pin::ToJSON() {
    Json::Value root;

    root["label"] = this->label;
    root["pin"] = this->pin;
    root["is_on"] = this->is_on;

    return root;
}

short Pin::GetPin() {
    return this->pin;
}

bool Pin::GetState() {
    return this->is_on;
}

short Pin::GetValue() {
    return this->pin_nbr;
}

std::string Pin::GetLabel() {
    return this->label;
}

void Pin::SetState(bool state) {
    this->is_on = state;
}

void Pin::ToggleState() {
    this->is_on = !this->is_on;
}

void Pin::SetLabel(std::string label) {
    if (label == "") {
        throw std::runtime_error("Label cannot be empty");
    }

    this->label = label;
}
