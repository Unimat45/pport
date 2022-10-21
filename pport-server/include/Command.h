#include "Parallel.h"

class Command {
private:
    std::string cmd;
    std::string inst;
    int pin;
    bool state;
    std::string label;
    
    Parallel parallel;

public:
    Command(std::string cmd);

    std::string Execute();
};
