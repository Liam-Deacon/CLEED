Chapter 19: Pattern - LEED Pattern Simulation
==============================================

The ``pattern`` program simulates LEED diffraction patterns for given surface
structures, allowing visualization of expected spot positions and symmetry
before performing full I-V calculations.

.. _ch19-overview:

19.1 Overview
-------------

LEED pattern simulation is an essential first step in surface structure analysis.
Before investing computational resources in I-V calculations, it is valuable to:

1. Verify that the proposed surface structure produces the observed
   diffraction pattern symmetry
2. Identify beam indices for experimental I-V curves
3. Understand the relationship between real-space and reciprocal-space
   lattice vectors

The ``pattern`` program calculates spot positions in reciprocal space based on
the surface unit cell and displays them graphically or outputs coordinates
for further processing.

.. _ch19-theory:

19.2 Theoretical Background
---------------------------

LEED spots appear at positions where the Laue condition is satisfied:

.. math::

   \mathbf{k}_{\parallel,\text{out}} - \mathbf{k}_{\parallel,\text{in}} = \mathbf{G}_{hk}

where :math:`\mathbf{G}_{hk} = h\mathbf{a}^* + k\mathbf{b}^*` is a surface
reciprocal lattice vector.

For a reconstructed or overlayer structure with superstructure matrix :math:`M`:

.. math::

   \begin{pmatrix} \mathbf{a}_s \\ \mathbf{b}_s \end{pmatrix} = M 
   \begin{pmatrix} \mathbf{a}_b \\ \mathbf{b}_b \end{pmatrix}

the pattern shows both substrate spots and fractional-order spots from the
superstructure.

.. _ch19-usage:

19.3 Usage
----------

.. code-block:: bash

   pattern -i <input_file> [-o <output_file>] [-e <energy>]

**Options**

``-i <input_file>``
   Surface structure input file (same format as CLEED input)

``-o <output_file>``
   Output file for spot positions (optional)

``-e <energy>``
   Electron energy in eV (affects Ewald sphere radius)

.. _ch19-output:

19.4 Output Format
------------------

The output lists spot positions in reciprocal-lattice units (h, k) along
with their Cartesian coordinates on the LEED screen:

.. code-block:: text

   # LEED Pattern: Surface = c(2x2)
   # Energy = 100.0 eV
   # h    k      x(Å⁻¹)    y(Å⁻¹)
     0    0      0.000     0.000
     1    0      1.571     0.000
     0    1      0.000     1.571
    -1    0     -1.571     0.000
     0.5  0.5    0.785     0.785
   ...

.. _ch19-alternatives:

19.5 Modern Alternatives
------------------------

For LEED pattern visualization, the standalone program **LEEDpat** provides
enhanced functionality:

- Interactive graphical display
- Multiple domains and rotational symmetry
- Export to various image formats
- Available at: https://www.fhi.mpg.de/958975/LEEDpat4

.. seealso::

   - :doc:`../part-1/ch02-leed-pattern` for LEED pattern theory
   - :doc:`ch20-lesen` for extracting I-V curves from experimental data

