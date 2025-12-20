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

TODO (PDF p25): Insert Table 9.1 (sample output file).
