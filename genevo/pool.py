import enum
import typing
import itertools

from . import c_definitions
from . import errors


def _max_for_bit(size: int) -> int:
    """Returns a number which has all `size` bits set to 1.

    Examples:
        >>> bin(_max_for_bit(3))
        <<< 0b111
        >>> bin(_max_for_bit(0))
        <<< 0b0
    """
    if size >= 0:
        return (1 << size) - 1
    else:
        raise ValueError("`size` should be non negative number")


class NodeConnectionType(enum.Enum):
    is_input = 0b10000000
    is_intermediate = 0b01000000
    is_output = 0b00100000


class _IterableContainer:
    """Implements slice indexing for container. Requires single _get_by_index
    to be implemented.
    """

    def _get_by_index(self, index: int):
        pass

    def __getitem__(self, key: int | slice):

        if isinstance(key, slice):
            # map list of indices with a such function which extracts an item
            # by its index
            return list(map(
                self._get_by_index,
                itertools.islice(  # produce indices for extraction
                    range(len(self)),
                    key.start, key.stop, key.step)
            ))

        else:
            return self._get_by_index(key)


class _HasStructBackend:
    """Base class for all the classes that have C struct as underlying data
    storage.
    """

    def _generate_struct(self) -> c_definitions.ctypes.Structure:
        pass

    @property
    def struct(self) -> c_definitions.ctypes.Structure:
        if not self._struct:
            self._struct = self._generate_struct()
        return self._struct


class Gene:
    def __init__(
        self,
        pool: "GenePool",
        outcome_node_id: int,
        outocome_node_type: NodeConnectionType,
        income_node_id: int,
        income_node_type: NodeConnectionType,
        weight_unnormalized: int = None,
        weight: int = None,
        struct: c_definitions.gene_struct_p = None,
        gene_bytes: c_definitions.gene_p = None
    ):

        if weight_unnormalized is None and weight is None:
            raise ValueError(
                "both `weight_unnormalized` and `weight` cannot be None")

        self._pool = pool
        self._outcome_node_id = outcome_node_id
        self._outocome_node_type = outocome_node_type
        self._income_node_id = income_node_id
        self._income_node_type = income_node_type
        self._weight_unnormalized = weight_unnormalized
        self._weight = weight
        self._struct = struct
        self._gene_bytes = gene_bytes

    @property
    def struct(self) -> c_definitions.gene_struct_p:
        return self._struct

    @property
    def weight(self) -> float:
        if self._weight is None:
            self._weight = (
                self._weight_unnormalized /
                self._pool._weight_normalization_coeff
            )
        return self._weight

    @property
    def weight_unnormalized(self) -> int:
        if self._weight_unnormalized is None:
            self._weight_unnormalized = int(
                self._weight * self._pool._weight_normalization_coeff
            )
        return self._weight_unnormalized

    @property
    def outcome_node(self) -> tuple[NodeConnectionType, int]:
        return (self._outcome_node_id, self._outocome_node_type)

    @property
    def income_node(self) -> tuple[NodeConnectionType, int]:
        return (self._income_node_id, self._income_node_type)

    @property
    def gene_bytes(self) -> c_definitions.gene_p:
        return self._gene_bytes

    @classmethod
    def from_bytes(
        pool: "GenePool", gene_bytes: c_definitions.gene_p
    ):
        raise NotImplementedError

    @classmethod
    def from_struct(
        pool: "GenePool", self, struct: c_definitions.gene_struct_p
    ):
        raise NotImplementedError


class _GroupingType(enum.Enum):
    group_by_bit = 1
    group_by_byte = 8


class _BitField(_IterableContainer):

    def __init__(
        self,
        bytes: iter[int],
        bytes_size: int = None,
        skip_last_bits: int = 0,
        grouping: _GroupingType | int = _GroupingType.group_by_byte,
        skip_byte_size_checking: bool = False
    ):
        """Creates BitField.

        Arguments:
            bytes_size: int, default = None; If None, then bytes size will be
                calculated by `len(bytes)`. It is convenient to set this
                parameter in case `bytes` is a pointer to dynamic array which
                size cannot be calculated manually.
            skip_last_bits: int, default = 0; Last given number of bits of the
                last byte wont be considered as the part of the current bit
                field.
                Suppose you have defined bit field with bytes [0xff, 0xff].
                With `skip_last_bits`=0 size of the bit field will be 16 bits,
                but with `skip_last_bits`=3 the size is equal to 13.
                This argument should be no bigger than 7.
            skip_byte_size_checking: bool, default = False; If set to False
                than each given int will be checked to have no more than 8
                significant bits.
                In case this check is disabled and one of the given numbers
                is bigger than 8 bits, any higher bits will be omited.
            grouping: int, default = _GroupingType.group_by_byte (==8); Defines
                the size of the items in the bit field. For example, if
                grouping is set to 10, than indexing inside this container will
                give 10-bit numbers.

        """
        if not (skip_last_bits < 8):
            raise ValueError(
                "you can't skip more than 7 bits of the last byte, just pass "
                "the one byte less")

        self._grouping_size = (
            grouping.value if isinstance(grouping, _GroupingType)
            else grouping
        )

        if not (self._grouping_size >= 1):
            raise ValueError("grouping size cannot be less than 1")

        self._bytes_len = bytes_size or len(bytes)
        self._bit_size = self._bytes_len * 8 - skip_last_bits

        if self._bit_size % self._grouping_size:  # != 0
            raise ValueError(
                "invalid combination of `skil_last_bits` and `grouping`; some "
                "of the last bits won't be ever indexed")

        if not skip_byte_size_checking:
            for index, byte in enumerate(self._bytes):
                if byte.bit_count() > 8:
                    raise ValueError(
                        f"number at position {index} is equal to {byte} has "
                        "more than 8 significant bits")

        self._bytes = bytes
        self._number = None
        self._skip_last_bits = skip_last_bits

    @classmethod
    def from_dynamic_array(
        cls,
        bytes: c_definitions.c_uint8_p,
        bytes_size: int,
        copy_bytes: bool = False,
        *args, **kwargs
    ):
        """Creates a bit field from pointer to uint8_t.

        Arguments:
            copy_bytes: bool, default = False; Set this to True in case the
                memory behind `bytes` will be freed soon.
            *args, **kwargs; The rest of the parameters which will be passed
                into standard constructor.
        """
        if copy_bytes:
            bytes = [
                byte.value
                for _, byte
                # zip will break after range exhaustion
                in zip(range(bytes_size), bytes)
            ]

        return cls(
            bytes=bytes,
            bytes_size=bytes_size,
            *args, **kwargs
        )

    def to_dynamic_array(self) -> tuple[int, c_definitions.c_uint8_p]:
        """Allocates dynamic array (if wasn't allocated yet) for the bytes
        inside this bit field.

        Returns:
            tuple[0], int; Size of the array.
            tuple[1], c_uint8_p; The array itself.

        """

        if isinstance(self._bytes, c_definitions.c_uint8_p):
            return self._bytes_len, self._bytes

        return self._bytes_len, (c_definitions.c_uint8_p * self._bytes_len)(
            *map(c_definitions.c_uint8, self._bytes))

    @property
    def number(self) -> int:
        if self._number is None:
            self._number = int.from_bytes(self._bytes, byteorder="big")
            self._number >>= self._skip_last_bits  # Cut off last unneeded bits
        return self._number

    @property
    def bit_length(self):
        return self._bit_size

    @property
    def groups_length(self):
        """Return number of groups inside the bit field.
        """
        return self._bit_size // self._grouping_size

    def __len__(self) -> int:
        """Returns number of groups.
        """
        return self.groups_length

    def _get_by_index(self, index: int) -> int:
        """Get a single number from bit field by its index.
        """
        bit_start = \
            self._bit_size - index * self._grouping_size - self._grouping_size

        if bit_start < 0:
            raise IndexError

        return self.number & (_max_for_bit(self._grouping_size) << bit_start)


class GenomeResidue(_BitField):
    def __init__(
        self,
        bytes: list[int] | c_definitions.gene_p,
        bit_size: int
    ):
        full_bytes_size = (bit_size // 8) + 1

        if isinstance(bytes, c_definitions.gene_p):
            bytes_list = [
                bytes[index].value
                for index in range(full_bytes_size)
            ]

        else:
            bytes_list = bytes

        super().__init__(
            bytes=bytes_list,
            skip_last_bits=(full_bytes_size * 8 - bit_size),
            grouping=_GroupingType.group_by_bit,
            skip_byte_size_checking=True
        )


class Genome(_IterableContainer, _HasStructBackend):
    def __init__(
        self,
        metadata: str,
        genes: list[Gene],
        genes_residue: GenomeResidue = None,
        struct: c_definitions.genome_struct_p = None
    ):
        self._metadata = metadata
        self._genes = genes
        self._residue = genes_residue
        self._struct = struct

    def _generate_struct(self):
        super()._generate_struct()
        raise NotImplementedError(
            "struct generation for genomes is not implemented yet")

    @property
    def residue_bit_size(self):
        return self._residue.bit_length

    @classmethod
    def from_struct(self, string: c_definitions.genome_t):
        pass

    def __len__(self) -> int:
        return len(self.genes)

    def _get_by_index(self, index: int) -> Gene:
        return self.genes[index]

    @property
    def genes(self) -> list[Gene]:
        return self._genes

    @property
    def genes_residue(self) -> GenomeResidue:
        return self._residue

    def mutate(self):
        pass

    def copulate_with(self, other: "Genome") -> "Genome":
        pass

    def __add__(self, other: "Genome") -> "Genome":
        pass

    @staticmethod
    def copulate(*genomes: "Genome") -> "Genome":
        pass


class GenePool(_IterableContainer):

    def __init__(
        self,
        input_neurons_number: int,
        output_neurons_number: int,
        metadata: str,
        node_id_part_bit_size: int,
        weight_part_bit_size: int,
        genomes: list[Genome] = None,
        struct: c_definitions.pool_struct_p = None
    ):
        self._input_neurons_number = input_neurons_number
        self._output_neurons_number = output_neurons_number
        self._metadata = metadata
        self._node_id_part_bit_size = node_id_part_bit_size
        self._weight_part_bit_size = weight_part_bit_size
        self._genomes = genomes or []
        self._struct = struct

    @classmethod
    def from_file_dump(cls, address: str) -> "GenePool":
        """Read pool from file dump.
        """
        pool_struct_p = c_definitions.read_pool(address.encode("utf-8"))
        errors.check_errors()
        pool_struct = pool_struct_p.contents

        genomes = []

        try:
            genome_struct = c_definitions.read_next_genome(pool_struct_p)
            errors.check_errors()
            genomes.append(Genome.from_struct(genome_struct))

        except StopIteration:
            pass

        finally:
            c_definitions.reset_genome_cursor(pool_struct_p)

        return cls(
            input_neurons_number=pool_struct.input_neurons_number.value,
            output_neurons_number=pool_struct.output_neurons_number.value,
            metadata=bytes(
                pool_struct.metadata[:pool_struct.metadata_byte_size.value]
            ).decode("utf-8"),
            node_id_part_bit_size=pool_struct.node_id_part_bit_size.value,
            weight_part_bit_size=pool_struct.weight_part_bit_size.value,
            genomes=genomes,
            struct=pool_struct_p
        )

    def dump_to_file(self, address: str):
        """Dump the pool into the file with given address.
        """
        c_definitions.write_pool(
            address.encode("utf-8"),
            self.struct, self.genome_structs_vector)
        errors.check_errors()

    def _generate_struct(self) -> c_definitions.pool_struct_p:
        """Generate C struct pool_struct_p for this gene pool.
        """
        metadata = self.metadata.encode("utf-8")
        return c_definitions.pointer(c_definitions.pool_struct_t(
            len(self),  # organisms_number
            self._input_neurons_number,
            self._output_neurons_number,
            len(metadata),
            metadata,
            self._node_id_part_bit_size,
            self._weight_part_bit_size,
            (self._node_id_part_bit_size * 2 + self._weight_part_bit_size),
            None,
            None,
            None
        ))

    @property
    def genome_structs_vector(self) -> c_definitions.genome_struct_p_p:
        return c_definitions.ctypes.pointer(
            (c_definitions.genome_struct_p * len(self))(*[
                genome.struct for genome in self.genomes
            ])
        )

    @property
    def gene_bits_size(self) -> int:
        raise NotImplementedError

    @property
    def gene_bytes_size(self) -> int:
        raise NotImplementedError

    @property
    def _weight_normalization_coeff(self) -> int:
        """A coefficient, which satisfies the following condition:
        > weight_unnormalized / COEFF = weight.
        """
        return _max_for_bit(self.gene_bits_size)

    def __len__(self) -> int:
        return len(self.genomes)

    def _get_by_index(self, index: int) -> Genome:
        return self.genomes[index]

    @property
    def genomes(self):
        return self._genomes

    def eliminate(self, eliminator: typing.Callable[Genome, bool]):
        pass

    def reproduce(self, kill_parents: float = 0) -> "GenePool":
        pass


# Convenient aliases
Organism = Genome
Population = GenePool
