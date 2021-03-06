import ctypes
import annotatec

c_uint8 = ctypes.c_uint8
c_uint16 = ctypes.c_uint16
c_uint32 = ctypes.c_uint32
c_uint64 = ctypes.c_uint64
c_void_p = ctypes.c_void_p
c_char_p = ctypes.c_char_p
c_uint8_p = ctypes.POINTER(c_uint8)  # equal to c_char_p


libc = annotatec.Loader(
    library="genevo/c/bin/genevo.so",
    sources=[
        "genevo/c/"
    ]
)

