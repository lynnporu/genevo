#include <stdint.h>

/*

All data structures is being dumped into the file using using network byte
order which is big-endian.

*/

uint64_t ntohll(uint64_t);
uint64_t htonll(uint64_t);

#define HTON(_VARIABLE) _Generic((_VARIABLE),  \
    uint16_t: htons,                           \
    uint32_t: htonl,                           \
    uint64_t: htonll)(_VARIABLE)

#define NTOH(_VARIABLE) _Generic((_VARIABLE),  \
    uint16_t: ntohs,                           \
    uint32_t: ntohl,                           \
    uint64_t: ntohll)(_VARIABLE)
