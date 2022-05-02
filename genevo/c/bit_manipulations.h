#pragma once

#include <stdint.h>
#include <math.h>

#define BITS_TO_BYTES(_BITS_NUM) (uint32_t)ceil((_BITS_NUM) / 8)
#define BITS_TO_BYTES_REMAINDER(_BITS_NUM) ((_BITS_NUM) % 8)
#define BYTES_TO_BITS(_BYTES_NUM) ((_BYTES_NUM) * 8)

#define MAX_FOR_64 0xffffffffffffUL
#define MAX_FOR_32 0xffffff

#define MAX_FOR_BIT_WIDTH(_BIT_SIZE) \
    ((_BIT_SIZE) == 64 ? MAX_FOR_64 : (1 << (_BIT_SIZE)) - 1)

/* Convert any integer with fixed bit width to one of range [-1; 1] */
#define NORMALIZE_FROM_BIT_WIDTH(_NUMBER, _BIT_SIZE) \
    ((double)(_NUMBER) / (double)(MAX_FOR_BIT_WIDTH(_BIT_SIZE)))

/* Does the opposite to NORMALIZE_FROM_BIT_WIDTH. */
#define DENORMALIZE_TO_BIT_WIDTH(_NUMBER, _BIT_SIZE) \
    (uint64_t)((double)(_NUMBER) * (double)(MAX_FOR_BIT_WIDTH(_BIT_SIZE)))
