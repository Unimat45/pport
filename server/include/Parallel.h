#ifndef PARALLEL_H
#define PARALLEL_H

#include <cstdint>
#include <string>
#include <vector>

#ifndef PARALLEL_SIZE
#define PARALLEL_SIZE 8
#endif

constexpr uint8_t ON = 1;
constexpr uint8_t OFF = 0;

typedef struct Pin {
    uint8_t state;
    std::string label;
} Pin;

class Parallel {
private:
    Pin *parallel[PARALLEL_SIZE];

    uint8_t bit_value(void);

public:
    Parallel(void);
    ~Parallel(void);

    void toggle_pin(size_t pin);
    void set_pin(size_t pin, uint8_t value);

    void set_label(size_t pin, std::string label);

    void set_all(uint8_t value);
    void toggle_all(void);

    std::vector<uint8_t> as_memory(void);
    std::vector<uint8_t> operator[](size_t pin);

    void save_to_cfg(void);
};


#endif // PARALLEL_H
