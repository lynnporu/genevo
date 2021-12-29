import enum
import typing

from . import containers
from . import c_definitions


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


class _GroupingType(enum.Enum):
    group_by_bit = 1
    group_by_byte = 8


class _BitField(containers._IterableContainer):

    def __init__(
        self,
        # byte_array: iter[int],  # For Python3.10
        byte_array: typing.Iterable[int],
        bytes_size: int = None,
        skip_last_bits: int = 0,
        # For Python3.10
        # grouping: _GroupingType | int = _GroupingType.group_by_byte,
        grouping: typing.Union[_GroupingType, int] =
            _GroupingType.group_by_byte,
        skip_byte_size_checking: bool = False
    ):
        """Creates BitField.

        Arguments:
            bytes_size: int, default = None; If None, then byte_array size will
                be calculated by `len(byte_array)`. It is convenient to set
                this parameter in case `byte_array` is a pointer to dynamic
                array which size cannot be calculated manually.
            skip_last_bits: int, default = 0; Last given number of bits of the
                last byte wont be considered as the part of the current bit
                field.
                Suppose you have defined bit field with byte_array
                [0xff, 0xff]. With `skip_last_bits`=0 size of the bit field
                will be 16 bits, but with `skip_last_bits`=3 the size is equal
                to 13.
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

        self._bytes_len = bytes_size or len(byte_array)
        self._bit_size = self._bytes_len * 8 - skip_last_bits

        if self._bit_size % self._grouping_size:  # != 0
            raise ValueError(
                "invalid combination of `skil_last_bits` and `grouping`; some "
                "of the last bits won't be ever indexed")

        if not skip_byte_size_checking:
            for index, byte in enumerate(self._byte_array):
                if byte.bit_count() > 8:
                    raise ValueError(
                        f"number at position {index} is equal to {byte} has "
                        "more than 8 significant bits")

        self._byte_array = byte_array
        self._number = None
        self._skip_last_bits = skip_last_bits

    @staticmethod
    def _copy_bytes(
        byte_array: c_definitions.c_uint8_p, bytes_size: int
    # ) -> list[int]:  # For Python3.10
    ) -> typing.List[int]:
        return byte_array[:bytes_size]

    @classmethod
    def from_dynamic_array(
        cls,
        byte_array: c_definitions.c_uint8_p,
        bytes_size: int,
        copy_bytes: bool = False,
        *args, **kwargs
    ):
        """Creates a bit field from pointer to uint8_t.

        Arguments:
            copy_bytes: bool, default = False; Set this to True in case the
                memory behind `byte_array` will be freed soon.
            *args, **kwargs; The rest of the parameters which will be passed
                into standard constructor.
        """
        if copy_bytes:
            byte_array = _BitField._copy_bytes(byte_array, bytes_size)

        return cls(
            byte_array=byte_array,
            bytes_size=bytes_size,
            *args, **kwargs
        )

    def to_dynamic_array(
        self,
        realloc: bool = False
    # ) -> tuple[int, c_definitions.c_uint8_p]:  # For Python3.10
    ) -> typing.Tuple[int, c_definitions.c_uint8_p]:
        """Allocates dynamic array (if wasn't allocated yet) for the bytes
        inside this bit field.

        Arguments:
            realloc: bool, default = False; If True then new array will be
                allocated.

        Returns:
            tuple[0], int; Size of the array.
            tuple[1], c_uint8_p; The array itself.
        """

        if (
            isinstance(self._byte_array, c_definitions.c_uint8_p) and
            not realloc
        ):
            return self._bytes_len, self._byte_array

        return self._bytes_len, (c_definitions.c_uint8_p * self._bytes_len)(
            *map(c_definitions.c_uint8, self._byte_array))

    @property
    def number(self) -> int:
        if self._number is None:
            self._number = int.from_bytes(self._byte_array, byteorder="big")
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
