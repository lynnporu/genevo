#include "types.h"

uint64_t ntohll(uint64_t const net) {

	return ((uint64_t)ntohll(net) << 32) + ntohll(net >> 32);

}

uint64_t htonll(uint64_t const host) {

	return ((uint64_t)htonll(host) << 32) + htonll(host >> 32);

}
