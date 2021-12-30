import abc
import enum
import typing

from . import definitions
from . import errors
from . import lazy
from . import containers
from . import bits


class NodeConnectionType(enum.Enum):
    is_input = 0b10000000
    is_intermediate = 0b01000000
    is_output = 0b00100000


_OUTCOME_CONNECTION_TYPE_BITMASK = 0b11100000
_INCOME_CONNECTION_TYPE_BITMASK = 0b00011100


class _HasStructBackend(metaclass=abc.ABCMeta):
    """Base class for all the classes that have C struct as underlying data
    storage.
    """

    @abc.abstractmethod
    def _generate_struct_ref(self):
        """Assigns self._struct_ref with a generated struct.
        """
        pass

    @abc.abstractclassmethod
    def from_struct(self, *args, **kwargs):
        pass

    def refresh_struct(self):
        """Deletes the struct, so new one will be generated next time you
        access `struct` property.
        """
        self._struct_ref = None

    @property
    def struct_ref(self):
        if not self._struct_ref:
            self._struct_ref = self._generate_struct_ref()
        return self._struct_ref

    @property
    def struct(self) -> definitions.ctypes.Structure:
        return self.struct_ref.contents


class Gene(_HasStructBackend):
    def __init__(
        self,
        pool: "GenePool",
        outcome_node_id: int,
        outcome_node_type: NodeConnectionType,
        income_node_id: int,
        income_node_type: NodeConnectionType,
        weight_unnormalized: int = None,
        weight: int = None,
        struct_ref: definitions.libc.gene_p = None,
        gene_bytes: definitions.libc.gene_byte_p = None
    ):

        if weight_unnormalized is None and weight is None:
            raise ValueError(
                "both `weight_unnormalized` and `weight` cannot be None")

        self._pool = pool
        self._outcome_node_id = outcome_node_id
        self._outcome_node_type = outcome_node_type
        self._income_node_id = income_node_id
        self._income_node_type = income_node_type
        self._weight_unnormalized = weight_unnormalized
        self._weight = weight
        self._struct_ref = struct_ref
        self._gene_bytes = gene_bytes

    def __repr__(self):
        return (
            f"<Gene "
            f"{self._outcome_node_id} -> {self._income_node_id}, "
            f"weight={self._weight}>")

    def _generate_struct_ref(self) -> definitions.libc.gene_p:
        return definitions.ctypes.pointer(definitions.libc.gene(
            # outcome_node_id
            (self._outcome_node_id +
             self.pool.range_starts[self._outcome_node_type]),
            # income_node_id
            (self._income_node_id +
             self.pool.range_starts[self._income_node_type]),
            # connection_type
            self.outcome_node_type + self.income_node_type >> 3,
            self.weight_unnormalized,
            self.weight
        ))

    @classmethod
    def from_dynamic_array(
        cls, pool: "GenePool", gene_bytes: definitions.libc.gene_byte_p,
        index_offset: int = -1
    ):
        """Create class instance from pointer to the vector of gene bytes.

        Arguments:
            pool: GenePool
            gene_bytes: definitions.libc.gene_byte_p aka POINTER(c_uint8)
            index_offset: int, default = -1; If set to non-negative integer,
                then index offset will be used.

        """
        struct_ref = (
            definitions.libc.get_gene_by_pointer(gene_bytes, pool)
            if index_offset < 0 else
            definitions.libc.get_gene_by_index(
                gene_bytes, index_offset, pool)
        )

        return cls.from_struct(
            pool=pool,
            struct_ref=struct_ref
        )

    @classmethod
    def from_struct(
        cls, pool: "GenePool", struct_ref: definitions.libc.gene_p
    ):
        struct = struct_ref.contents

        connection_type = struct.connection_type
        outcome_node_type = NodeConnectionType(
            connection_type & _OUTCOME_CONNECTION_TYPE_BITMASK)
        income_node_type = NodeConnectionType(
            (connection_type & _INCOME_CONNECTION_TYPE_BITMASK) << 3)

        return cls(
            pool=pool,
            outcome_node_id=struct.outcome_node_id,
            outcome_node_type=outcome_node_type,
            income_node_id=struct.income_node_id,
            income_node_type=income_node_type,
            weight_unnormalized=struct.weight_unnormalized,
            weight=struct.weight,
            struct_ref=struct_ref,
            gene_bytes=None
        )

    @property
    def pool(self):
        return self._pool

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
    # For Python3.10
    # def outcome_node(self) -> tuple[NodeConnectionType, int]:
    def outcome_node(self) -> typing.Tuple[NodeConnectionType, int]:
        return (self._outcome_node_id, self._outcome_node_type)

    @property
    # For Python3.10
    # def income_node(self) -> tuple[NodeConnectionType, int]:
    def income_node(self) -> typing.Tuple[NodeConnectionType, int]:
        return (self._income_node_id, self._income_node_type)

    @property
    def gene_bytes(self) -> definitions.libc.gene_byte_p:

        if self._gene_bytes is None:
            self._gene_bytes = definitions.libc.generate_genes_byte_array(
                self.struct_ref, self._pool.struct_ref, 1)

        return self._gene_bytes


class GenomeResidue(bits._BitField):

    @classmethod
    def from_dynamic_array(
        cls,
        byte_array: definitions.c_uint8_p,
        bit_size: int,
        copy_bytes: bool = False,
        *args, **kwargs
    ):

        full_bytes_size = (bit_size // 8) + 1

        return super().from_dynamic_array(
            byte_array=byte_array,
            copy_bytes=copy_bytes,
            bytes_size=full_bytes_size,
            skip_last_bits=(full_bytes_size * 8 - bit_size),
            grouping=bits._GroupingType.group_by_bit,
            skip_byte_size_checking=True
        )


class Genome(containers._LazyIterableContainer, _HasStructBackend):
    def __init__(
        self,
        pool: "GenePool",
        metadata: str,
        # genes: list[Gene],  # For Python3.10
        genes: typing.Union[None, typing.List[Gene]],
        genes_residue: GenomeResidue = None,
        genome_struct_ref: definitions.libc.genome_p = None,
    ):
        """
        Pass either genes and genes_residue or genome_struct_ref.
        """

        super().__init__()

        self._metadata = metadata

        if (genes or genes_residue) and genome_struct_ref:
            raise TypeError(
                "Pass either `genes` and `genes_residue` or "
                "`genome_struct_ref`")

        if bool(genes) != bool(genes_residue):
            raise TypeError(
                "Both `genes` and `genes_residue` should be set or undefined.")

        self._struct_ref = genome_struct_ref
        self._pool = pool

        if genome_struct_ref:
            self._iter_caching_on = True
            self._residue = GenomeResidue.from_dynamic_array(
                byte_array=genome_struct_ref.contents.residue,
                bit_size=genome_struct_ref.contents.residue_size_bits,
                copy_bytes=False
            )
        else:
            self._iter_caching_on = False
            self._residue = genes_residue

        self._genes = genes

    def __repr__(self):
        return f"<Genome with {len(self)} genes>"

    def _generate_struct_ref(self) -> definitions.libc.genome_p:
        super()._generate_struct_ref()
        metadata = self._metadata.encode("utf-8")
        return definitions.ctypes.pointer(definitions.libc.genome(
            len(self),
            metadata,
            len(metadata),
            self._gene_bytes,
            self._residue.bit_length,
            self._residue.to_dynamic_array()
        ))

    def _iter_function(self, index: int) -> Gene:
        struct_ref = definitions.libc.get_gene_in_genome_by_index(
            self.struct_ref, index, self.pool.struct_ref)
        return Gene.from_struct(pool=self.pool, struct_ref=struct_ref)

    @property
    def pool(self):
        return self._pool

    @property
    # For Python3.10
    # def _gene_bytes(self) -> tuple[int, definitions.libc.gene_byte_p]:
    def _gene_bytes(self) -> typing.Tuple[int, definitions.libc.gene_byte_p]:
        """Returns array if bytes for genes. If the genome has no its struct
        yet, then new array will be allocated.
        """
        if self._struct_ref:
            return self.struct.genes

        else:
            return definitions.libc.generate_genes_byte_array(
                (definitions.libc.gene_p * len(self))(
                    *[gene.struct_ref for gene in self.genes],
                    self._pool._struct,
                    len(self.genes)
                )
            )

    @property
    def residue_bit_size(self):
        return self._residue.bit_length

    @classmethod
    def from_struct(
        cls,
        pool: "GenePool",
        genome_struct_ref: definitions.libc.genome_p
    ):
        genome_struct = genome_struct_ref.contents

        return cls(
            pool=pool,
            metadata=bytes(
                genome_struct.metadata[:genome_struct.metadata_byte_size]
            ).decode("utf-8"),
            genes=None,
            genes_residue=None,
            genome_struct_ref=genome_struct_ref
        )

    def __len__(self) -> int:
        return len(self.genes)

    def _get_by_index(self, index: int) -> Gene:
        return self.genes[index]

    @property
    # def genes(self) -> list[Gene]:  # For Python3.10
    def genes(self) -> typing.List[Gene]:
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


class GenePool(containers._IterableContainer, _HasStructBackend):

    def __init__(
        self,
        input_neurons_number: int,
        output_neurons_number: int,
        metadata: str,
        node_id_part_bit_size: int,
        weight_part_bit_size: int,
        # genomes: list[Genome] = None,  # For Python3.10
        genomes: typing.List[Genome] = None,
        struct_ref: definitions.libc.pool_p = None
    ):
        # ! genomes may be not initialized yet, if class was created by
        # ! from_file_dump method

        self._input_neurons_number = input_neurons_number
        self._output_neurons_number = output_neurons_number
        self._metadata = metadata
        self._node_id_part_bit_size = node_id_part_bit_size
        self._weight_part_bit_size = weight_part_bit_size
        self._genomes = genomes or []
        self._struct_ref = struct_ref

        self._range_starts = None

    def __del__(self):
        if self.struct_ref is not None:
            definitions.libc.close_pool(self.struct_ref)

    def __repr__(self):
        return f"<GenePool with {len(self)} genomes>"

    @classmethod
    def from_struct(cls, struct_ref: definitions.libc.pool):

        struct = struct_ref.contents

        instance = cls(
            input_neurons_number=struct.input_neurons_number,
            output_neurons_number=struct.output_neurons_number,
            metadata=bytes(
                struct.metadata[:struct.metadata_byte_size]
            ).decode("utf-8"),
            node_id_part_bit_size=struct.node_id_part_bit_size,
            weight_part_bit_size=struct.weight_part_bit_size,
            genomes=[],
            struct_ref=struct_ref
        )

        genomes = []

        try:
            genome_struct_p = definitions.libc.read_next_genome(struct_ref)
            errors.check_errors()  # may raise StopIteration
            genomes.append(Genome.from_struct(
                pool=instance, genome_struct_ref=genome_struct_p
            ))

        except StopIteration:
            pass

        finally:
            definitions.libc.reset_genome_cursor(struct_ref)

        instance._genomes = genomes

        return instance

    @classmethod
    def from_file_dump(cls, address: str) -> "GenePool":
        """Read pool from file dump.
        """
        pool_struct_p = definitions.libc.read_pool(address.encode("utf-8"))
        errors.check_errors()
        return cls.from_struct(pool_struct_p)

    def dump_to_file(self, address: str):
        """Dump the pool into the file with given address.
        """
        definitions.libc.write_pool(
            address.encode("utf-8"),
            self.struct_ref, self.genome_structs_vector)
        errors.check_errors()

    def _generate_struct_ref(self) -> definitions.libc.pool_p:
        """Generate C struct pool_struct_p for this gene pool.
        """
        metadata = self.metadata.encode("utf-8")
        return definitions.ctypes.pointer(definitions.libc.pool(
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
    def input_neurons_number(self):
        return self._input_neurons_number

    @property
    def output_neurons_number(self):
        return self._output_neurons_number

    @property
    def nodes_capacity(self) -> int:
        return bits._max_for_bit(self._node_id_part_bit_size)

    @property
    def node_id_part_bit_size(self) -> int:
        return self._node_id_part_bit_size

    @property
    def weight_part_bit_size(self) -> int:
        return self._weight_part_bit_size

    @property
    def range_starts(self) -> dict:
        """Returns start id of range if input, intermediate and output nodes.

        For example, if node_id_part_bit_size has 10 bits, then the model
        capacity is ((1<<10) - 1) = 1023 neurons.
        Lets suppose you've set number of input neurons to be 100 and number of
        output to be 200. In this case you'll have the following ranges:
        {
            NodeConnectionType.is_input: 0,
            NodeConnectionType.is_intermediate: 101,
            NodeConnectionType.is_output: 824
        }

        Return:
            dict[key]: NodeConnectionType
            dict[value]: Start range
        """
        if self._range_starts is None:

            self._range_starts = dict(zip(
                [
                    NodeConnectionType.is_input,
                    NodeConnectionType.is_intermediate,
                    NodeConnectionType.is_output
                ],
                [
                    0,  # input nodes
                    self.input_neurons_number + 1,  # intermediate nodes
                    self.nodes_capacity - self.output_neurons_number + 1
                    # output nodes
                ]
            ))

        return self._range_starts

    @property
    def genome_structs_vector(self) -> definitions.libc.genome_p_p:
        return definitions.ctypes.pointer(
            (definitions.libc.genome_p * len(self))(
                *[genome.struct_ref for genome in self.genomes]
            )
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
        return bits._max_for_bit(self.gene_bits_size)

    def __len__(self) -> int:
        return len(self.genomes)

    def _get_by_index(self, index: int) -> Genome:
        return self.genomes[index]

    @property
    def genomes(self):
        return self._genomes

    def eliminate(self, eliminator: typing.Callable[[Genome], bool]):
        pass

    def reproduce(self, kill_parents: float = 0) -> "GenePool":
        pass


# Convenient aliases
Organism = Genome
Population = GenePool
