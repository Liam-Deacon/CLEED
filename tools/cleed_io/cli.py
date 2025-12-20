import argparse
import sys
from ase.io import read
from cleed_io.slicer import slice_slab
from cleed_io.writer import write_bul, write_inp

def main():
    parser = argparse.ArgumentParser(description="Convert structure files (POSCAR, CIF, etc.) to CLEED .bul/.inp")
    parser.add_argument("-i", "--input", required=True, help="Input structure file (POSCAR, .cif, .xyz)")
    parser.add_argument("-o", "--output", required=True, help="Output basename (e.g. 'ni111' -> ni111.bul, ni111.inp)")
    parser.add_argument("--bulk-layers", type=int, default=2, help="Number of bottom layers to treat as bulk")
    parser.add_argument("--format", help="Input format (optional, ASE detects automatically)")
    
    args = parser.parse_args()
    
    try:
        print(f"Reading {args.input}...")
        atoms = read(args.input, format=args.format)
        
        print(f"Slicing structure ({len(atoms)} atoms)...")
        bulk, over = slice_slab(atoms, bulk_layers_count=args.bulk_layers)
        
        bul_file = f"{args.output}.bul"
        inp_file = f"{args.output}.inp"
        
        print(f"Writing {bul_file}...")
        write_bul(bul_file, bulk)
        
        print(f"Writing {inp_file}...")
        write_inp(inp_file, over, bulk)
        
        print("Done.")
        
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
