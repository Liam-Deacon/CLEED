.. _cpp_bindings:

************************
C++ Library (cleed++)
************************

Status
======

The ``cleed++`` library is experimental. The ABI and API are not yet guaranteed
stable and may change between releases.

Build
=====

Enable the C++ library with ``BUILD_CPP_BINDINGS``:

.. code-block:: console

   cmake -S . -B build -DBUILD_CPP_BINDINGS=ON
   cmake --build build

This builds:

- ``libcleed++`` (shared)
- ``libcleed++`` (static)
- ``cleedpp_smoke`` (C++ example)

Install layout
==============

When installed, the C++ headers and libraries are placed alongside the C
artifacts:

- Headers: ``include/cleed++``
- Libraries: ``lib``

Interop expectations
====================

- Public C headers under ``src/include`` can be included from C++ (they are
  wrapped in ``extern "C"`` guards).
- ``cleed++`` currently wraps C data structures directly. Treat the C ABI as
  the stable boundary for external bindings until a dedicated facade is
  introduced.

Example
=======

``cleedpp_smoke`` demonstrates a minimal link against both the C and C++
libraries.

