#ifndef UDP_SEND_H
#define UDP_SEND_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

char *udp_send(const char *const host, const uint16_t port, const char *const cmd);

#ifdef __cplusplus
}
#endif

#endif // UDP_SEND_H