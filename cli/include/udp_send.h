#ifndef UDP_SEND_H
#define UDP_SEND_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *udp_send(const char *const host, const uint16_t port, const char *const cmd, size_t *res_len);

#ifdef __cplusplus
}
#endif

#endif // UDP_SEND_H
