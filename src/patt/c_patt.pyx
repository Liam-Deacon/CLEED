from libc.stdlib cimport malloc, free

# name of latt main function 
cdef extern from "patt.h":
    cdef int patt_main(int argc, char **argv)

cpdef int main(argv=['patt']):
    cdef int i = -1
    if len(argv) < 1:
        argv += ['patt', '--help']
    cdef char **c_argv = <char**>malloc(sizeof(char*) * len(argv))

    if c_argv is NULL:
        raise MemoryError('Unable to allocate argv')

    try:
        args = [str(x).encode() for x in argv]
        for i, arg in enumerate(args):
            c_argv[i] = arg
        i = patt_main(len(argv)-1, c_argv)
    finally:
        free(c_argv)
    
    return i