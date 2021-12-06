import ctypes

from ctypes import c_uint8, c_uint16, c_uint32, c_uint64, c_void_p, c_char_p
c_uint8_p = ctypes.POINTER(c_uint8)  # equal to c_char_p

libc = ctypes.LoadLibrary("genevo.so")


class gene_struct_t(ctypes.Structure):
    _fields_ = [
        ("outcome_node_id", c_uint64),
        ("income_node_id", c_uint64),
        ("connection_type", c_uint8),
        ("weight_unnormalized", c_uint64),
        ("weight", ctypes.c_double)
    ]


class genome_struct_t(ctypes.Structure):
    _fields_ = [
        ("length", c_uint32),
        ("metadata", c_char_p),
        ("metadata_byte_size", c_uint16),
        ("genes", ctypes.POINTER(c_uint8)),
        ("residue_size_bits", c_uint16),
        ("residue", ctypes.POINTER(c_uint8))
    ]


class file_mapping_struct_t(ctypes.Structure):
    _fields_ = [
        ("descriptor", ctypes.c_int),
        ("size", ctypes.c_size_t),
        ("data", c_void_p)
    ]


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


get_err_string = ctypes.CFUNCTYPE(
    c_char_p,  # restype
    c_uint8  # errcode
)(libc.get_err_string)

write_pool = ctypes.CFUNCTYPE(
    None,  # restype
    c_char_p,  # address
    ctypes.POINTER(pool_struct_t),  # pool
    ctypes.POINTER(ctypes.POINTER(genome_struct_t))
)(libc.write_pool)

point_gene_by_index = ctypes.CFUNCTYPE(
    ctypes.POINTER(c_uint8),  # restype
    ctypes.POINTER(genome_struct_t),
    ctypes.POINTER(pool_struct_t)
)(libc.point_gene_by_index)

get_gene_by_index = ctypes.CFUNCTYPE(
    ctypes.POINTER(gene_struct_t),  # restype
    ctypes.POINTER(genome_struct_t),
    ctypes.POINTER(pool_struct_t)
)(libc.get_gene_by_index)

read_pool = ctypes.CFUNCTYPE(
    ctypes.POINTER(pool_struct_t),  # restype
    c_char_p  # address
)(libc.read_pool)

close_pool = ctypes.CFUNCTYPE(None, ctypes.POINTER(pool_struct_t))

read_next_genome = ctypes.CFUNCTYPE(
    ctypes.POINTER(genome_struct_t),  # restype
    ctypes.POINTER(pool_struct_t)
)(libc.read_next_genome)

read_genomes = ctypes.CFUNCTYPE(
    ctypes.POINTER(ctypes.POINTER(genome_struct_t)),  # restype
    ctypes.POINTER(pool_struct_t)
)

reset_genome_cursor = ctypes.CFUNCTYPE(None, ctypes.POINTER(pool_struct_t))

free_genomes_ptrs = ctypes.CFUNCTYPE(
    None,  # restype
    ctypes.POINTER(ctypes.POINTER(genome_struct_t))
)
