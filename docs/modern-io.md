# Modern Input/Output for CLEED

This guide explains how to use the `cleed-io` tools to convert modern crystallography files (POSCAR, CIF, XYZ) into CLEED-compatible `.bul` and `.inp` files.

## Motivation

Standard DFT workflows (VASP, Quantum Espresso) use "Supercell" geometries (3D periodic boxes). CLEED requires a specific "Layered" definition (Semi-infinite Bulk + Finite Overlayer). 

The `cleed-convert` tool bridges this gap by automatically slicing a 3D slab into the required LEED components.

## Installation

The tools require Python 3.8+ and the Atomic Simulation Environment (ASE).

```bash
pip install tools/
```

## Usage

### Converting a VASP POSCAR

Assume you have a `POSCAR` file representing a Ni(111) slab with a CO molecule adsorbed.

```bash
cleed-convert --input POSCAR --output ni111_co
```

This will generate:
*   `ni111_co.bul`: Contains the bulk lattice vectors and the bottom atomic layers defined as the bulk unit.
*   `ni111_co.inp`: Contains the adsorbate and surface reconstruction atoms.

### Specifying Bulk Layers

By default, the bottom 2 atomic layers are treated as the "Bulk". You can adjust this:

```bash
# Treat the bottom 4 layers as bulk
cleed-convert --input structure.cif --output surface --bulk-layers 4
```

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
