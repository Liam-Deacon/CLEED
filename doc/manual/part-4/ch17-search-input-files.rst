Chapter 17: Input files (SEARCH)
================================

.. note::
   OCR transcription from `doc/CLEED_Manual.pdf` (pages 47-49). Table 17.1 requires
   careful review.

17.1 Parameter Input
--------------------

.. code-block:: text

   # input file for SEARCH
   # Si(111)-Er (1x1)
   # atomic positions according to Spence et al. Phys. Rev. B 61 (2000) 5707.
   #
   a1: 1.9162240 -3.3189974 0.0
   a2: 1.9162240 3.3189974 0.0
   #
   m1: 1. 0.
   m2: 0. 1.
   #
   # bulk: (see *.bul)
   # pb: Si 0.0 2.2126649 -0.7822951 dr3 0.05 0.05 0.05
   # pb: Si 0.0 0.0 0.0 dr3 0.05 0.05 0.05
   #
   # overlayer:
   po: Si 0.0 0.0 +2.353 dr3 0.05 0.05 0.05
   po: Si 0.0 -2.2126649 +3.153 dr3 0.05 0.05 0.05
   po: Ru 0.0 0.0 +5.453 dr3 0.05 0.05 0.05
   po: Si 0.0 +2.2126649 +7.233 dr3 0.05 0.05 0.05
   po: Si 0.0 -2.2126649 +8.053 dr3 0.05 0.05 0.05
   #
   # minimum radii:
   rm: Si 1.00
   rm: Ru 1.00
   # z range
   zr: 1.00 9.00
   # sz: 0 (xyz search), 1 (z only)
   sz: 1
   # sr: rotational axis
   sr: 3 0.0 0.0

Table 17.1: Sample parameter input file for p(1 x 1) Er / Si(111).

17.2 Bulk parameters
--------------------

Bulk parameters are described in the sample file. The minimum radii are defined per atom type:

``rm: <element> <radius>``
  Minimum radii for each atom type. If the distance between two atoms is smaller than the
  sum of their minimum radii, an additional number is added to the actual R factor in
  order to repel the search from this un-physical geometry.

17.3 R factor control file
--------------------------

Additional parameters:

``zr: f f``
  Z-range. The atoms are forced to stay within this range of z coordinates. If an atom
  exceeds this range, an additional number is added to the actual R factor in order to
  repel the search from this un-physical geometry.

``sz: n``
  Variation of vertical parameters (1) or vertical and lateral parameters (0) in agreement
  with the specified symmetry.

``sr: n f f``
  Rotational symmetry: degree (n-fold axis), position of axis (x, y in \AA).


17.2 Bulk parameters
--------------------

<project>.bul. See Chapter 8.

17.3 R factor control file
--------------------------

<project>.ctr. See Chapter 14.
