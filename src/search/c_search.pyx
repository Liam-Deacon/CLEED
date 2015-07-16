from libc.stdlib cimport malloc, free

# name of search main function 
cdef extern from "search.h":
    int search_main(int argc, char **argv)

from csearch cimport *

cpdef int main(argv=['csearch']):
    cdef int i = -1
    if len(argv) < 1:
        argv += ['csearch', '--help']
    cdef char **c_argv = <char**>malloc(sizeof(char*) * len(argv))

    if c_argv is NULL:
        raise MemoryError('Unable to allocate argv')

    try:
        args = [str(x).encode() for x in argv]
        for i, arg in enumerate(args):
            c_argv[i] = arg
        i = search_main(len(argv)-1, c_argv)
    finally:
        free(c_argv)
    
    return i