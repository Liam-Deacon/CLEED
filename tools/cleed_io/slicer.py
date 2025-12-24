import numpy as np
from ase import Atoms
from ase.geometry import cell_to_cellpar

def align_surface(atoms: Atoms, vacuum_direction='z', tolerance=1e-3) -> Atoms:
    """
    Aligns the atoms object such that the periodic surface vectors (a, b)
    lie in the xy-plane (z=0) and the surface normal points along +z.
    
    This matches the CLEED convention where z is the surface normal.
    """
    # 1. Identify the non-periodic direction or the longest vector if 3D periodic
    # For a standard slab, we assume c is the vacuum direction.
    
    cell = atoms.get_cell()
    pbc = atoms.get_pbc()
    
    # We assume standard ASE slab convention: 3rd vector is out-of-plane
    # Check if we need to rotate.
    
    # Vector a (cell[0]) should be along x
    # Vector b (cell[1]) should be in xy plane
    # Vector c (cell[2]) points up
    
    # Standardize orientation: a along x
    atoms.rotate(cell[0], 'x', rotate_cell=True)
    
    # Now rotate around x to put b in xy plane
    # We need the normal of (a, b) to be along z
    
    current_cell = atoms.get_cell()
    # Cross product of a and b
    normal = np.cross(current_cell[0], current_cell[1])
    
    # Align this normal to z
    atoms.rotate(normal, 'z', rotate_cell=True)
    
    # Ensure c is positive z
    final_cell = atoms.get_cell()
    if final_cell[2][2] < 0:
        # Flip z? No, that mirrors the structure. 
        # Just ensure the slab is oriented correctly.
        # ViPErLEED: "positive z-direction pointing away from the surface into the vacuum"
        pass

    return atoms

def cluster_layers(atoms: Atoms, tolerance=0.2):
    """
    Groups atoms into layers based on Z-coordinate.
    Returns a list of lists of atom indices.
    """
    z_coords = atoms.positions[:, 2]
    sorted_indices = np.argsort(z_coords)
    
    layers = []
    if len(sorted_indices) == 0:
        return layers
        
    current_layer = [sorted_indices[0]]
    current_z = z_coords[sorted_indices[0]]
    
    for idx in sorted_indices[1:]:
        z = z_coords[idx]
        if abs(z - current_z) < tolerance:
            current_layer.append(idx)
        else:
            layers.append(current_layer)
            current_layer = [idx]
            current_z = z
            
    layers.append(current_layer)
    return layers

def slice_slab(atoms: Atoms, bulk_layers_count=2):
    """
    Splits an ASE Atoms object into Bulk and Overlayer parts for CLEED.
    
    Args:
        atoms: The ASE structure.
        bulk_layers_count: Number of bottom layers to treat as 'Bulk'.
                           If None, attempts to detect (future feature).
                           Default is 2 (minimum for define a bulk spacing).
                           
    Returns:
        bulk_atoms (Atoms): The atoms forming the bulk unit.
        overlayer_atoms (Atoms): The atoms forming the surface/overlayer.
    """
    # 1. Align
    aligned = align_surface(atoms.copy())
    
    # 2. Cluster
    layers = cluster_layers(aligned)
    
    if len(layers) < bulk_layers_count:
        raise ValueError(f"Structure has {len(layers)} layers, but {bulk_layers_count} required for bulk.")
        
    # 3. Split
    # CLEED Bulk: defined by the periodicity of the bottom layers.
    # We take the atoms in the 'bulk_layers_count' bottom layers.
    
    bulk_indices = []
    for i in range(bulk_layers_count):
        bulk_indices.extend(layers[i])
        
    overlayer_indices = []
    for i in range(bulk_layers_count, len(layers)):
        overlayer_indices.extend(layers[i])
        
    bulk_atoms = aligned[bulk_indices]
    overlayer_atoms = aligned[overlayer_indices]
    
    # 4. Define Bulk Unit Cell (a3)
    # The 'c' vector of the bulk_atoms needs to be the inter-layer bulk spacing vector
    # NOT the huge vacuum slab vector.
    # If we have >1 bulk layer, we can estimate a3 from the difference in average Z.
    # For now, we retain the in-plane vectors and set a3 to the slab c (which is wrong for .bul).
    # Correct logic: a3 should be the 3D bulk periodicity. 
    # If we assume the input *is* a slab of a known bulk, the user might need to provide a3
    # or we guess it from the bottom 2 layers.
    
    if len(layers) >= 2:
        z1 = np.mean(aligned.positions[layers[0], 2])
        z2 = np.mean(aligned.positions[layers[1], 2])
        dz = z2 - z1
        # This assumes vertical stacking.
        # Ideally, we find the translation vector between layer 0 and layer 1.
        
        # Heuristic: Use the slab cell for a1, a2.
        # Construct a3 from the layer spacing.
        cell = aligned.get_cell()
        bulk_atoms.set_cell([cell[0], cell[1], [0, 0, dz * len(layers)]]) # Dummy a3
        
    return bulk_atoms, overlayer_atoms
