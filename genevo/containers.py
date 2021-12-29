import abc
import typing
import itertools


class _IterableContainer(metaclass=abc.ABCMeta):
    """Implements slice indexing for container. Requires single _get_by_index
    to be implemented.
    """

    @abc.abstractmethod
    def _get_by_index(self, index: int):
        pass

    @abc.abstractmethod
    def __len__(self):
        pass

    def __iter__(self):
        return (
            self._get_by_index(index)
            for index in range(len(self))
        )

    def to_list(self):
        return list(self.__iter__())

    # def __getitem__(self, key: int | slice):  # For Python3.10
    def __getitem__(self, key: typing.Union[int, slice]):

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
