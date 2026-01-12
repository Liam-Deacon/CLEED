# Modern Input/Output for CLEED

This guide explains how to use the `cleed-io` tools to convert modern crystallography files (POSCAR, CIF, XYZ) into CLEED-compatible `.bul` and `.inp` files, and vice-versa.

## Motivation

Standard DFT workflows (VASP, Quantum Espresso) use "Supercell" geometries (3D periodic boxes). CLEED requires a specific "Layered" definition (Semi-infinite Bulk + Finite Overlayer). 

The `cleed-convert` tool bridges this gap by automatically slicing a 3D slab into the required LEED components.

## Installation

The tools require Python 3.8+ and the Atomic Simulation Environment (ASE).

```bash
pip install tools/
```

## Supported Formats

Via the **ASE** backend, `cleed-convert` supports reading and writing over 30 formats, including:
*   **VASP:** `POSCAR`, `CONTCAR`, `XDATCAR`
*   **Crystallography:** `.cif`
*   **XYZ:** `.xyz`, `.extxyz`
*   **Quantum Espresso:** `.in`, `.out`
*   **Gaussian:** `.log`
*   **FHI-aims:** `.in`

## Usage

### 1. Import: POSCAR $\to$ CLEED

Convert a VASP POSCAR to CLEED files:

```bash
cleed-convert --to-cleed --input POSCAR --output my_surface
```

This generates:
*   `my_surface.bul`: Contains the bulk lattice vectors and atoms.
*   `my_surface.inp`: Contains the overlayer atoms.

**Options:**
*   `--bulk-layers N`: Specify how many bottom layers constitute the bulk (default: 2).

### 2. Export: CLEED $\to$ POSCAR

Convert CLEED simulation files back to a format suitable for visualization (VESTA, Ovito):

```bash
cleed-convert --from-cleed --input my_surface.inp --bulk my_surface.bul --output RECONSTRUCTED_POSCAR --format vasp
```

This reconstructs the full 3D slab geometry.

## How it Works

1.  **Alignment:** The tool rotates the input structure so the lattice vectors $\vec{a}$ and $\vec{b}$ lie in the $xy$-plane ($z=0$), matching LEED conventions.
2.  **Slicing:** Atoms are clustered into layers based on their $z$-coordinate.
3.  **Separation:**
    *   The bottom $N$ layers are extracted to form the `.bul` file.
    *   All remaining layers (surface + adsorbate) are written to the `.inp` file.

## Integration with ViPErLEED

This tool supports the conventions used by ViPErLEED:
*   Slab is asymmetric (Bulk at bottom).
*   Vacuum is along $+z$.