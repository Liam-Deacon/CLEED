# Manual extraction notes

This directory contains the in-progress reStructuredText transcription of
`doc/CLEED_Manual.pdf`.

## Method

- The PDF uses Type 3 fonts, so direct `pdftotext` output is mostly illegible.
- We used OCR (`ocrmypdf --force-ocr`) to generate an OCR-backed PDF for text extraction.
- The OCR text is used as a starting point, then manually cleaned into RST.
- Figures are referenced as TODO placeholders; see `doc/manual/figures-and-tables.rst`.
- Tables are flagged for manual verification with page numbers.

## Known verification tasks

- Replace all `Figure TODO` references with actual figure numbers and captions.
- Transcribe tables listed in `doc/manual/figures-and-tables.rst`.
- Verify equations, code blocks, and inline symbols against the PDF.
- Confirm bibliography entries for accents and punctuation.
