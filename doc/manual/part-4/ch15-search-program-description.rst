Chapter 15: Program description (SEARCH)
========================================

.. note::
   OCR transcription from `doc/CLEED_Manual.pdf` (page 44). Verify against the PDF.

The search program performs the structure optimisation. It uses standard optimisation
algorithms such as the downhill simplex method [48, 49], Powell's method [48, 50],
simulated annealing [48] and the genetic algorithm [51, 52] which can be selected by the
user in order to perform the search for the best fit geometry. While the first two
algorithms are strictly downhill oriented, i.e. will find only the nearest local R factor
minimum, the latter two algorithms should in principle provide a means of locating the
global R factor minimum within the given constraints, at the expense of many more search
steps. Within each search step a set of geometrical parameters is chosen by the algorithm
depending on the R factor values achieved before and in accordance with user-specified
symmetry constraints. The parameters are written to a file serving as input for the LEED
program whose output is then fed into the R factor program in order to calculate an R
factor value for this parameter set.
