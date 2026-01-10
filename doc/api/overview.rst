.. _api-overview:

============
API Overview
============

This page provides a high-level overview of the CLEED API architecture
and key concepts for developers.

.. contents:: On this page
   :local:
   :depth: 2

Architecture
============

The CLEED package follows a modular design where each component can be
used independently or as part of the full LEED-IV optimization workflow.

.. mermaid::

   flowchart TB
       subgraph Input["Input Files"]
           BUL[".bul files<br/>(bulk structure)"]
           INP[".inp files<br/>(parameters)"]
           CTR[".ctr files<br/>(control)"]
           PHS["Phase shifts"]
       end

       subgraph Core["Core Programs"]
           CSEARCH["csearch<br/>(optimizer)"]
           CLEED["cleed_*<br/>(LEED calc)"]
           CRFAC["crfac<br/>(R-factor)"]
       end

       subgraph Output["Output"]
           IV["I(V) curves"]
           RFAC["R-factor value"]
           OPT["Optimized structure"]
       end

       BUL --> CSEARCH
       INP --> CSEARCH
       CTR --> CSEARCH
       CSEARCH --> |"parameters"| CLEED
       PHS --> CLEED
       CLEED --> IV
       IV --> CRFAC
       CRFAC --> RFAC
       RFAC --> |"fitness"| CSEARCH
       CSEARCH --> OPT


Optimizer Registry
==================

The search module uses a registry pattern to manage different optimization
algorithms. This allows easy switching between algorithms at runtime.

Available Optimizers
--------------------

.. list-table:: Optimization Algorithms
   :header-rows: 1
   :widths: 20 15 65

   * - Algorithm
     - Type
     - Description
   * - Simplex (Nelder-Mead)
     - Local
     - Derivative-free simplex method, good for local refinement
   * - Powell
     - Local
     - Conjugate direction method, efficient for smooth functions
   * - Simulated Annealing
     - Global
     - Stochastic method with temperature-based acceptance
   * - Differential Evolution
     - Global
     - Population-based evolutionary algorithm
   * - Particle Swarm
     - Global
     - Swarm intelligence optimization


Configuration Structures
========================

Each optimizer has its own configuration structure that inherits from
a common base configuration.

Common Parameters
-----------------

.. list-table:: Global Optimizer Parameters
   :header-rows: 1
   :widths: 25 15 60

   * - Parameter
     - Default
     - Description
   * - ``max_iter``
     - 1000
     - Maximum number of iterations
   * - ``max_eval``
     - 10000
     - Maximum function evaluations
   * - ``tolerance``
     - 1e-6
     - Convergence tolerance
   * - ``seed``
     - 0
     - Random seed (0 = time-based)


Evaluation Pipeline
===================

The objective function evaluation follows this pipeline:

1. **Parameter Validation**: Check bounds and constraints
2. **Geometry Update**: Apply parameter changes to atomic positions
3. **LEED Calculation**: Run ``cleed_*`` to compute theoretical I(V)
4. **R-factor Computation**: Compare with experimental data via ``crfac``
5. **Fitness Return**: Return R-factor value to optimizer

.. mermaid::

   sequenceDiagram
       participant O as Optimizer
       participant E as Evaluator
       participant L as LEED Calculator
       participant R as R-factor Calculator

       O->>E: trial parameters
       E->>E: validate bounds
       E->>E: update geometry
       E->>L: run cleed_*
       L-->>E: I(V) curves
       E->>R: run crfac
       R-->>E: R-factor
       E-->>O: fitness value


Error Handling
==============

The API uses integer return codes for error handling:

.. list-table:: Common Return Codes
   :header-rows: 1
   :widths: 20 80

   * - Code
     - Meaning
   * - 0
     - Success
   * - 1
     - General error
   * - -1
     - Invalid parameter
   * - -2
     - Memory allocation failure
   * - -3
     - File I/O error


See Also
========

* :doc:`/algorithms/index` - Detailed algorithm documentation
* :doc:`/LEED_programs` - Program usage guides
* :doc:`/file_formats` - Input/output file formats
