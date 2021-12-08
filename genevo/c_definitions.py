import ctypes

from ctypes import c_uint8, c_uint16, c_uint32, c_uint64, c_void_p, c_char_p
c_uint8_p = ctypes.POINTER(c_uint8)  # equal to c_char_p

libc = ctypes.cdll.LoadLibrary("genevo/c/bin/genevo.so")

gene_p = c_uint8_p


class gene_struct_t(ctypes.Structure):
    _fields_ = [
        ("outcome_node_id", c_uint64),
        ("income_node_id", c_uint64),
        ("connection_type", c_uint8),
        ("weight_unnormalized", c_uint64),
        ("weight", ctypes.c_double)
    ]


gene_struct_p = ctypes.POINTER(gene_struct_t)


class genome_struct_t(ctypes.Structure):
    _fields_ = [
        ("length", c_uint32),
        ("metadata", c_char_p),
        ("metadata_byte_size", c_uint16),
        ("genes", c_uint8_p),
        ("residue_size_bits", c_uint16),
        ("residue", c_uint8_p)
    ]


genome_struct_p = ctypes.POINTER(genome_struct_t)
genome_struct_p_p = ctypes.POINTER(genome_struct_p)


class file_mapping_struct_t(ctypes.Structure):
    _fields_ = [
        ("descriptor", ctypes.c_int),
        ("size", ctypes.c_size_t),
        ("data", c_void_p)
    ]


file_mapping_struct_p = ctypes.POINTER(file_mapping_struct_t)


class pool_struct_t(ctypes.Structure):
    _fields_ = [
        ("organisms_number", c_uint64),
        ("input_neurons_number", c_uint64),
        ("output_neurons_number", c_uint64),
        ("metadata_byte_size", c_uint16),
        ("metadata", c_char_p),
        ("node_id_part_bit_size", c_uint8),
        ("weight_part_bit_size", c_uint8),
        ("gene_bytes_size", c_uint8),
        ("file_mapping", ctypes.POINTER(file_mapping_struct_t)),
        ("first_genome_start_position", c_void_p),
        ("cursor", c_void_p)
    ]


pool_struct_p = ctypes.POINTER(pool_struct_t)


get_err_string = libc.get_err_string
get_err_string.restype = c_char_p
get_err_string.argtypes = [
    c_uint8  # errcode
]

write_pool = libc.write_pool
write_pool.restype = None
write_pool.argtypes = [
    c_char_p,  # address
    pool_struct_p,  # pool
    genome_struct_p_p
]

point_gene_by_index = libc.point_gene_by_index
point_gene_by_index.restype = c_uint8_p
point_gene_by_index.argtypes = [
    genome_struct_p,
    pool_struct_p
]

get_gene_by_index = libc.get_gene_by_index
get_gene_by_index.restype = gene_struct_p
get_gene_by_index.argtypes = [
    genome_struct_p,
    pool_struct_p
]

get_gene_by_pointer = libc.get_gene_by_pointer
get_gene_by_pointer.restype = gene_struct_p
get_gene_by_pointer.argtypes = [
    gene_p,  # pointer
    pool_struct_p
]

read_pool = libc.read_pool
read_pool.restype = pool_struct_p
read_pool.argtypes = [
    c_char_p  # address
]

# close_pool = ctypes.CFUNCTYPE(None, pool_struct_p)

read_next_genome = libc.read_next_genome
read_next_genome.restype = genome_struct_p
read_next_genome.argtypes = [
    pool_struct_p
]

read_genomes = libc.read_genomes
read_genomes.restype = genome_struct_p_p
read_genomes.argtypes = [
    pool_struct_p
]

reset_genome_cursor = libc.reset_genome_cursor
reset_genome_cursor.restype = None
reset_genome_cursor.argtypes = [pool_struct_p]

free_genomes_ptrs = libc.free_genomes_ptrs
free_genomes_ptrs.restype = None
free_genomes_ptrs.argtypes = [
    genome_struct_p_p
]
