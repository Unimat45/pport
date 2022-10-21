#include "Parallel.h"

#include <sstream>
#include <fstream>
#include <sys/io.h>

Parallel::Parallel() {
    this->loadFromFile();
}

std::string Parallel::ToString() {
    int longest = this->findLongest();
    std::stringstream result;

    for (size_t i = 0; i < PORT_SIZE; i++) {
        result << this->pins[i].ToString(longest) << std::endl;
    }
    
    return result.str();
}

Json::Value Parallel::ToJSON() {
    Json::Value result;

    for (size_t i = 0; i < PORT_SIZE; i++) {
        result.append(this->pins[i].ToJSON());
    }

    return result;
}

std::string Parallel::ToJSONStr() {
    return toUnstyledString(this->ToJSON());
}

Pin* Parallel::operator[](int pin) {
    if (pin < 2 || pin > PORT_SIZE + 1) {
        throw std::runtime_error("Invalid pin number");
    }
    return &this->pins[pin - 2];
}

int Parallel::findLongest() {
    int longest = 0;
    
    for (size_t i = 0; i < PORT_SIZE; i++) {
        int size = this->pins[i].ToString().size();

        if (size > longest) {
            longest = size;
        }
    }
    return longest;    
}

void Parallel::WriteToFile() {
    Json::Value file;

    file["value"] = this->CalculateValue();
    
    Json::Value labels;

    for (size_t i = 0; i < PORT_SIZE; i++) {
        Json::Value label;
        label["pin"] = this->pins[i].GetPin();
        label["label"] = this->pins[i].GetLabel();

        labels.append(label);
    }

    file["labels"] = labels;

    std::ofstream f(STATE_FILE);

    f << file;

    f.close();

    // outb(file["value"].asInt(), PORT);

    std::cout << file["value"].asInt() << std::endl;
}

void Parallel::loadFromFile() {
    std::ifstream f(STATE_FILE);
    Json::Value file;

    f >> file;
    f.close();

    unsigned char data = file["value"].asInt();

    outb(data, PORT);

    Json::Value labels = file["labels"];

    for (size_t i = 0; i < PORT_SIZE; i++) {
        Json::Value label = labels[(Json::ArrayIndex)i];
        
        int pin = label["pin"].asInt();
        unsigned char value = 1 << (pin - 2);

        if (label["label"].isNull()) {
            this->pins[i] = Pin(pin, (data & value) == value);
        }
        else {
            this->pins[i] = Pin(pin, (data & value) == value, label["label"].asCString());
        }
    }
}

int Parallel::CalculateValue() {
    unsigned char result = 0;

    for (size_t i = 0; i < PORT_SIZE; i++) {
        Pin p = this->pins[i];

        if (p.GetState()) {
            result |= p.GetValue();
        }
        else {
            result &= 0xFF - p.GetValue();
        }
    }
    
    return result;
}