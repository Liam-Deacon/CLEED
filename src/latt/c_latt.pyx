from libc.stdlib cimport malloc, free

# name of latt main function 
cdef extern from "latt.h":
    cdef int latt_main(int argc, char **argv)

from clatt cimport __version__, __description__

cpdef int main(argv=['latt']):
    cdef int i = -1
    if len(argv) < 1:
        argv += ['latt', '--help']
    cdef char **c_argv = <char**>malloc(sizeof(char*) * len(argv))

    if c_argv is NULL:
        raise MemoryError('Unable to allocate argv')

    try:
        args = [str(x).encode() for x in argv]
        for i, arg in enumerate(args):
            c_argv[i] = arg
        i = latt_main(len(argv)-1, c_argv)
    finally:
        free(c_argv)
    
    return i
  
def latt_version():
    return __version__