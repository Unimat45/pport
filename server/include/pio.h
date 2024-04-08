#ifndef PIO_H
#define PIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void io_write(uint8_t value);
int io_init(void);

#ifdef __cplusplus
}
#endif
#endif // PIO_H