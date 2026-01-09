.. _gui_status:

*********************
GUI status and Qt6 plan
*********************

This page summarizes the current GUI stack and the modernization plan for
issues #9 and #26.

C++ Qt GUIs
===========

- GUI targets are optional and gated by ``-DWITH_QT=ON``.
- The build prefers Qt6, then Qt5, and only falls back to Qt4 if needed.
- Current CMake targets: ``latt-gui`` and ``patt-gui``.

Recommendation
--------------

Use Qt6 where possible. When Qt is not available, configure with
``-DWITH_QT=OFF`` to build the headless CLI tools only.

Python GUIs (legacy)
====================

The legacy Python GUIs under ``src/cleed-gui`` and ``src/mkiv-gui`` still use
PyQt4 and QtWebKit, both of which are deprecated. These components are not
currently built or tested in CI.

Next decisions
--------------

- Choose a primary GUI stack:
  - C++ Qt6 Widgets, or
  - Python Qt6 (PySide6 recommended for licensing).
- Decide whether to migrate the legacy Python GUIs or replace them entirely.

