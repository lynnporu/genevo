#pragma once

#include <stdint.h>
#include <math.h>

#define BITS_TO_BYTES(_BITS_NUM) (uint32_t)ceil((_BITS_NUM) / 8)
#define BITS_TO_BYTES_REMAINDER(_BITS_NUM) ((_BITS_NUM) % 8)
#define BYTES_TO_BITS(_BYTES_NUM) ((_BYTES_NUM) * 8)
