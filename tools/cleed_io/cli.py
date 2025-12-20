import argparse
import sys
import os
from ase.io import read, write
from cleed_io.slicer import slice_slab
from cleed_io.writer import write_bul, write_inp
from cleed_io.reader import read_cleed_files

def main():
    parser = argparse.ArgumentParser(description="Convert between structure files (POSCAR, CIF) and CLEED (.bul/.inp)")
    
    # Mode selection
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--to-cleed", action="store_true", help="Convert FROM standard format TO CLEED (.bul/.inp)")
    group.add_argument("--from-cleed", action="store_true", help="Convert FROM CLEED (.bul/.inp) TO standard format")
    
    # Common args
    parser.add_argument("-i", "--input", help="Input file (Standard format OR .inp file for from-cleed mode)")
    parser.add_argument("-b", "--bulk", help="Bulk file (.bul), required for --from-cleed mode")
    parser.add_argument("-o", "--output", required=True, help="Output filename (Basename for to-cleed, Full name for from-cleed)")
    parser.add_argument("--format", help="ASE File format (e.g. 'vasp', 'cif', 'xyz'). Auto-detected if omitted.")
    
    # To-CLEED specific
    parser.add_argument("--bulk-layers", type=int, default=2, help="Number of bottom layers to treat as bulk (to-cleed only)")
    
    args = parser.parse_args()
    
    try:
        if args.to_cleed:
            if not args.input:
                parser.error("--to-cleed requires --input")
                
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
            
        elif args.from_cleed:
            if not args.input or not args.bulk:
                parser.error("--from-cleed requires --input (.inp) and --bulk (.bul)")
                
            print(f"Reading CLEED files: {args.bulk} + {args.input}...")
            atoms = read_cleed_files(args.bulk, args.input)
            
            print(f"Writing {args.output}...")
            write(args.output, atoms, format=args.format)
            
        print("Done.")
        
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()