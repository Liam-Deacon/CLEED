from libc.stdlib cimport malloc, free

# name of latt main function 
cdef extern from "leed.h":
    cdef int leed_main(int argc, char **argv)

cpdef int main(argv=['cleed']):
    cdef int i = -1
    if len(argv) < 1:
        argv += ['cleed', '--help']
    cdef char **c_argv = <char**>malloc(sizeof(char*) * len(argv))

    if c_argv is NULL:
        raise MemoryError('Unable to allocate argv')

    try:
        args = [str(x).encode() for x in argv]
        for i, arg in enumerate(args):
            c_argv[i] = arg
        i = leed_main(len(argv)-1, c_argv)
    finally:
        free(c_argv)
    
    return i