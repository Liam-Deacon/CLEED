Chapter 20: Lesen - Extract I-V Curves
======================================

The ``lesen`` program (German for "read") extracts intensity-versus-energy (I-V)
curves from experimental LEED data files, preparing them for comparison with
theoretical calculations.

.. _ch20-overview:

20.1 Overview
-------------

Experimental LEED I-V curves are obtained by measuring the intensity of
diffraction spots as a function of electron energy. The ``lesen`` utility:

1. Reads raw experimental data in various formats
2. Performs background subtraction and normalization
3. Outputs I-V curves in the format required by ``crfac``
4. Can average symmetry-equivalent beams

.. _ch20-input-formats:

20.2 Input Formats
------------------

``lesen`` supports several input formats common in LEED experiments:

**Column Format**

Simple ASCII files with energy and intensity columns:

.. code-block:: text

   # Energy(eV)  I(1,0)  I(0,1)  I(1,1)  ...
   50.0         123.4   98.7    45.2
   51.0         125.1   99.2    46.8
   ...

**SPA-LEED Format**

Data from Spot Profile Analysis LEED instruments, which include
spot position and profile information.

**Video LEED Format**

Integrated intensities extracted from video-LEED measurements.

.. _ch20-usage:

20.3 Usage
----------

.. code-block:: bash

   lesen -i <input_file> -o <output_file> [-b <beam_list>] [-s]

**Options**

``-i <input_file>``
   Input file containing raw experimental data

``-o <output_file>``
   Output file in CLEED I-V format

``-b <beam_list>``
   List of beams to extract (e.g., "1,0 0,1 1,1")

``-s``
   Apply symmetry averaging to equivalent beams

``-n``
   Normalize intensities (divide by incident beam current)

.. _ch20-output:

20.4 Output Format
------------------

The output format matches that expected by ``crfac``:

.. code-block:: text

   # Beam: (1,0)
   # Energy range: 50.0 - 300.0 eV
   50.0  0.1234
   51.0  0.1251
   52.0  0.1189
   ...
   
   # Beam: (0,1)
   ...

Each beam is preceded by a header line, and energy-intensity pairs follow
in ascending energy order.

.. _ch20-preprocessing:

20.5 Preprocessing Recommendations
----------------------------------

For best results in structural analysis:

1. **Energy Range**: Use data from approximately 50-500 eV where
   multiple-scattering effects are significant

2. **Energy Step**: Steps of 1-2 eV are typical; finer steps increase
   computation time without improving precision

3. **Background Subtraction**: Remove inelastic background using
   appropriate algorithms before extracting spot intensities

4. **Normalization**: Normalize to incident beam current to account for
   filament emission variations

5. **Symmetry Averaging**: Average symmetry-equivalent beams to reduce
   noise and experimental artifacts

.. _ch20-experimental-notes:

20.6 Experimental Considerations
--------------------------------

The quality of structural determination depends critically on experimental
I-V curve quality:

- **Beam Current Stability**: Use stabilized electron guns
- **Sample Alignment**: Ensure normal incidence (or known off-normal angle)
- **Temperature**: Record sample temperature for Debye-Waller corrections
- **Contamination**: Monitor for surface contamination during measurement

.. seealso::

   - :doc:`../part-3/ch13-program-crfac` for R-factor calculation
   - :doc:`ch19-pattern` for beam identification

