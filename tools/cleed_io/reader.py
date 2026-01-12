import numpy as np
from ase import Atoms

def parse_vector(line):
    # Format: "label: x y z"
    parts = line.split()
    # Find the numeric parts. Usually after the first token (label).
    # Sometimes comments follow.
    # CLEED format: "a1: 1.23 4.56 0.00"
    values = []
    for p in parts[1:]:
        try:
            values.append(float(p))
        except ValueError:
            pass # Ignore comments or flags
    return np.array(values[:3])

def parse_atom(line):
    # Format: "pb: Label x y z type ..."
    # Example: "pb: Ni_BVH  1.2450 -0.7188  -2.0330  dr3 ..."
    parts = line.split()
    label = parts[1]
    
    # Extract coordinates (next 3 floats)
    coords = []
    idx = 2
    while len(coords) < 3 and idx < len(parts):
        try:
            coords.append(float(parts[idx]))
        except ValueError:
            pass # Should not happen for coords
        idx += 1
        
    # Attempt to extract element symbol from Label (e.g., "Ni_BVH" -> "Ni")
    # Heuristic: Take first 1-2 chars if they are alpha.
    symbol = ""
    for char in label:
        if char.isalpha():
            symbol += char
        else:
            break
    
    # Fallback/Validation could be added here using periodictable
    if not symbol:
        symbol = "X" # Unknown
        
    return symbol, np.array(coords)

def read_cleed_files(bul_file, inp_file):
    """
    Reads CLEED .bul and .inp files and reconstructs the full unit cell.
    
    Args:
        bul_file (str): Path to .bul file.
        inp_file (str): Path to .inp file.
        
    Returns:
        Atoms: ASE Atoms object containing Bulk + Overlayer.
    """
    
    # 1. Parse Bulk File
    a1 = a2 = a3 = None
    bulk_atoms = []
    bulk_symbols = []
    
    with open(bul_file, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#') or line.startswith('c:'):
                continue
                
            if line.lower().startswith('a1:'):
                a1 = parse_vector(line)
            elif line.lower().startswith('a2:'):
                a2 = parse_vector(line)
            elif line.lower().startswith('a3:'):
                a3 = parse_vector(line)
            elif line.lower().startswith('pb:'):
                sym, pos = parse_atom(line)
                bulk_symbols.append(sym)
                bulk_atoms.append(pos)

    # 2. Parse Input File (Overlayer)
    over_atoms = []
    over_symbols = []
    # m1, m2 could be parsed here for superstructures
    
    with open(inp_file, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#') or line.startswith('c:'):
                continue
                
            if line.lower().startswith('po:'):
                sym, pos = parse_atom(line)
                over_symbols.append(sym)
                over_atoms.append(pos)
                
    # 3. Construct Unit Cell
    if a1 is None or a2 is None or a3 is None:
        raise ValueError("Lattice vectors (a1, a2, a3) not found in .bul file")
        
    cell = np.array([a1, a2, a3])
    
    # Combine atoms
    all_symbols = bulk_symbols + over_symbols
    all_positions = bulk_atoms + over_atoms
    
    if not all_positions:
        # Create empty cell if no atoms
        return Atoms(cell=cell, pbc=True)
        
    atoms = Atoms(symbols=all_symbols, positions=all_positions, cell=cell, pbc=True)
    return atoms
