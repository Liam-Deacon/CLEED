.. _api-index:

=============
API Reference
=============

This section provides comprehensive documentation for the CLEED C API,
generated from source code comments using Doxygen and Breathe.

.. contents:: On this page
   :local:
   :depth: 2

Overview
========

The CLEED codebase is organized into several logical modules:

**Core LEED Calculations**
    Functions and data structures for computing LEED I(V) curves,
    including scattering theory, phase shifts, and intensity calculations.

**R-factor Computation**
    Implementation of various R-factor metrics for comparing
    experimental and theoretical I(V) curves.

**Search/Optimization Algorithms**
    Global and local optimization algorithms for structural refinement,
    including Differential Evolution, Particle Swarm Optimization,
    Simplex, Powell, and Simulated Annealing methods.

**Matrix Operations**
    Linear algebra utilities used throughout the codebase.

**Pattern Visualization**
    Tools for generating and visualizing LEED patterns.

**Lattice Utilities**
    Functions for working with surface lattices and Miller indices.

.. toctree::
   :maxdepth: 2
   :caption: API Documentation

   overview


Search Algorithm API
====================

The search module provides optimization algorithms for LEED-IV structural
refinement. Below are the key data structures and functions.

Optimizer Configuration
-----------------------

.. doxygengroup:: search_core
   :project: CLEED
   :content-only:

Optimization Algorithms
-----------------------

.. doxygengroup:: search_algos
   :project: CLEED
   :content-only:

Evaluation Functions
--------------------

.. doxygengroup:: search_eval
   :project: CLEED
   :content-only:

I/O Functions
-------------

.. doxygengroup:: search_io
   :project: CLEED
   :content-only:

Random Number Generation
------------------------

.. doxygengroup:: search_rng
   :project: CLEED
   :content-only:


Pattern API
===========

Functions for generating and manipulating LEED patterns.

.. doxygenfile:: pattern.h
   :project: CLEED
   :sections: func typedef

.. doxygenfile:: spots.h
   :project: CLEED
   :sections: func typedef enum


Lattice API
===========

Functions for working with surface lattices.

.. doxygenfile:: lattice.h
   :project: CLEED
   :sections: func typedef


Quick Links
===========

* :ref:`genindex` - General index
* :ref:`search` - Search the documentation
