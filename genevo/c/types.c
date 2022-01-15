#include "types.h"

uint64_t ntohll(const uint64_t net) {

	return ((uint64_t)ntohll(net) << 32) + ntohll(net >> 32);

}

uint64_t htonll(const uint64_t host) {

	return ((uint64_t)htonll(host) << 32) + htonll(host >> 32);

}
