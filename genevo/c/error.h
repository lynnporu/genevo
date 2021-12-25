/*

This header contains declarations for all error levels that are being used
across the library.

*/

#pragma once

#include <stdint.h>
#include <wchar.h>
#include <stdint.h>

typedef uint8_t err_status_t;

/* @function get_err_string
*  @return string
*  @argument uint8
*/
const char * get_err_string(const err_status_t errcode);

/* @variable ERROR_LEVEL
*  @type uint8
*/
extern err_status_t ERROR_LEVEL;


#define ERR_OK                              (err_status_t)0x00
#define ERR_OK_STR                          "No errors."


// System errors ===============================================================

#define ERR_CANNOT_MALLOC                   (err_status_t)0xf0
#define ERR_CANNOT_MALLOC_STR               "Cannot allocate memory."


// General errors ==============================================================

#define ERR_OUT_OF_BOUNDS                   (err_status_t)0xe0
#define ERR_OUT_OF_BOUNDS_STR               "You are trying to read out of "   \
                                            "array bounds."

#define ERR_INCOMPATIBLE_FLAGS              (err_status_t)0xe1
#define ERR_INCOMPATIBLE_FLAGS_STR          "You have just given "             \
                                            "incompatible flags."

#define ERR_WRONG_FLAG                      (err_status_t)0xe2
#define ERR_WRONG_FLAG_STR                  "You have just given wrong flag "  \
                                            "or mode."


// Gene pool file errors =======================================================

#define ERR_FILE_CANNOT_OPEN                (err_status_t)0x01
#define ERR_FILE_CANNOT_OPEN_STR            "Cannot open the file."

#define ERR_FILE_CANNOT_FSTAT               (err_status_t)0x02
#define ERR_FILE_CANNOT_FSTAT_STR           "Cannot get size of the file."

#define ERR_FILE_CANNOT_MMAP                (err_status_t)0x03
#define ERR_FILE_CANNOT_MMAP_STR            "Cannot create map of the file."

#define ERR_FILE_CANNOT_LSEEK               (err_status_t)0x04
#define ERR_FILE_CANNOT_LSEEK_STR           "Cannot set cursor - lseek error."

#define ERR_FILE_CANNOT_WRITE               (err_status_t)0x05
#define ERR_FILE_CANNOT_WRITE_STR           "Cannot stretch the file - write " \
                                            "error."

#define ERR_FILE_CANNOT_STRETCH_READ        (err_status_t)0x06
#define ERR_FILE_CANNOT_STRETCH_READ_STR    "Cannot set size of the file in "  \
                                            "read mode."

// Gene pool file errors =======================================================

#define ERR_POOL_CORRUPT_TOO_SMALL          (err_status_t)0x11
#define ERR_POOL_CORRUPT_TOO_SMALL_STR      "Pool file is definitely "         \
                                            "corrupted as it's length is "     \
                                            "less than 64 bits."

#define ERR_POOL_CORRUPT_INITIAL            (err_status_t)0x12
#define ERR_POOL_CORRUPT_INITIAL_STR        "Initial byte of the gene pool "   \
                                            "was not found."

#define ERR_POOL_CORRUPT_METADATA_START     (err_status_t)0x13
#define ERR_POOL_CORRUPT_METADATA_START_STR "Initial byte at the beginning "   \
                                            "of the gene pool metadata was "   \
                                            "not found."

#define ERR_POOL_CORRUPT_METADATA_END       (err_status_t)0x14
#define ERR_POOL_CORRUPT_METADATA_END_STR   "Terminal byte at the beginning "  \
                                            "of the gene pool metadata was "   \
                                            "not found."

#define ERR_POOL_CORRUPT_END                (err_status_t)0x15
#define ERR_POOL_CORRUPT_END_STR            "Terminal byte of the gene pool "  \
                                            "was not found."


// Genome errors ===============================================================

#define ERR_GENM_CORRUPT_METADATA_START     (err_status_t)0x21
#define ERR_GENM_CORRUPT_METADATA_START_STR "Initial byte at the beginning "   \
                                            "of the genome metadata was "      \
                                            "not found."

#define ERR_GENM_CORRUPT_METADATA_END       (err_status_t)0x22
#define ERR_GENM_CORRUPT_METADATA_END_STR   "Terminal byte at the end "  \
                                            "of the genome metadata was "      \
                                            "not found."

#define ERR_GENM_CORRUPT_START              (err_status_t)0x23
#define ERR_GENM_CORRUPT_START_STR          "Initial byte at the beginning "   \
                                            "of the genome was not found."

#define ERR_GENM_CORRUPT_END                (err_status_t)0x24
#define ERR_GENM_CORRUPT_END_STR            "Terminal byte at the end "  \
                                            "of the genome was not found."

#define ERR_GENM_CORRUPT_RESIDUE            (err_status_t)0x25
#define ERR_GENM_CORRUPT_RESIDUE_STR        "Residue byte was not found."

#define ERR_GENM_END_ITERATION              (err_status_t)0x26
#define ERR_GENM_END_ITERATION_STR          "End of the pool reached."


// Gene ========================================================================

#define ERR_GENE_NOT_ALIGNED                (err_status_t)0x31
#define ERR_GENE_NOT_ALIGNED_STR            "Gene size is not aligned to 8 "   \
                                            "bites."

#define ERR_GENE_OGSB_TOO_LARGE             (err_status_t)0x32
#define ERR_GENE_OGSB_TOO_LARGE_STR         "Node ID part of the gene is "     \
                                            "bigger than 64 bits. Those are  " \
                                            "not supported."

#define ERR_GENE_WEIGHT_TOO_LARGE           (err_status_t)0x33
#define ERR_GENE_WEIGHT_TOO_LARGE_STR       "Weight part of the gene is "      \
                                            "bigger than 64 bits. Those are  " \
                                            "not supported."
