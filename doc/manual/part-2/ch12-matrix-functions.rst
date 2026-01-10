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

Table 12.1: Matrix structure (see code listing above).

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

.. list-table:: Table 12.2: Basic matrix functions
   :header-rows: 1

   * - function name
     - description
     - source file
   * - ``mat matalloc(mat, int, int, int)``
     - allocate memory for matrix
     - ``matalloc.c``
   * - ``int matcheck(mat)``
     - check validity of pointer
     - ``matcheck.c``
   * - ``mat matcop(mat, mat)``
     - copy matrices
     - ``matcop.c``
   * - ``int matfree(mat)``
     - free memory
     - ``matfree.c``

12.2.1 matalloc, matfree
------------------------

See Table 12.2 for function signatures.

12.2.2 matcheck
---------------

See Table 12.2 for function signatures.

12.2.3 matcop
-------------

See Table 12.2 for function signatures.

12.3 Matrix Inversion (matinv)
------------------------------

The storage scheme for rectangular and square matrices is:

.. math::

   R_M(m, n) \mapsto *(rel + (m - 1) * rows + n)
   	ag{12.1}

.. math::

   I_M(m, n) \mapsto *(iel + (m - 1) * rows + n)
   	ag{12.2}

diagonal matrices are stored as:

.. math::

   R_M(m, m) \mapsto *(rel + m)
   	ag{12.3}

.. math::

   I_M(m, m) \mapsto *(iel + m)
   	ag{12.4}

Note that the first element in the arrays ``rel`` and ``iel`` is never used, therefore these
arrays have the length ``[(n * m + 1) * sizeof(real)]`` for a rectangular or square
``n x m`` matrix and ``[(n + 1) * sizeof(real)]`` for a diagonal matrix.

The general format for matrix functions which have a matrix as result is:

.. code-block:: text

   (mat) result = matfunction((mat) storage, operand_1, operand_2, ...)

The first parameter of the function call (storage) is usually a matrix where the result
will be written to (type ``mat``, i.e. a pointer to a matrix structure). If the dimensions
or matrix type of this matrix do not match with those of the result, the necessary
adjustments in memory will be done automatically. In particular, if storage is the null
pointer (``NULL``), a new matrix will be created. In any case, the return value ``result``
is a pointer to this updated matrix which is not necessarily identical with storage. The
parameters (operands) depend on the purpose of the particular matrix function. They can
be either matrices or single numbers.

12.4 Display and Control Functions for Matrices
-----------------------------------------------

The matrix functions described in this section are not thought to be used in the
"production state" of a LEED program since they produce very large outputs. During the
 development of LEED (or other electron scattering) programs it is however useful, to
 display the contents of matrices in the right format. Two functions are available for this
 purpose. ``int matshow(mat)`` displays the real and imaginary part of complex matrix
 elements while ``int matshowabs(mat)`` displays only the modulus of complex matrix
 elements and therefore reduces the amount of output.

For example the matrix:

.. math::

   egin{pmatrix}
   1 & 1 + i \
   1 - i & i
   \end{pmatrix}

would be displayed by ``matshow`` as:

.. code-block:: text

   (2 rows) x (2 columns):
   (1.00, 0.00) (1.00, 1.00)
   (1.00, -1.00) (0.00, 1.00)

while ``matshowabs`` would produce the following output:

.. code-block:: text

   *** abs *** (2 rows) x (2 columns):
   1.00 1.41
   1.41 1.00

Note that both functions also display the matrix dimensions which is particularly useful
when the rows are too long to be displayed in one line of the screen.
