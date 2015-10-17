#####################################################################
#                       CYSEARCH.PYX
#
#  Copyright 2015 Liam Deacon <liam.m.deacon@gmail.com>
#
#  Licensed under GNU General Public License 3.0 or later.
#  Some rights reserved. See COPYING, AUTHORS.
#
# @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
#
#####################################################################
from Shadow import gil
'''
Cython extension for performing geometry minimisation using SciPy by 
providing a bridge with the C code within `csearch`.

.. warning::
    cleed_real and its derivative types are all double precision
    due to a lack of preprocessor for Cython and in order to maintain 
    maximum compatibility with Python in-built floating point. It 
    is important that when compiling csearch that the macros 
    ``CLEED_REAL_IS_DOUBLE=1`` and ``CLEED_REAL_IS_FLOAT=0`` are 
    defined along with ``USE_CYTHON=1`` and ``USE_GSL=0`` 
    
Notes
-----
    To produce C code use::
    
        cython -I ../include cysearch.pyx
        
    Ensure that libpython27.a and python/numpy include directories are 
    added when compiling and linking to cysearch.c
'''

cimport numpy as np
cimport cython

from scipy.optimize import minimize
from libc.string cimport memcpy

cdef extern from "cleed_real.h":
    ctypedef double cleed_real

cdef extern from "cleed_vector.h":
    ctypedef double cleed_vector
    
cdef extern from "cleed_matrix.h":
    ctypedef double cleed_basic_matrix

cdef extern from "csearch.h":
    ctypedef cleed_real (*cfptr)(cleed_vector*)
    cdef double sr_evalrf(cleed_vector *x)
    cdef int sr_mkver(const cleed_vector *y, const cleed_basic_matrix *p, size_t n)
    cdef enum:
        MAX_ITER_AMOEBA
        MAX_ITER_POWELL

np.import_array() # initialize C API to call PyArray_SimpleNewFromData

cdef size_t NITER = 0

cdef update_simplex_vertices(double* x, size_t n, size_t iter):
    if not P:
        raise MemoryError("Simplex vertices matrix is NULL")
    elif iter >= n:
        raise ValueError("iter is too large")    


def simplex_callback(xi):
    '''
    Function that is called after each evaluation during the 
    simplex search
    '''
    global NITER
    cdef np.ndarray[np.double_t, ndim=1, mode="c"] x = xi
    #update_simplex_vertices(<cleed_vector *> x[0], len(xi), NITER)
    sr_mkver(<cleed_vector *> &x[0], P, len(xi))
    NITER += 1

def py_evalrf(xi):
    cdef np.ndarray[np.double_t, ndim=1, mode="c"] x = xi
    return sr_evalrf(<cleed_vector *> x.data) 

@cython.boundscheck(False)
@cython.wraparound(False)
cdef public api c_ptr_to_ndarray(double* data, int size):
    '''
    Returns an ndarray from a c pointer of type double.
    
    ::Note
        Python doesn't take ownership of `data`. You must free `data` yourself.
    '''
    if not (data and size >= 0): raise ValueError
    cdef np.npy_intp dims = size
    return np.PyArray_SimpleNewFromData(1, &dims, np.NPY_DOUBLE, <void*>data)

@cython.boundscheck(False)
@cython.wraparound(False)
cdef public api void cy_simplex(cleed_basic_matrix* p, 
                                cleed_vector *y, 
                                size_t n, 
                                double ftol, 
                                size_t *nfunc) with gil:
    '''
    Performs the Nelder-Mead simplex search for the best geometry using SciPy
    
    Parameters
    ----------
    p : cleed_basic_matrix *
        Pointer to a continguous array of the simplex vertices.
    y : cleed_vector *
        Pointer to the result vector of the RFactor function evaluations
    n : size_t
        The number of parameters in the search vector.
    ftol : double
        Tolerance in the RFactor function evaluation.
    SR_RF : void *
        Function pointer - currently hacked to void * and ignored due to 
        limitations of Cython. The imported sr_evalrf is used instead 
        for the RFactor function evaluation.
    nfunc : size_t *
        Pointer to the number of function evaluations made during 
        minimization.
        
    See Also
    --------
    simplex_callback() - function which is called after each evaluation.
    
    '''
    cdef double* x0 = <double *> p
    cdef size_t m = 0
    
    if not (p and n >= 0): 
        raise ValueError
    
    # find best geometry
    res = minimize(py_evalrf, c_ptr_to_ndarray(x0, n), 
                   method='nelder-mead',
                   callback=simplex_callback, 
                   options={'xtol': 1e-8, 
                            'disp': True,
                            'ftol': ftol,
                            'maxfev': MAX_ITER_AMOEBA})
        
    if not res.success:  # update values 
        raise Exception(res.message)
    
    cdef np.ndarray[np.double_t, ndim=1, mode="c"] y_np = res.x
    cdef np.ndarray[np.double_t, ndim=1, mode="c"] p_np = res.fun
    
    memcpy(y, &y_np[0], n * sizeof(np.double_t))
    memcpy(p, &p_np[0], sizeof(np.double_t) * n * n+1)
    m = int(res.nfev)
    nfunc = <size_t *> &m
    
    # save a backup simplex of the best result
    sr_mkver(y, p, n) 
    
cdef public api void cy_simann(cleed_basic_matrix* p, 
                               cleed_vector *y, 
                               size_t n,
                               cleed_vector *pb,
                               cleed_vector *yb, 
                               double ftol, 
                               size_t *iter,
                               double temp) with gil:
    '''
    Performs a simulated annealing search for the best geometry using SciPy
    
    Parameters
    ----------
    p : cleed_basic_matrix *
        Pointer to a continguous array of the simplex vertices.
    y : cleed_vector *
        Pointer to the result vector of the RFactor function evaluations
    n : size_t
        The number of parameters in the search vector.
    ftol : double
        Tolerance in the RFactor function evaluation.
    SR_RF : void *
        Function pointer - currently hacked to void * and ignored due to 
        limitations of Cython. The imported sr_evalrf is used instead 
        for the RFactor function evaluation.
    nfunc : size_t *
        Pointer to the number of function evaluations made during 
        minimization.
        
    See Also
    --------
    simplex_callback() - function which is called after each evaluation.
    
    '''
    if not (p and n >= 0): 
        raise ValueError
    
    cdef double* x0 = <double *> p
    cdef size_t m = 0
    
    # find best geometry
    res = minimize(py_evalrf, c_ptr_to_ndarray(x0, n), 
                   method='anneal',
                   callback=None, 
                   options={'xtol': 1e-8,
                            'schedule': 'boltzmann',
                            'T0': 3.5, 
                            'disp': True,
                            'ftol': ftol,
                            'maxfev': 2000,
                            #'maxaccept': 1.,
                            'boltzmann': 1.,
                            'learn_rate': 1.,
                            'dwell': 1
                            }
                   )
        
    if not res.success:  # update values
        raise Exception(res.message)
    
    cdef np.ndarray[np.double_t, ndim=1, mode="c"] y_np = res.x
    cdef np.ndarray[np.double_t, ndim=1, mode="c"] p_np = res.fun
    
    memcpy(y, &y_np[0], n * sizeof(np.double_t))
    memcpy(p, &p_np[0], sizeof(np.double_t) * n * n+1)
    m = int(res.nfev)
    nfunc = <size_t> &m
    
    # save a backup simplex of the best result
    sr_mkver(y, p, n) 
    
cdef public api void cy_powell(cleed_vector *p, 
                               cleed_basic_matrix *y, 
                               size_t n,
                               cleed_real ftol, 
                               size_t *iter, 
                               cleed_real *fret):
    '''
    Performs a simulated annealing search for the best geometry using SciPy
    
    Parameters
    ----------
    p : cleed_basic_matrix *
        Pointer to a continguous array of the simplex vertices.
    y : cleed_vector *
        Pointer to the result vector of the RFactor function evaluations
    n : size_t
        The number of parameters in the search vector.
    ftol : double
        Tolerance in the RFactor function evaluation.
    iter : size_t *
        Counter for number of iterations.
    fret : cleed_real *
        Pointer to the return value of the function evaluation.
        
    See Also
    --------
    simplex_callback() - function which is called after each evaluation.
    
    '''
    if not (p and n >= 0): 
        raise ValueError
    
    cdef double* x0 = <double *> p
    cdef size_t m = 0
    
    # find best geometry
    res = minimize(py_evalrf, c_ptr_to_ndarray(x0, n), 
                   method='powell',
                   callback=None, 
                   options={'xtol': 1e-8,
                            'disp': True,
                            'ftol': ftol,
                            'maxfev': MAX_ITER_POWELL,
                            }
                   )
        
    if not res.success:  # update values
        raise Exception(res.message)
    
    cdef np.ndarray[np.double_t, ndim=1, mode="c"] y_np = res.x
    cdef np.ndarray[np.double_t, ndim=1, mode="c"] p_np = res.fun
    
    memcpy(y, &y_np[0], n * sizeof(np.double_t))
    memcpy(p, &p_np[0], sizeof(np.double_t) * n * n+1)
    m = int(res.nfev)
    nfunc = <size_t> &m
    
    # save a backup simplex of the best result
    sr_mkver(y, p, n)