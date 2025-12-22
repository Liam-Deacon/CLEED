Manual Verification Tasks
=========================

.. _manual-figure-table:

Figures (placeholders)
----------------------

The manual references figures, but the OCR text does not preserve figure numbers or
captions. Use the PDF pages below to recover the exact figure identifiers and captions.
Replace the TODO references in the chapter files accordingly.

+----------------------+----------------------+-----------------------------------------+
| PDF page             | Figure reference     | Notes                                   |
+======================+======================+=========================================+
| 9                    | Figure TODO (LEED    | "rear view" LEED optics schematic.      |
|                      | optics)              | Chapter 1 instrumentation section.      |
+----------------------+----------------------+-----------------------------------------+
| 10                   | Figure TODO (SPA-    | SPA-LEED Faraday cup arrangement.       |
|                      | LEED optics)         | Chapter 1 instrumentation section.      |
+----------------------+----------------------+-----------------------------------------+
| 13                   | Figure TODO          | Common surface unit cells + LEED        |
|                      | (unit cells)         | patterns. Chapter 2.1.                  |
+----------------------+----------------------+-----------------------------------------+
| 14                   | Figure TODO          | Superstructure matrix vs Wood notation. |
|                      | (superstructures)    | Chapter 2.1.                            |
+----------------------+----------------------+-----------------------------------------+

.. _manual-table-tasks:

Tables (OCR transcription required)
-----------------------------------

Transcribe the tables below directly from the PDF. These are critical for the manual
usability and should be double-checked for alignment, spacing, and units.

+----------------------+----------------------+-----------------------------------------+
| PDF page(s)          | Table                | Notes                                   |
+======================+======================+=========================================+
| 18-19                | Table 8.1            | Sample parameter input file.            |
+----------------------+----------------------+-----------------------------------------+
| 21                   | Table 8.2            | Sample bulk input file.                 |
+----------------------+----------------------+-----------------------------------------+
| 22                   | Table 8.3            | Sample overlayer input file.            |
+----------------------+----------------------+-----------------------------------------+
| 22                   | Table 8.4            | Sample phase shifts input file.         |
+----------------------+----------------------+-----------------------------------------+
| 25                   | Table 9.1            | Sample output file.                     |
+----------------------+----------------------+-----------------------------------------+
| 33                   | Table 12.1           | Matrix structure.                       |
+----------------------+----------------------+-----------------------------------------+
| 34                   | Table 12.2           | Basic matrix functions.                 |
+----------------------+----------------------+-----------------------------------------+
| 40-41                | Table 14.1           | R factor control file.                  |
+----------------------+----------------------+-----------------------------------------+
| 47-49                | Table 17.1           | SEARCH parameter input file.            |
+----------------------+----------------------+-----------------------------------------+

Equations (verification snapshots)
----------------------------------

Inline LaTeX is used in the RST files. The PNGs below are cropped from the PDF as
verification references.

.. list-table:: Equation verification snapshots
   :header-rows: 1

   * - PDF page(s)
     - Equation(s)
     - Verification PNG(s)
   * - 27
     - 10.1-10.3
     - - `doc/manual/verify/equations/eq-10-1.png`
       - `doc/manual/verify/equations/eq-10-2-3.png`
   * - 28
     - 10.4-10.7
     - - `doc/manual/verify/equations/eq-10-4.png`
       - `doc/manual/verify/equations/eq-10-5.png`
       - `doc/manual/verify/equations/eq-10-6.png`
       - `doc/manual/verify/equations/eq-10-7.png`
   * - 30-31
     - 11.2 (cg, blm, gaunt)
     - - `doc/manual/verify/equations/eq-11-cg.png`
       - `doc/manual/verify/equations/eq-11-cg-transform.png`
       - `doc/manual/verify/equations/eq-11-blm.png`
       - `doc/manual/verify/equations/eq-11-blm-transform.png`
       - `doc/manual/verify/equations/eq-11-gaunt.png`
       - `doc/manual/verify/equations/eq-11-memory.png`
