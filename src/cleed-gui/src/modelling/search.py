from modelling.atom import Atom
from modelling.control import Control
from modelling.elements import Elements, ELEMENTS
from modelling.iv import IV
from modelling.model import Model
from modelling.symmetry import RotationalSymmetry 

class SearchParams(object):
    def __init__(self, start_energy=50., end_energy=100.1, energy_step=4., 
                 theta=0., phi=0., epsion=1.e-3, lmax=7, 
                 v_real=-13.0, v_imag=4.0, z_only=True, symmetry=None,
                 **kwargs):
        '''initialise SearchParameters class
        
        theta and phi may be a list() if doing multiple 
        angle of incidence'''
        self.setOpticalPotential(v_real, v_imag)
        self.setEnergyRange(start_energy, end_energy, energy_step)
        self.setStartAngles(theta, phi)
        self.setLMax(lmax)
        self.setEpsilon(epsilon)
        self.setSearchZOnly(z_only)
        self.setSymmetry(symmetry)
        self.__dict__.update(kwargs)
    
    def __str__(self):
        '''return parameter string for cleed input file'''
        return None
    
    def __repr__(self):
        '''internal printing for class instance'''
        return "SearchParams(" + ", ".join(["%s=%s" % (key, self.__dict__[key])
                                            for key in self.__dict__]) + ")"
    
    def setOpticalPotential(self, real=-13.0, imaginary=4.0):
        '''set the real and imaginary parts of the optical potential'''
        self.vr = float(real)
        self.vi = float(real)
        
    def setEnergyRange(self, start, stop, step=1.):
        '''set the range of energies for LEED simulation
        
        Higher energies require significantly more computing time.
        '''
        self.initialEnergy = float(start)
        self.finalEnergy = float(stop)
        self.energyStep = float(step) 
    
    def setSearchZOnly(self, z_only=True):
        '''specify whether to search only z, otherwise xyz 
        (at additional computational cost'''
        self.searchZOnly = bool(z_only) 
    
    def setStartAngles(self, theta=0, phi=0):
        '''
        set initial angles for search
        
        Parameters
        ----------
          theta (float, int or list) - in-plane rotation
          phi (float, int or list) - azimuthal angle
          
        '''
        if isinstance(theta, list) or isinstance(phi, list):
            # then AOI search
            if isinstance(theta, list) and isinstance(phi, list):
                if len(theta) != len(phi):
                    raise IndexError("list size different for theta and phi")
                else:
                    self.theta = theta
                    self.phi = phi
            elif isinstance(theta, list):
                self.theta = theta
                self.phi = [phi] * len(theta)
            else:
                self.theta = [theta] * len(phi)
                self.phi = phi
                
        else:
            # normal search
            self.theta = theta
            self.phi = phi
    
    def setRotationalSymmetry(self, symmetry=None):
        '''set n-fold rotational symmetry with x, y centre'''
        if isinstance(symmetry, list):
            if len(symmetry) == 3:
                self.symmetry = [int(symmetry[0]),    # n-fold rotation  
                                 float(symmetry[1]),  # x-centre
                                 float(symmetry[2])   # y-centre   
                                 ]
        else:
            self.symmetry = symmetry
    
    def setLMax(self, lmax):
        '''set the maximum angular momentum quantum number
        
        Higher lmax results in more accurate results, but takes significantly 
        more computing time. 0 <= lmax <= 18 (int)
        '''
        if lmax >= 0 and lmax < 19:
            self.lmax = int(lmax)
        
    def setEpsilon(self, epsilon):
        '''set convergence criterion'''
        self.epsilon = float(epsilon)

class Search(object):
    '''container class for csearch input'''
    def __init__(self, bulk_model, surface_model, control, model_name, 
                 parameters, bulk_filename=None, input_filename=None, 
                 control_filename=None, **kwargs):
        self.setControlItems(control)
        self.__dict__.update(kwargs)
        