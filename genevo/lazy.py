"""This module contains class for creating lazy stubs.
"""


class LazyStub:
    """Creates stub for some value which has not been initialized yet.
    """

    def __init__(self, initfunc, *args, **kwargs):
        """Creates a stub which can be initialized with the `initfunc`. All the
        *args and **kwargs will be passed into initfunc.
        """
        self._func = initfunc
        self._args = args
        self._kwargs = kwargs
        self._result = None
        self._initialized = False

    def init(self, *args, **kwargs):
        """Initialize the stub. You can freely replace it by the result of this
        function. Anyway, function won't be executed twice.
        You can add additional *args and **kwargs here.
        """
        self._result = self._func(*args, *self._args, **kwargs, **self._kwargs)
        self._initialized = True
        return self._result
