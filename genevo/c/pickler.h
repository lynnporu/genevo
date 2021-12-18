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

#include "gene_pool.h"
#include "error.h"
#include "files.h"

// Even the empty pool file should be at least 64 bits long.
#define POOL_FILE_MIN_SAFE_BIT_SIZE 64

#define POOL_INITIAL_BYTE           (uint8_t) 0xAB
#define POOL_META_INITIAL_BYTE      (uint8_t) 0xBC
#define POOL_META_TERMINAL_BYTE     (uint8_t) 0xCD
#define POOL_TERMINAL_BYTE          (uint8_t) 0xFF

#define GENOME_INITIAL_BYTE         (uint8_t) 0xA0
#define GENOME_META_INITIAL_BYTE    (uint8_t) 0xDE
#define GENOME_META_TERMINAL_BYTE   (uint8_t) 0xEF
#define GENOME_RESIDUE_BYTE         (uint8_t) 0xA2
#define GENOME_TERMINAL_BYTE        (uint8_t) 0xA1

/*

! The notation defined in gene_pool.h is used here. (So MNSb means the size of
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
    uint8_t   initial_byte;
    uint32_t  genes_number;
    uint16_t  metadata_byte_size;
    uint8_t   metadata_initial_byte;
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
    uint8_t  initial_byte;
    uint64_t organisms_number;
    uint64_t input_neurons_number;
    uint64_t output_neurons_number;
    uint8_t  node_id_part_bit_size;
    uint8_t  weight_part_bit_size;
    uint16_t metadata_byte_size;
    uint8_t  metadata_initial_byte;
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
    uint8_t *slots, uint64_t *number,
    uint8_t start, uint8_t end);

void copy_uint64_to_bitslots(
    uint64_t *number, uint8_t *slots,
    uint8_t start, uint8_t number_size
);

uint8_t * point_gene_in_genome_by_index(genome_t *, uint32_t index, pool_t *);
uint8_t * point_gene_by_index(uint8_t *genes, uint32_t index, pool_t *pool);
gene_t * get_gene_in_genome_by_index(genome_t *, uint32_t, pool_t *);
gene_t * get_gene_by_index(uint8_t *genes, uint32_t index, pool_t *);
gene_t * get_gene_by_pointer(uint8_t *gene_start_byte, pool_t *);

pool_t * read_pool(const char *address);
void close_pool(pool_t *pool);
void write_pool(const char *address, pool_t *pool, genome_t **genomes);
gene_byte_t * generate_genes_byte_array(gene_t **, pool_t *, uint64_t length);
void free_genes_byte_array(gene_byte_t *array);

genome_t * read_next_genome(pool_t *pool);
void reset_genome_cursor(pool_t *pool);

genome_t ** read_genomes(pool_t *pool);
void free_genomes_ptrs(genome_t ** genomes);
