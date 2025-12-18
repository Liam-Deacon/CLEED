---
name: restructured-text
version: 1
description: "Guidelines for writing valid reStructuredText (.rst) in this repo (Sphinx)."
scope: "doc/**/*.rst"
---

# reStructuredText (Sphinx) guidelines

## Core rules

- Treat ``.rst`` as reStructuredText (not Markdown). Indentation and blank lines are semantic.
- Prefer Sphinx structure over ``.. include::`` for navigation: use ``.. toctree::`` so each page is a real document.
- Avoid tabs; use spaces.

## Headings

- Underlines must be **at least as long** as the title text.
- Keep heading levels consistent within a file.

Example::

  Title
  =====

  Section
  -------

## Lists

- For enumerated/bulleted lists, indent continuation paragraphs under the list item and add a blank line between items when they contain multiple paragraphs.

Example::

  1. Item title

     Continuation paragraph (indented).

  2. Next item

## Inline markup

- Escape characters that trigger markup when they are literal.
- Prefer Sphinx roles for semantic markup:

  - ``:file:`` for filenames (e.g. ``:file:`*.inp```).
  - ``:command:`` for commands.
  - ``:envvar:`` for environment variables (define them via ``.. envvar::`` somewhere in the docs).

Common pitfall::

  This breaks: CLEED *.res results file

Better::

  Use :file:`*.res` results files

## Links and references

- Labels must use the underscore form: ``.. _label_name:``.
- Use ``:ref:`label_name``` for internal links.
- Use explicit links for emails/URLs:

  - ``mailto:`` for email links.
  - Full URLs for external links.

Example::

  See :ref:`file_formats` or email `support <mailto:someone@example.com>`_.

