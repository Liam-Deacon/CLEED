from libc.stdlib cimport malloc, free

# name of ftsmooth main function 
cdef extern from "ftsmooth.h":
    cdef int ftsmooth_main(int argc, char **argv)

from cftsmooth cimport __version__, __description__

cpdef int main(argv=['ftsmooth']):
    cdef int i = -1
    if len(argv) < 1:
        argv += ['ftsmooth', '--help']
    cdef char **c_argv = <char**>malloc(sizeof(char*) * len(argv))

    if c_argv is NULL:
        raise MemoryError('Unable to allocate argv')

    try:
        args = [str(x).encode() for x in argv]
        for i, arg in enumerate(args):
            c_argv[i] = arg
        i = ftsmooth_main(len(argv)-1, c_argv)
    finally:
        free(c_argv)
    
    return i