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

#define BYTE_SIZE sizeof(uint8_t)

#define IS_BIG_ENDIAN (!*(uint8_t *)&(uint16_t){1})

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

uint8_t * point_gene_by_index(genome_t *, uint32_t, pool_t *);
gene_t * get_gene_by_index(genome_t *, uint32_t, pool_t *);
gene_t * get_gene_by_pointer(uint8_t *gene_start_byte, pool_t *);

pool_t * read_pool(const char *address);
void close_pool(pool_t *pool);
void write_pool(const char *address, pool_t *pool, genome_t **genomes);

genome_t * read_next_genome(pool_t *pool);
void reset_genome_cursor(pool_t *pool);

genome_t ** read_genomes(pool_t *pool);
void free_genomes_ptrs(genome_t ** genomes);
