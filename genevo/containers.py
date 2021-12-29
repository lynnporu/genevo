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


class _LazyIterableContainer(_IterableContainer, metaclass=abc.ABCMeta):
    """Container for iterable, which items was not computed yet.
    """

    def __init__(self):
        self._iter_caching_on = True
        self._iter_cache = dict()

    @abc.abstractmethod
    def _iter_function(self, index: int):
        pass

    def _reset_iter_cache(self):
        self._iter_cache = dict()

    def _ensure_index_inited(self, index: int):
        if index not in self._iter_cache:
            self._iter_cache[index] = self._iter_function(index)

    def __getitem__(self, key: typing.Union[int, slice]):

        if not self._iter_caching_on:
            return super().__getitem__(key)

        # ensure range was inited
        if isinstance(key, slice):
            for index in range(key.start, key.stop, key.step):
                self._ensure_index_inited(index)
            return super().__getitem__(key)

        else:
            self._ensure_index_inited(key)
            return super().__getitem__(key)
