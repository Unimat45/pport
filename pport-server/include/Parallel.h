#include "Pin.h"

#define PORT 0x378
#define STATE_FILE "/etc/pport.state"

class Parallel {

private:
    Pin pins[PORT_SIZE];

    void loadFromFile();

public:
    Parallel();

    Pin* operator[](int pin);
    
    std::string ToJSONStr();
    Json::Value ToJSON();

    void WriteToFile();
    int CalculateValue();
};