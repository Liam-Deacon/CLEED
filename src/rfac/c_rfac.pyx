# distutils: language = c
# defines Cython interface to rfac

# cdef extern from "cleed_real.h":
#     cdef real
# 
# cdef extern from "rfac.h":
#     cdef struct rfac_iv_data:
#         pass

import sys
from libc.stdlib cimport malloc, free

# name of rfac main function 
cdef extern from "rfac.h":
    cdef int rfac_main(int argc, char **argv)

cpdef int main(argv=[]):
    cdef int i = -1
    if len(argv) < 1:
        argv += ['crfac', '--help']
    cdef char **c_argv = <char**>malloc(sizeof(char*) * len(argv))

    if c_argv is NULL:
        raise MemoryError('Unable to allocate argv')

    try:
        args = [str(x).encode() for x in argv]
        for i, arg in enumerate(args):
            c_argv[i] = arg
        i = rfac_main(len(argv)-1, c_argv)
    finally:
        free(c_argv)
    
    return i