Chapter 9: Output files
=======================

.. note::
   OCR transcription from `doc/CLEED_Manual.pdf` (pages 24-25). Table 9.1 requires
   careful review against the PDF. See :ref:`manual-table-tasks`.

9.1 Results - IV curves
-----------------------

Table 9.1 shows a sample output file for a :math:`p(\sqrt{3} \times \sqrt{3})` H2O / Ru(0001)
 overlayer structure. The header of the file consists of lines beginning with ``#``. They
 contain information needed e.g. by the R factor program:

``vn``
  Program version.

``ts``
  Creation date and time.

``en``
  Number of energy points, first energy, last energy, energy step.

``bn``
  Total number of beams in output files.

``bi``
  Beam counter, 1st beam index, 2nd beam index, beam set (for each beam).

The header is followed by lines containing the energy (in eV) and the relative beam
 intensities in the same order as the beam list separated by blanks.

.. code-block:: text

   ########################################
   #           output from CLEED          #
   ########################################
   #vn 0.20 (version GH/30.08.97)
   #ts Thu Jul 2 07:18:21 1998
   #
   #en 58 32.000000 260.100000 4.000000
   #bn 109
   #bi 0 0.000000 0.000000 0
   #bi 1 -1.000000 0.000000 0
   #bi 2 -1.000000 1.000000 0
   #bi 3 0.000000 -1.000000 0
   #bi 4 0.000000 1.000000 0
   #bi 5 1.000000 -1.000000 0
   #bi 6 1.000000 0.000000 0
   #bi 7 -2.000000 1.000000 0
   #bi 8 -1.000000 -1.000000 0
   #bi 9 -1.000000 2.000000 0
   #bi 10 1.000000 -2.000000 0
   ...
   #bi 104 -3.333333 2.666667 2
   #bi 105 0.666667 -3.333333 2
   #bi 106 0.666667 2.666667 2
   #bi 107 2.666667 -3.333333 2
   #bi 108 2.666667 0.666667 2
   32.00 1.758462e-02 1.557583e-03 6.515479e-04 6.515850e-04 1.557595e-03
   1.557651e-03 6.515733e-04 0.000000e+00 0.000000e+00 0.000000e+00 0.000000e+00
   ... 0.000000e+00 0.000000e+00 0.000000e+00 0.000000e+00 0.000000e+00
   ...
   260.00 1.303590e-02 7.845590e-05 5.554994e-03 5.555435e-03 7.845676e-05
   7.851732e-05 5.555437e-03 8.115542e-04 7.950326e-04 7.949291e-04 8.117350e-04
   ... 1.905631e-05 1.905916e-05 1.286778e-05 1.286476e-05 1.906064e-05

Table 9.1: Sample output file for p(sqrt(3) x sqrt(3)) (H2)O / Ru(0001).
