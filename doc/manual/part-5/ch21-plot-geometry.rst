Chapter 21: Plot Geometry - Structure Visualization
====================================================

The ``plot_geometry`` utility visualizes surface atomic structures,
providing graphical representations of the models used in LEED calculations.

.. _ch21-overview:

21.1 Overview
-------------

Visualization of surface structures is essential for:

1. Verifying that input files correctly describe the intended structure
2. Communicating structural results in publications
3. Comparing different structural models
4. Understanding the geometry of relaxations and reconstructions

The ``plot_geometry`` program reads CLEED input files and generates
graphical output showing atomic positions.

.. _ch21-usage:

21.2 Usage
----------

.. code-block:: bash

   plot_geometry -i <input_file> [-o <output_file>] [-f <format>]

**Options**

``-i <input_file>``
   CLEED structure input file

``-o <output_file>``
   Output graphics file (default: display on screen)

``-f <format>``
   Output format: ``ps`` (PostScript), ``png``, ``svg``, or ``screen``

``-v <view>``
   View direction: ``top`` (default), ``side``, or ``3d``

``-l <layers>``
   Number of layers to display (default: all)

.. _ch21-views:

21.3 View Options
-----------------

**Top View**

Shows the surface looking down the surface normal, useful for
understanding the 2D periodicity and in-plane atomic arrangements.

**Side View**

Shows a cross-section perpendicular to the surface, useful for
visualizing layer spacings and vertical relaxations.

**3D View**

Perspective view showing the full 3D structure, useful for
presentations and understanding complex geometries.

.. _ch21-output-formats:

21.4 Output Formats
-------------------

``plot_geometry`` supports several output formats:

- **PostScript (.ps)**: Publication-quality vector graphics
- **PNG (.png)**: Raster graphics for web and presentations
- **SVG (.svg)**: Scalable vector graphics for web
- **Screen**: Interactive display (requires X11/Qt)

.. _ch21-customization:

21.5 Customization
------------------

The appearance can be customized through a configuration file or
command-line options:

- Atom colors by element
- Atom radii (covalent, van der Waals, or custom)
- Bond display threshold
- Unit cell outline
- Layer shading

Example configuration:

.. code-block:: text

   # plot_geometry.conf
   atom_color_Cu = orange
   atom_color_O = red
   atom_radius_scale = 0.8
   show_unit_cell = true
   show_bonds = true
   bond_cutoff = 3.0

.. _ch21-alternatives:

21.6 Modern Alternatives
------------------------

For advanced visualization, consider these tools:

**VESTA**
   Free 3D visualization program for structural models
   https://jp-minerals.org/vesta/

**ASE (Atomic Simulation Environment)**
   Python library with visualization capabilities
   https://wiki.fysik.dtu.dk/ase/

**Jmol/JSmol**
   Java-based molecular viewer, browser-compatible
   http://jmol.sourceforge.net/

**OVITO**
   Visualization and analysis for atomistic simulation data
   https://www.ovito.org/

.. seealso::

   - :doc:`../part-2/ch08-input-files` for structure file format
   - :doc:`ch19-pattern` for LEED pattern visualization

