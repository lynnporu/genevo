#pragma once

#include <stdint.h>
#include <wchar.h>
#include <stdint.h>

static uint8_t ERROR_LEVEL = 0;
static inline const char * get_err_string(const uint8_t errcode);


#define ERR_OK                              0x00
#define ERR_OK_STR                         "No errors."


// System errors ===============================================================

#define ERR_CANNOT_MALLOC                  0xf0
#define ERR_CANNOT_MALLOC_STR              "Cannot allocate memory."


// Gene pool file errors =======================================================

#define ERR_FILE_CANNOT_OPEN                0x01
#define ERR_FILE_CANNOT_OPEN_STR            "Cannot open the file."

#define ERR_FILE_CANNOT_FSTAT               0x02
#define ERR_FILE_CANNOT_FSTAT_STR           "Cannot get size of the file."

#define ERR_FILE_CANNOT_MMAP                0x03
#define ERR_FILE_CANNOT_MMAP_STR            "Cannot create map of the file."

// Gene pool file errors =======================================================

#define ERR_POOL_CORRUPT_TOO_SMALL          0x11
#define ERR_POOL_CORRUPT_TOO_SMALL_STR      "Pool file is definitely "         \
                                            "corrupted as it's length is "     \
                                            "less than 64 bits."

#define ERR_POOL_CORRUPT_INITIAL            0x12
#define ERR_POOL_CORRUPT_INITIAL_STR        "Initial byte of the gene pool "   \
                                            "was not found."

#define ERR_POOL_CORRUPT_METADATA_START     0x13
#define ERR_POOL_CORRUPT_METADATA_START_STR "Initial byte at the beginning "   \
                                            "of the gene pool metadata was "   \
                                            "not found."

#define ERR_POOL_CORRUPT_METADATA_END       0x14
#define ERR_POOL_CORRUPT_METADATA_END_STR   "Terminal byte at the beginning "  \
                                            "of the gene pool metadata was "   \
                                            "not found."

#define ERR_POOL_CORRUPT_END                0x15
#define ERR_POOL_CORRUPT_END_STR            "Terminal byte of the gene pool "  \
                                            "was not found."


// Genome errors ===============================================================

#define ERR_GENM_CORRUPT_METADATA_START     0x21
#define ERR_GENM_CORRUPT_METADATA_START_STR "Initial byte at the beginning "   \
                                            "of the genome metadata was "      \
                                            "not found."

#define ERR_GENM_CORRUPT_METADATA_END       0x22
#define ERR_GENM_CORRUPT_METADATA_END_STR   "Terminal byte at the beginning "  \
                                            "of the genome metadata was "      \
                                            "not found."


// Gene ========================================================================

#define ERR_GENE_NOT_ALIGNED                0x31
#define ERR_GENE_NOT_ALIGNED_STR            "Gene size is not aligned to 8 "   \
                                            "bites."
