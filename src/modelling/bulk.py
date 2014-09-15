from __future__ import absolute_import
import os.path

from os import sys, path
sys.path.append(path.dirname(path.dirname(path.abspath(__file__))))

from modelling.atom import Atom
from modelling.model import Model
from modelling.unitcell import Unitcell

        
class Bulk(object):
    '''LEED control class'''
    def __init__(self, Unitcell, atoms=[], vr=-13.0, vi=4.0, 
                 ei=50., ef=100.1, es=4., theta=0., phi=0., ep=1.e-3, lm=7):
        '''contolList is an array of ControlItems'''
        self.setControlItems(controlList)
        self.setOpticalPotential(vr, vi)
        self.setEnergyRange(ei, ef, es)

    def addAtom(self, atom):
        '''append an atom model'''
        if not isinstance(atom, Atom):
            raise TypeError('%s is not a valid Atom instance' 
                                % str(atom))
        if atom not in self.atoms:
            self.atoms.append(atom)
        
    def clear(self):
        '''clear model'''
        self.unitcell = None
        self.atoms = []

    def setAtoms(self, atoms):
        '''set atoms in model from Atom() list'''
        self.atoms = []
        for atom in atoms:
            if not isinstance(atom, Atom):
                raise TypeError('%s is not a valid Atom instance' 
                                % str(atom))
            if atom not in self.atoms:
                self.atoms.append(atom)
  
    def setComment(self, comment):
        '''set comment for control'''
        self.comment = str(comment)
    
    def setUnitcell(self, unitcell):
        if not isinstance(unitcell, Unitcell):
            raise TypeError('%s is not a valid Unitcell instance' 
                            % str(unitcell))
        self.unitcell = unitcell
    
    def loadFromFile(self, filename):
        '''load a control structure from disk'''
        comment = ""
        try:
            with open(filename, 'r') as f:
                for line in f:
                    line = str(line)
                    if line.lstrip().startswith('#'):
                        comment += line.lstrip()  # may need to make this smarter
                    elif "ef=" in line or "id=" in line or "ti=" or "wt=" in line:
                        args = line.lstrip().split(':')
                        ef = None
                        ti = []
                        id = None
                        wt = 1.
                        
                        for arg in args:
                            # remove surrounding whitespace
                            arg = arg.lstrip().rstrip().rstrip('#')
                            if arg.startswith("ef="):
                                ef = arg.lstrip("ef=")
                            elif arg.startswith("id="):
                                id = arg.lstrip("id=")
                            elif arg.startswith("ti="):
                                reflexes = arg.lstrip("ti=").split('+')
                                for reflex in reflexes:
                                    # create tuple for reflex
                                    reflex = eval(reflex.lstrip().rstrip())
                                    # add tuple to reflexes
                                    ti.append(Reflex(*reflex))
                            elif arg.startswith("wt="):
                                wt = arg.lstrip("wt=")
                        
                        if not id:
                            id = os.path.splitext(os.path.basename(ef))[0]
                        
                        item = ControlItem(ef, ti, id, wt)
                        self.addControlItem(item)
                        
                    self.setComment(comment)
        except IOError:
            assert(IOError("Cannot open control file: '%s'" % filename))                        
    
    def saveToFile(self, filename):
        '''save a control structure to disk'''
        try:
            with open(filename, 'w') as f:
                f.write(self.comment)
                for item in self.controlItems:
                    f.write(str(item) + '\n')
                f.write('\n')
        except IOError:
            assert(IOError("Cannot save control file: '%s'" % filename)) 