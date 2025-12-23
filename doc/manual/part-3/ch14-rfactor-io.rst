Chapter 14: Input / output files (R factor program)
===================================================

.. note::
   OCR transcription from `doc/CLEED_Manual.pdf` (pages 40-42). Table 14.1 requires
   careful review.

14.1 Control file
-----------------

.. code-block:: text

   # Si(111)-Er (1x1)
   # (control file for R factor program CRFAC)
   #
   # Theor. indices are listed for 3-fold rot. symmetry output
   # and a calculation up to 250 eV. Delete if no experimental
   # data are available for certain spots.
   #
   # ef=<experimental input file>
   # ti=<corresponding indices in theoretical input file>
   # id=<group ID> (does not really matter)
   # wt=<weight>
   # : = separator
   #
   ef=Si111_Er_expt_01:ti=(0.00,1.00):id=01:wt=1.
   ef=Si111_Er_expt_02:ti=(1.00,0.00):id=02:wt=1.
   ef=Si111_Er_expt_03:ti=(1.00,1.00)+(2.00,-1.00):id=03:wt=1.
   ef=Si111_Er_expt_04:ti=(0.00,2.00):id=04:wt=1.
   ef=Si111_Er_expt_05:ti=(2.00,0.00):id=05:wt=1.
   ef=Si111_Er_expt_06:ti=(-1.00,3.00)+(1.00,2.00):id=06:wt=1.
   ef=Si111_Er_expt_07:ti=(3.00,-1.00)+(2.00,1.00):id=07:wt=1.
   ef=Si111_Er_expt_08:ti=(0.00,3.00):id=08:wt=1.
   ef=Si111_Er_expt_09:ti=(3.00,0.00):id=09:wt=1.
   ef=Si111_Er_expt_10:ti=(2.00,2.00)+(4.00,-2.00):id=10:wt=1.
   ef=Si111_Er_expt_11:ti=(-1.00,4.00)+(1.00,3.00):id=11:wt=1.
   ef=Si111_Er_expt_12:ti=(4.00,-1.00)+(3.00,1.00):id=12:wt=1.
   ef=Si111_Er_expt_13:ti=(0.00,4.00):id=13:wt=1.
   ef=Si111_Er_expt_14:ti=(4.00,0.00):id=14:wt=1.

Table 14.1: Sample parameter input file for p(1 x 1) Er / Si(111).

14.2 Output
-----------

Table 14.1 shows an example R factor control file for a p(1 x 1) Er / Si(111) structure.
Each line defines the correlation of one pair of theoretical and experimental IV curves
using the following syntax:

.. code-block:: text

   ef=<expt. file>:ti=<index list>:id=<ID number>:wt=<weight>

``<expt. file>`` contains one experimental IV curve in the format:

.. code-block:: text

   energy 1 intensity 1
   energy 2 intensity 2

Lines beginning with a ``#`` and blank lines are ignored.

``<index list>`` is a list of indices of beams to be averaged and compared with the
experimental IV curve. The two indices of each beam are in round brackets and can be
preceded by a weighting factor followed by ``*``. Different beams are connected through
``+``. The general form is:

.. code-block:: text

   {<weight>}*(<index_1>,<index_2>) + {<weight>}*(<index_1>,<index_2>) + ...

``<ID number>`` is an integer number attached to this pair of IV curves. When the option
``-a all`` is specified, the partial R factors will be calculated for all groups of IV curves
sharing the same ID numbers.

``<weight>`` is a positive real number defining the weight of this pair of IV curves when
calculating the average R factor. This weight (default: 1) will be multiplied with the
fraction of the total energy range covered by this pair of IV curves.
