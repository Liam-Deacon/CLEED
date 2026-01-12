import os
import unittest
import numpy as np
from ase import Atoms
from ase.build import fcc111, add_adsorbate
from cleed_io.slicer import slice_slab
from cleed_io.reader import read_cleed_files
from cleed_io.writer import write_bul, write_inp

class TestCleedIO(unittest.TestCase):
    def setUp(self):
        # Create a mock slab: Ni(111) with O adsorbate
        # 4 Layers: 2 Bulk, 2 Surface
        slab = fcc111('Ni', size=(1, 1, 4), vacuum=10.0)
        add_adsorbate(slab, 'O', 1.2, 'fcc')
        self.slab = slab
        self.bul_file = "test.bul"
        self.inp_file = "test.inp"

    def tearDown(self):
        if os.path.exists(self.bul_file):
            os.remove(self.bul_file)
        if os.path.exists(self.inp_file):
            os.remove(self.inp_file)

    def test_slice_slab(self):
        """Test if slice_slab correctly identifies bulk and overlayer atoms."""
        bulk_layers = 2
        bulk, over = slice_slab(self.slab, bulk_layers_count=bulk_layers)
        
        # Ni slab 1x1x4 = 4 Ni atoms. + 1 O atom = 5 atoms.
        # Bulk should be bottom 2 layers -> 2 atoms
        # Over should be top 2 layers + adsorbate -> 2 Ni + 1 O = 3 atoms
        
        self.assertEqual(len(bulk), 2)
        self.assertEqual(len(over), 3)
        self.assertEqual(bulk[0].symbol, 'Ni')
        self.assertEqual(over[-1].symbol, 'O')

    def test_round_trip(self):
        """Test Write -> Read cycle."""
        bulk, over = slice_slab(self.slab, bulk_layers_count=2)
        
        write_bul(self.bul_file, bulk)
        write_inp(self.inp_file, over, bulk)
        
        # Read back
        reconstructed = read_cleed_files(self.bul_file, self.inp_file)
        
        # Check atom counts
        self.assertEqual(len(reconstructed), len(self.slab))
        
        # Check symbols
        # Note: Order might differ depending on how write/read handles lists
        # But composition should be same.
        self.assertEqual(reconstructed.get_chemical_formula(), self.slab.get_chemical_formula())
        
        # Check Lattice Vectors (a1, a2)
        # a3 might differ because slice_slab estimates it or uses vacuum
        cell_orig = self.slab.get_cell()
        cell_new = reconstructed.get_cell()
        
        np.testing.assert_array_almost_equal(cell_orig[0], cell_new[0], decimal=3)
        np.testing.assert_array_almost_equal(cell_orig[1], cell_new[1], decimal=3)

if __name__ == '__main__':
    unittest.main()
