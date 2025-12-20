Chapter 12: Matrix Functions
============================

.. note::
   OCR transcription from `doc/CLEED_Manual.pdf` (pages 33-36). Tables need careful
   verification.

12.1 Introduction
----------------

In contrast to the conventional definition of matrices in C as arrays of pointers to arrays
 of numbers (integer, float, etc.), in our programs matrices are structures ``struct mat_str``
 (defined in ``mat.h``, see Table 12.1). This has the advantage that all relevant information
 such as matrix dimensions (rows, cols) and the type of matrix elements (num_type) can be
 exchanged between functions together with the matrix elements themselves through a single
 variable of type ``mat``, which is a pointer to this structure (``typedef struct mat_str *mat``
 in ``mat.h``).

.. code-block:: c

   struct mat_str         /* real or complex matrix */
   {
       int mag_no;        /* magic number */

       int mat_type;      /* type of matrix (square, etc.) */
       int num_type;      /* type of matrix elements (real, complex) */

       int rows;          /* 1st dimension of matrix (number of rows) */
       int cols;          /* 2nd dimension of matrix (number of columns) */
       real *rel;         /* pointer to real parts of matrix elements */
       real *iel;         /* pointer to imag. parts of matrix elements */
   };

TODO (PDF p33): Confirm Table 12.1 formatting and structure details.

In order to make the matrix type independent of the type of the matrix elements, complex
 matrices are effectively stored as two matrices, one containing the real part of the
 elements (``*rel``) and one containing the imaginary part (``*iel``). For real matrices
 ``iel`` is a null pointer; in addition, the structure element ``num_type`` is an integer
 number representing the type of the matrix elements.

The structure element ``mat_type`` is another integer number representing the matrix type
 (rectangular, square, diagonal) which decides upon the storage scheme of the matrix
 elements and various other things. For example, the matrix inversion routine checks first
 of all, if the matrix to be inverted is square at all, and sends an error message if not.

The storage scheme for rectangular and square matrices is:

.. math::

   R_M(m, n) \mapsto *(rel + (m - 1) * rows + n)
   \tag{12.1}

12.2 Basic Matrix Functions
---------------------------

The matrix functions described in this section are mainly called by other "high level"
 matrix functions in order to check, allocate, free, etc. memory space used to store a
 matrix. They are listed in Table 12.2.

TODO (PDF p34): Insert Table 12.2 (function list).

12.2.1 matalloc, matfree
------------------------

TODO (PDF p35): Populate section 12.2.1.

12.2.2 matcheck
---------------

TODO (PDF p35): Populate section 12.2.2.

12.2.3 matcop
-------------

TODO (PDF p35): Populate section 12.2.3.

12.3 Matrix Inversion (matinv)
------------------------------

TODO (PDF p35): Populate section 12.3.

12.4 Display and Control Functions for Matrices
-----------------------------------------------

The matrix functions described in this section are not thought to be used in the
"production state" of a LEED program since they produce very large outputs. During the
 development of LEED (or other electron scattering) programs it is however useful, to
 display the contents of matrices in the right format. Two functions are available for this
 purpose. ``int matshow(mat)`` displays the real and imaginary part of complex matrix
 elements while ``int matshowabs(mat)`` displays only the modulus of complex matrix
 elements and therefore reduces the amount of output.

TODO (PDF p36): Populate the example matrix and output.
