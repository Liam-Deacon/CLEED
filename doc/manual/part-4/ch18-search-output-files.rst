Chapter 18: Output files (SEARCH)
=================================

.. note::
   OCR transcription from `doc/CLEED_Manual.pdf` (pages 51-52). Verify against the PDF.

18.1 Log file
-------------

A protocol of the search is stored in the file ``<project>.log``, where ``<project>`` is
the name of the parameter input file without extension. The log file starts with a list
of atom positions in the start geometry followed by a listing of the parameters and the
corresponding R factor values of each search step.

18.2 LEED files
---------------

The input parameter file for the LEED program is called ``<project>.par``. In order to
call the LEED program successfully, there must also exist a file called ``<project>.bul``
containing the bulk parameters, i.e. the geometrical and non-geometrical parameters which
are not varied within the structure search. The calculated IV curves will be stored in
``<project>.res``. ``<project>.out`` contains the control output from the LEED program.

The input parameter file and the calculated IV curves of the current best fit geometry are
copied to ``<project>.pmin`` and ``<project>.rmin``, respectively.

18.3 R factor files
-------------------

The output from the R factor program is written to the file ``<project>.dum``, which
contains only one line with the current R factor value.

18.4 Vertex file
----------------

If the simplex method is used, the vertices of the simplex are stored in the file
``<project>.ver`` after each change of the simplex. The file ``<project>.vbk`` contains a
backup of the iteration step before.
