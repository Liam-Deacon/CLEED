.. _background:

**********
Background
**********

CLEED (Computational Low Energy Electron Diffraction) is a package for LEED-IV
analysis: comparing experimental I(V) curves to simulated curves and optimising
structural parameters to minimise a goodness-of-fit metric (R-factor).

What is LEED?
=============

Low Energy Electron Diffraction (LEED) is a surface-sensitive technique that uses
the wave nature of electrons to probe the atomic structure of crystalline surfaces.
Electrons with kinetic energies between 20-500 eV have de Broglie wavelengths of
0.5-3 Å, comparable to interatomic distances, making them ideal probes for
surface crystallography.

Key characteristics of LEED:

- **Surface sensitivity**: The inelastic mean free path of low-energy electrons
  is only 5-10 Å, so LEED probes only the topmost atomic layers
- **Quantitative structural information**: Analysis of diffraction spot intensities
  vs. energy (I-V curves) reveals atomic positions with ~0.02-0.05 Å precision
- **Wide applicability**: Works on any crystalline surface—metals, semiconductors,
  oxides, molecular adsorbates

Historical Context
==================

LEED was discovered by Davisson and Germer in 1927, providing the first experimental
confirmation of de Broglie's electron wave hypothesis. However, quantitative structural
analysis only became practical in the 1970s when:

1. Ultra-high vacuum technology enabled clean surface preparation
2. Computer power became sufficient for dynamical scattering calculations
3. Theoretical frameworks (Pendry, Van Hove, Tong) were developed

CLEED continues this tradition, implementing state-of-the-art algorithms for
surface structure determination.

CLEED Package Overview
======================

The CLEED package consists of three main programs:

1. **cleed_sym / cleed_nsym**: Calculate theoretical I-V curves using full dynamical
   scattering theory. The symmetrized version (``cleed_sym``) exploits surface symmetry
   for faster calculations.

2. **crfac**: Compare experimental and theoretical I-V curves using various R-factors
   (Pendry, Zanazzi-Jona, etc.) with automatic energy-scale optimization.

3. **csearch**: Automated structure optimization using simplex, Powell's method,
   simulated annealing, or genetic algorithms.

For a detailed, canonical reference of the original workflows, inputs/outputs,
and algorithms, see the PDF manual and the restructured documentation:

* :download:`CLEED Reference Manual (PDF) <CLEED_Manual.pdf>`
* :doc:`manual/index` — Full manual in searchable web format

Modern Context and Resources
============================

While CLEED represents a classic implementation of LEED-IV analysis, several
developments have occurred in the field since its original development:

Related Software Packages
-------------------------

- **LEEDpat** (`FHI-Berlin <https://www.fhi-berlin.mpg.de/KHsoftware/LEEDpat/>`_):
  Simulation of LEED patterns for surface structure visualization

- **SATLEED**: Van Hove's implementation, distributed with the NIST Surface
  Structure Database

- **TensErLEED**: Tensor LEED implementation by Blum & Heinz for efficient
  refinement near reference structures
  (`Comp. Phys. Comm. 134 (2001) 392 <https://doi.org/10.1016/S0010-4655(00)00209-5>`_)

- **ViPErLEED**: Modern Python-based LEED analysis package with GPU acceleration
  (`GitHub <https://github.com/viperleed/viperleed>`_,
  `Phys. Rev. Research 6, 013096 (2024) <https://doi.org/10.1103/PhysRevResearch.6.013096>`_)

Databases and Resources
-----------------------

- **NIST Surface Structure Database (SSD)**:
  Comprehensive database of solved surface structures
  (`NIST SRD 42 <https://www.nist.gov/srd/nist-standard-reference-database-42>`_)

- **Surface Structure Database (SSDB)** at FHI-Berlin:
  `https://www.fhi-berlin.mpg.de/KHsoftware/ssdb/ <https://www.fhi-berlin.mpg.de/KHsoftware/ssdb/>`_

Phase Shift Calculations
------------------------

CLEED requires pre-calculated atomic phase shifts. These can be generated using:

- **Barbieri/Van Hove phase shift package**:
  `http://www.icts.hkbu.edu.hk/VanHove_files/leed/leedpack.html <http://www.icts.hkbu.edu.hk/VanHove_files/leed/leedpack.html>`_

- Phase shifts from DFT calculations (VASP, WIEN2k, etc.)

Literature References
---------------------

Essential reading for LEED theory and practice:

1. J. B. Pendry, "Low Energy Electron Diffraction", Academic Press, 1974
   — The foundational theoretical treatment

2. M. A. Van Hove, W. H. Weinberg, C.-M. Chan, "Low-Energy Electron Diffraction",
   Springer, 1986 (`doi:10.1007/978-3-642-82721-1 <https://doi.org/10.1007/978-3-642-82721-1>`_)
   — Comprehensive textbook covering theory and practice

3. K. Heinz, "LEED and DLEED as modern tools for quantitative surface structure
   determination", Rep. Prog. Phys. 58 (1995) 637
   (`doi:10.1088/0034-4885/58/6/003 <https://doi.org/10.1088/0034-4885/58/6/003>`_)

4. M. A. Van Hove, "From Surface Science to Nanoscience", Surf. Interface Anal.
   28 (1999) 36 — Historical perspective

Review articles on modern developments:

5. G. Held, "Low-Energy Electron Diffraction Crystallography of Surfaces and
   Interfaces", Bunsen-Magazin 12 (2010) 124

6. F. Kraushofer et al., "Self-consistent determination of LEED I-V curves using
   machine learning", Phys. Rev. B (2023)

Citing CLEED
============

If you use CLEED in your research, please cite:

- The original CLEED paper (if available)
- G. Held et al., relevant publications for the specific algorithms used
- This documentation: CLEED Package, https://github.com/Liam-Deacon/CLEED

See :doc:`authors` for the full list of contributors.

