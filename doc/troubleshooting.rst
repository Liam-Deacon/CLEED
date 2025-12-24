.. _troubleshooting:

****************
Troubleshooting
****************

Common issues and quick checks when running CLEED.

Missing phase shift files
=========================

Symptoms: errors about missing ``*.phs`` or no phase shift directory.

- Ensure ``CLEED_PHASE`` points to a directory containing element ``.phs`` files.
- Regenerate phase shifts with ``phsh.py`` and verify output names.

Input file prefix mismatch
==========================

Symptoms: tools cannot find matching inputs or generate output files with
unexpected names.

- Use the same prefix for ``.bul``, ``.inp``, and ``.ctr`` files (for example,
  ``Ni111_2x2O``).

R-factor fails to find theoretical beams
========================================

Symptoms: ``crfac`` reports missing theoretical indices.

- Verify that your ``.ctr`` mappings match the beam list in the ``.res`` file.
- Check that the calculated energy range overlaps the experimental data.

Unexpected parameter defaults
=============================

Symptoms: runs succeed but results look wrong or ignore expected settings.

- Parameters can appear in any order; the last occurrence is used.
- If a parameter is missing, CLEED may fall back to an internal default.

