/*

This module contains methods for dumping the pool into file and vice versa.
For all operations network byte order (big-endian) is used, so it'll work a
little slower on little-endian platform because of byte swappings.

*/

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <arpa/inet.h>

#include "pool.h"
#include "error.h"
#include "files.h"

// Even the empty pool file should be at least 64 bits long.
#define POOL_FILE_MIN_SAFE_BIT_SIZE 64

typedef uint8_t file_control_byte_t;

#define POOL_INITIAL_BYTE           (file_control_byte_t)0xAB
#define POOL_META_INITIAL_BYTE      (file_control_byte_t)0xBC
#define POOL_META_TERMINAL_BYTE     (file_control_byte_t)0xCD
#define POOL_TERMINAL_BYTE          (file_control_byte_t)0xFF

#define GENOME_INITIAL_BYTE         (file_control_byte_t)0xA0
#define GENOME_META_INITIAL_BYTE    (file_control_byte_t)0xDE
#define GENOME_META_TERMINAL_BYTE   (file_control_byte_t)0xEF
#define GENOME_RESIDUE_BYTE         (file_control_byte_t)0xA2
#define GENOME_TERMINAL_BYTE        (file_control_byte_t)0xA1

/*

! The notation defined in pool.h is used here. (So MNSb means the size of
  metadata of a genome in bits, for example.)

Structure of the genome in the memory is the following:

Content                               Size         Note
----------                            ----------   ----------
GENOME_INITIAL_BYTE                   8            Start byte of the genome
[number of genes]                     32
MNSB                                  16           Size of the metadata which
                                                   describes this genome.
GENOME_META_INITIAL_BYTE              8            Start byte of the metadata.
MN                                    MNSb         The metadata itself
GENOME_META_TERMINAL_BYTE             8            End byte of the metadata.
[gene #1]                             2*IGSb + WGSb
[gene #2]                             2*IGSb + WGSb
...
GENOME_RESIDUE_BYTE                   8            Genome residue byte
[size of the residue in bits=RSb]     16           Size of the residual genome*.
...                                   RSb          Residual genome.
GENOME_TERMINAL_BYTE                  8            End byte of the genome

* Maximum size of a gene in bits is 255*2 + 255 = 765. Just 10 bits is enough
  To encode this number, but that would be unconvenient to put into struct, so
  16 chosen instead.

If the size of the genome cannot be aligned to size of the single gene (i.e.
genome contains some terminal bits that are not anough to form the new gene),
the residue can be placed after GENOME_RESIDUE_BYTE.

*/

typedef struct genome_preamble_s {
    file_control_byte_t initial_byte;
    uint32_t            genes_number;
    uint16_t            metadata_byte_size;
    file_control_byte_t metadata_initial_byte;
} __attribute__((packed, aligned(1))) genome_file_preamble_t;

/*

Structure of the gene pool written in the file is the following:

Content                               Size (bits)  Note
----------                            ----------   ----------
POOL_INITIAL_BYTE                     8            Initial byte used to verify
                                                   integrity of the file
[number of organisms]                 64
[number of input neurons]             64
[number of output neurons]            64
[size of the OG and IG in bits]       8
[size of the WG in bits]              8
[size of metadata in bytes = MPSB]    16           Metadata contains desription
                                                   of this gene pool
POOL_META_INITIAL_BYTE                8            Start byte of the metadata
MP                                    MPSb         The metadata itself
POOL_META_TERMINAL_BYTE               8            End byte of the metadata
[genome of the organism #1]           -
[genome of the organism #2]           -
...
POOL_TERMINAL_BYTE                    8            End byte used to verify
                                                   integrity of the file.

! Note, that OG and IG has the same size.

! Note, that size of OG in bits (OGSb) can be encoded with 8-bit number. That
  means, maximum OGSb value can be 255. Maximum value for number, which consists
  of 255 bits is 5.789604e+76, which is pretty large.

*/

typedef struct pool_file_preamble_s {
    file_control_byte_t initial_byte;
    uint64_t            organisms_number;
    uint64_t            input_neurons_number;
    uint64_t            output_neurons_number;
    uint8_t             node_id_part_bit_size;
    uint8_t             weight_part_bit_size;
    uint16_t            metadata_byte_size;
    file_control_byte_t metadata_initial_byte;
} __attribute__((packed, aligned(1))) pool_file_preamble_t;

/*

All data structures is being dumped into the file using using network byte
order which is big-endian.

*/

#define hton16 htons
#define ntoh16 ntohs
#define hton32 htonl
#define ntoh32 ntohl
#define hton64(x) ((((uint64_t)htonl(x)) << 32) + htonl((x) >> 32))
#define ntoh64(x) ((((uint64_t)ntohl(x)) << 32) + ntohl((x) >> 32))

void copy_bitslots_to_uint64(
    const uint8_t *slots, uint64_t * const number,
    uint8_t start, uint8_t end);

void copy_uint64_to_bitslots(
    const uint64_t *number, uint8_t * const slots,
    uint8_t start, uint8_t number_size
);

/* @declaration function point_gene_in_genome_by_index
*  @return uint8*
*  @argument genome*
*  @argument uint32
*  @argument pool*
*/
uint8_t * point_gene_in_genome_by_index(genome_t * const, uint32_t index, pool_t *);

/* @declaration function point_gene_by_index
*  @return uint8*
*  @argument uint8*
*  @argument uint32
*  @argument pool*
*/
uint8_t * point_gene_by_index(uint8_t * const genes, uint32_t index, pool_t * const pool);

/* @declaration function get_gene_in_genome_by_index
*  @return gene*
*  @argument genome*
*  @argument uint32
*  @argument pool*
*/
gene_t * get_gene_in_genome_by_index(genome_t * const, uint32_t, pool_t * const);

/* @declaration function get_gene_by_index
*  @return gene*
*  @argument uint8*
*  @argument uint32
*  @argument pool*
*/
gene_t * get_gene_by_index(uint8_t * const genes, uint32_t index, pool_t * const);

/* @declaration function get_gene_by_pointer
*  @return gene*
*  @argument uint8
*  @argument pool*
*/
gene_t * get_gene_by_pointer(uint8_t * const gene_start_byte, pool_t * const);

void open_file_for_pool(
    const char *address, pool_t * const pool, genome_t ** const genomes);
void close_file_for_pool(pool_t * const pool);

/* @declaration flags save_pool_flag
*  @member POOL_COPY_DATA                (1 << 0)
*  @member POOL_COPY_METADATA            (1 << 1)
*  @member POOL_ASSIGN_GENOME_POINTERS   (1 << 2)
*  @member POOL_ASSIGN_METADATA_POINTERS (1 << 3)
*  @member POOL_REWRITE_DESCRIPTION      (1 << 4)
*/
typedef uint8_t save_pool_flag_t;
#define POOL_COPY_DATA                (save_pool_flag_t)(1 << 0)
#define POOL_COPY_METADATA            (save_pool_flag_t)(1 << 1)
#define POOL_ASSIGN_GENOME_POINTERS   (save_pool_flag_t)(1 << 2)
#define POOL_ASSIGN_METADATA_POINTERS (save_pool_flag_t)(1 << 3)
#define POOL_REWRITE_DESCRIPTION      (save_pool_flag_t)(1 << 4)

/* @declaration function save_pool
*  @return void
*  @argument pool*
*  @argument genome**
*  @argument save_pool_flag
*/
void save_pool(pool_t * const, genome_t ** const, save_pool_flag_t flags);

/* @declaration function read_pool
*  @return pool*
*  @argument char*
*/
pool_t * read_pool(const char *address);

/* @declaration function write_pool
*  @return void
*  @argument char*
*  @argument pool*
*  @argument genome**
*/
void write_pool(const char *address, pool_t * const, genome_t ** const);

/* @declaration function close_pool
*  @return void
*  @argument pool*
*/
void close_pool(pool_t * const);

/* @declaration function genes_to_byte_array
*  @return uint8
*  @argument gene**
*  @argument pool*
*  @argument uint64
*/
gene_byte_t * genes_to_byte_array(gene_t **, pool_t *, uint64_t length);

/* @declaration function free_genes_byte_array
*  @return void
*  @argument uint8*
*/
void free_genes_byte_array(gene_byte_t * const);


/* @declaration function read_next_genome
*  @return genome*
*  @argument pool*
*/
genome_t * read_next_genome(pool_t * const);

/* @declaration function reset_genome_cursor
*  @return void
*  @argument pool*
*/
void reset_genome_cursor(pool_t * const);


/* @declaration function read_genomes
*  @return genome**
*  @argument pool*
*/
genome_t ** read_genomes(pool_t * const);

/* @declaration function free_genomes_ptrs
*  @return void
*  @argument genome**
*/
void free_genomes_ptrs(genome_t ** const);
