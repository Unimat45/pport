#ifndef PINEXCEPTION_H
#define PINEXCEPTION_H

#include <string>
#include <exception>

class PinException : public std::exception {
private:
    std::string msg;

public:
    PinException(size_t pin) {
        char buf[1024];
        snprintf(buf, 1024, "ERROR: Invalid pin number '%zd'", pin);
        this->msg = std::string(buf);
    }
    PinException(void) {
        this->msg = std::string("ERROR: Invalid pin number 'ALL'");
    }
    const char *what() const noexcept override {
        return msg.c_str();
    }
};

#endif // PINEXCEPTION_H