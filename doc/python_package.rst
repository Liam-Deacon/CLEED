.. _python_package:

************************
Python package (cleed)
************************

Status
======

The Python package is an initial wrapper around the CLEED executables. It does
not yet bundle native binaries; instead, it provides a CLI that invokes
executables available on the system ``PATH``. Wheel build and bundling are
planned as follow-ups.

Install
=======

From source (editable):

.. code-block:: console

   python3 -m pip install -e .

CLI usage
=========

List available tools:

.. code-block:: console

   cleed --list-tools

Invoke a tool, passing args through after ``--``:

.. code-block:: console

   cleed cleed_nsym -- -c Ni111_2x2O.ctr -i Ni111_2x2O.inp -b Ni111_2x2O.bul

Build plan
==========

Future work (see `issue #37 <https://github.com/Liam-Deacon/CLEED/issues/37>`_)
will replace the PATH-based wrapper with packaged binaries and wheels built
via CI.
