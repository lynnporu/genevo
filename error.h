#pragma once

#include <stdint.h>

static uint8_t ERROR_LEVEL = 0;


#define ERR_OK                              0x000
#define ERR_OK_STR                         "No errors."


// Gene pool file errors =======================================================

#define ERR_POOL_CORRUPT_TOO_SMALL          0x101
#define ERR_POOL_CORRUPT_TOO_SMALL_STR      "Pool file is definitely "         \
                                            "corrupted as it's length is "     \
                                            "less than 64 bits."

#define ERR_POOL_CORRUPT_INITIAL            0x102
#define ERR_POOL_CORRUPT_INITIAL_STR        "Initial byte of the gene pool "   \
                                            "was not found."

#define ERR_POOL_CORRUPT_METADATA_START     0x103
#define ERR_POOL_CORRUPT_METADATA_START_STR "Initial byte at the beginning "   \
                                            "of the gene pool metadata was "   \
                                            "not found."

#define ERR_POOL_CORRUPT_METADATA_END       0x104
#define ERR_POOL_CORRUPT_METADATA_END_STR   "Terminal byte at the beginning "  \
                                            "of the gene pool metadata was "   \
                                            "not found."

#define ERR_POOL_CORRUPT_END                0x105
#define ERR_POOL_CORRUPT_END_STR            "Terminal byte of the gene pool "  \
                                            "was not found."


// Genome errors ===============================================================

#define ERR_GENM_CORRUPT_METADATA_START     0x201
#define ERR_GENM_CORRUPT_METADATA_START_STR "Initial byte at the beginning "   \
                                            "of the genome metadata was "      \
                                            "not found."

#define ERR_GENM_CORRUPT_METADATA_END       0x202
#define ERR_GENM_CORRUPT_METADATA_END_STR   "Terminal byte at the beginning "  \
                                            "of the genome metadata was "      \
                                            "not found."


// Gene ========================================================================

#define ERR_GENE_NOT_ALIGNED                0x301
#define ERR_GENE_NOT_ALIGNED_STR            "Gene size is not aligned to 8 "   \
                                            "bites."
