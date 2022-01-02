import typing
import ctypes

from . import definitions


class GenevoError(Exception):
    """Base class for all exceptions.
    """

    def __init__(self, message: str = ""):
        super().__init__(message)


class FileParsingError(GenevoError):
    """Class for all errors related to processing pool files.
    """


class PoolParsingError(FileParsingError):
    pass


class GenomeParsingError(FileParsingError):
    pass


class GeneParsingError(FileParsingError):
    pass


def get_error_level() -> ctypes.c_uint8:
    """Returns ERROR_LEVEL variable of the .so.

    Returns:
        ctypes.c_uint8; Value of the ERROR_LEVEL.
    """
    return definitions.libc.ERROR_LEVEL.value


def check_errors(raise_immediately: bool = True) -> typing.Optional[Exception]:
    """Check last error level.

    Arguments:
        raise_immediately: bool, default = True; If True, then Exception will
            be raised after some erroneous state was detected in .so. Otherwise
            Exception class will be returned as the result of this function.

    Returns:
        optional Exception; If ERROR_LEVEL is 0, then None will be returned.
    """

    error_level = get_error_level()
    # TODO: sometimes get_error_level() returns int, other times c_ubyte.
    error_level_value = (
        error_level
        if isinstance(error_level, int)
        else error_level.value)

    error = None

    if not error_level_value:
        return error  # = None at this stage

    elif 0xf0 <= error_level_value <= 0xff:
        error = OSError

    elif 0x01 <= error_level_value <= 0x0f:
        error = OSError

    elif 0x11 <= error_level_value <= 0x1f:
        error = PoolParsingError

    elif 0x21 <= error_level_value <= 0x2f:
        error = GenomeParsingError

    elif 0x31 <= error_level_value <= 0x3f:
        error = GeneParsingError

    elif error_level_value == 0xe0:
        error = StopIteration

    elif 0xe1 <= error_level_value <= 0xef:
        error = TypeError

    error_instance = error(
        definitions.libc.get_err_string(error_level).decode("utf-8"))

    if raise_immediately:
        raise error_instance
    else:
        return error_instance
