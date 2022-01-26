#pragma once

#include <structure.hpp>

/*

Simple network structure
==========================

Simple network's gene defines a connection between two neurons. All the neurons
are enumerated by the ID. Connection's strength is defined by the float number
in the range [-1; 1].

Each gene contains 3 numbers: ID of outcome node, ID of income node, weight
(the first two has the same bit width). Node capacity of the network is defined
by the largest number 'ID of ... node' can hold.

Let's say, each node has 2.5 connections average (income and outcome + some
possibility of connections to other neurons). That means, for pool of 100
neurons we'll get 250 connections. Let's call it 'density'. Knowing gene size,
nodes capacity and the density we can calculate the size of the genome. Those
are given in 'den=...' columns in the table below.

                          Types of simple network

Name       Constant       Node        Byte    den=1.5    den=2.5    den=4.0
                          capacity    size
------     -----------    ----------  ------  ---------  ---------  ---------
plant      gene_Sn_5_6    32          2       96 b       160 b      0.25 Kb
roundworm  gene_Sn_9_14   512         4       ~3 Kb      ~5.2 Kb    ~8.2 Kb
leech      gene_Sn_14_20  ~16K        6       144 Kb     240 Kb     384 Kb
lobster    gene_Sn_17_22  ~1.3M       7       ~1.4 Mb    ~2.9 Mb    3.5 Mb
guppy      gene_Sn_17_22  ~4.2M       8       48 Mb      80 Mb      128 Mb
frog       gene_Sn_22_20  ~16.8M      9       216 Mb     360 Mb     576 Mb
cat        gene_Sn_30_28  ~1B         11      ~16.5 Gb   ~27.5 Gb   44 Gb

Plants don't actually have any brain, this type of the network was added just
for fun.

                     Instance identifiers of this network

Name       Id (HEX)
------     -----------
plant      01000506
roundworm  0100090e
leech      01000e14
lobster    01001116
guppy      01001614
frog       01001818
cat        01001e1c

 */

namespace simple_network {

typedef GeneStructure<3> GeneSn;

#define DECLARE(_HUMAN_NAME, _NODE_ID_WIDTH, _WEIGHT_WIDTH)                    \
    constexpr GeneSn gene_ ## _NODE_ID_WIDTH ## _ ## _WEIGHT_WIDTH = {         \
        .id = GENE_STRUCTURE_ID(                                               \
            simple_network::class_id, (_NODE_ID_WIDTH << 8) | _WEIGHT_WIDTH),  \
        .capacity = 3,                                                         \
        .numbers = { UNSIGNED_NORMALIZED(_NODE_ID_WIDTH),                      \
                     UNSIGNED_NORMALIZED(_NODE_ID_WIDTH),                      \
                     SIGNED_DENORMALIZED(_WEIGHT_WIDTH) }};                    \
    constexpr GeneSn                                                           \
    gene_ ## _HUMAN_NAME = gene_ ## _NODE_ID_WIDTH ## _ ## _WEIGHT_WIDTH;

constexpr gene_structure_class_t class_id = 1;

// This is just for fun
// Size = 2 bytes, capacity = 32 nodes, weight precision = 0.15625
DECLARE(plant, 5, 6);

// Caenorhabditis elegans, 302 neurons
// Size = 4 bytes, capacity = 512 nodes, weight precision = 6.106e-5
DECLARE(roundworm, 9, 14);

// Hirudinea (Leech), ~10e3 neurons
// Size = 6 bytes, capacity = 16_384 nodes, weight precision = 9.536e-7
DECLARE(leech, 14, 20);

// Nephropidae (Lobster), ~100e3 neurons
// Size = 7 bytes, capacity = 1_310_72 nodes, weight precision = 2.384e-7
DECLARE(lobster, 17, 22);

// Poecilia reticulata (Guppy fish), ~4.3e6 neurons
// Size = 8 bytes, capacity = 4_194_304 nodes, weight precision = 9.536e-7
DECLARE(guppy, 22, 20);

// Anura (Frog), ~16e6 neurons
// Size = 9 bytes, capacity = 16_777_216 nodes, weight precision = 5.960e-8
DECLARE(frog, 24, 24);

// Felis catus (Cat), ~760e6
// Size = 11 bytes, capacity = 1_073_741_824 nodes, weight precision = 3.725e-9
DECLARE(cat, 30, 28);

#undef DECLARE

}
