import enum
import itertools
import typing

from . import c_definitions


def max_for_bit(size: int) -> int:
    """Returns a number which has all `size` bits set to 1.

    Examples:
        >>> bin(max_for_bit(3))
        <<< 0b111
        >>> bin(max_for_bit(0))
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


class _BitField:

    def __init__(
        self,
        grouping: _GroupingType | int = _GroupingType.group_by_byte,
        skip_last_bits: int = 0
    ):
        raise NotImplementedError

    def __getitem__(self, key: int | slice) -> Gene:
        raise NotImplementedError


class GenomeResidue(_BitField):
    def __init__(
        self,
        bytes: list[int] | c_definitions.gene_p,
        bit_size: int
    ):
        super().__init__()
        raise NotImplementedError


class GeneSequence(_BitField):
    def __init__(
        self,
        bytes: list[Gene] | c_definitions.gene_p,
        gene_byte_size: int
    ):
        super().__init__()
        raise NotImplementedError


class Genome:
    def __init__(
        self,
        metadata: str,
        genes: GeneSequence,
        genes_residue: GenomeResidue = None,
        struct: c_definitions.genome_struct_p = None
    ):
        self._metadata = metadata
        self._genes = genes
        self._residue = genes_residue
        self._struct = struct

    @property
    def struct(self) -> c_definitions.gene_struct_p:
        return self._struct

    @classmethod
    def from_struct(self, string: c_definitions.genome_t):
        pass

    @property
    def genes(self) -> GeneSequence:
        return self._genes

    @property
    def genes_residue(self) -> GenomeResidue:
        return self._residue

    @property
    def bit_iter(self) -> typing.Generator[int]:
        yield from self._genes.bit_iter
        yield from self._residue.bit_iter

    def mutate(self):
        pass

    def copulate_with(self, other: "Genome") -> "Genome":
        pass

    def __add__(self, other: "Genome") -> "Genome":
        pass

    @staticmethod
    def copulate(*genomes: "Genome") -> "Genome":
        pass


class GenePool:

    def __init__(
        self,
        input_neurons_number: int,
        output_neurons_number: int,
        metadata: str,
        node_id_part_bit_size: int,
        weight_part_bit_size: int,
        genomes: list[Genome] = None,
        file_address: str = None,
        struct: c_definitions.pool_struct_p = None
    ):
        self._input_neurons_number = input_neurons_number
        self._output_neurons_number = output_neurons_number
        self._metadata = metadata
        self._node_id_part_bit_size = node_id_part_bit_size
        self._weight_part_bit_size = weight_part_bit_size
        self._genomes = genomes or []
        self._file_address = file_address
        self._struct = struct

    @property
    def struct(self) -> c_definitions.gene_struct_p:
        return self._struct

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
        return max_for_bit(self.gene_bits_size)

    def eliminate(self, eliminator: typing.Callable[Genome, bool]):
        pass

    def reproduce(self, kill_parents: float = 0) -> "GenePool":
        pass


# Convenient aliases
Organism = Genome
Population = GenePool
