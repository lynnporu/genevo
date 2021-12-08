import typing
import enum

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
        struct: c_definitions.gene_struct_t = None,
        gene_bytes: c_definitions.gene_p = None
    ):

        if weight_unnormalized is None and weight is None:
            raise ValueError(
                "bot `weight_unnormalized` and `weight` cannot be None")

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
    def outcome_node(self) -> typing.Tuple[NodeConnectionType, int]:
        return (self._outcome_node_id, self._outocome_node_type)

    @property
    def income_node(self) -> typing.Tuple[NodeConnectionType, int]:
        return (self._income_node_id, self._income_node_type)

    @property
    def struct(self) -> c_definitions.gene_struct_t:
        return self._struct

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
        pool: "GenePool", self, struct: c_definitions.gene_struct_t
    ):
        raise NotImplementedError


class Genome:
    def __init__(
        self,
        metadata: str,
        genes: typing.Union[typing.List[Gene], typing.List[int]],
        residue: typing.List[int]
    ):
        self.metadata = metadata
        self.genes = genes
        self.residue = residue

    @classmethod
    def from_struct(self, string: c_definitions.genome_t):
        pass


class GenePool:

    def __init__(
        self,
        input_neurons_number: int,
        output_neurons_number: int,
        metadata: str,
        node_id_part_bit_size: int,
        weight_part_bit_size: int,
        genomes: typing.List[Genome] = None,
        file_address: str = None
    ):
        self.input_neurons_number = input_neurons_number
        self.output_neurons_number = output_neurons_number
        self.metadata = metadata
        self.node_id_part_bit_size = node_id_part_bit_size
        self.weight_part_bit_size = weight_part_bit_size
        self.genomes = genomes or []
        self.file_address = file_address

    @property
    def gene_bits_size(self):
        raise NotImplementedError

    @property
    def gene_bytes_size(self):
        raise NotImplementedError

    @property
    def _weight_normalization_coeff(self):
        """A coefficient, which satisfies the following condition:
        > weight_unnormalized / COEFF = weight.
        """
        return max_for_bit(self.gene_bits_size)
