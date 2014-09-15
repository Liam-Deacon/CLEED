#!/usr/bin/env python
#encoding: utf-8

##############################################################################
# Author: Liam Deacon                                                        #
#                                                                            #
# Contact: liam.deacon@diamond.ac.uk                                         #
#                                                                            #
# Copyright: Copyright (C) 2013-2014 Liam Deacon                             #
#                                                                            #
# License: MIT License                                                       #
#                                                                            #
# Permission is hereby granted, free of charge, to any person obtaining a    #
# copy of this software and associated documentation files (the "Software"), #
# to deal in the Software without restriction, including without limitation  #
# the rights to use, copy, modify, merge, publish, distribute, sublicense,   #
# and/or sell copies of the Software, and to permit persons to whom the      #
# Software is furnished to do so, subject to the following conditions:       #
#                                                                            #
# The above copyright notice and this permission notice shall be included in #
# all copies or substantial portions of the Software.                        #
#                                                                            #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR #
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   #
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    #
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    #
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        #
# DEALINGS IN THE SOFTWARE.                                                  #
#                                                                            #
##############################################################################

class Unitcell(object):
    '''
    Unitcell class        
    '''
 
    def __init__(self, a, c, matrix_3x3, **kwargs):
        '''
        Constructor for the Unitcell class
        
        Parameters
        ----------
        a : float
            The in-plane lattice vector in Angstroms
        c : float
            The out-of-plane lattice vector in Angstroms. For cubic systems this
            will be equal to a.
        matrix_3x3: ndarray
            A 3x3 matrix describing the x,y,z construction of a,b,c basis vectors
            of the unitcell. Units for x, y & z should be in terms of fractional
            coordinates. 
        alpha : float, optional
            Angle alpha in degrees.
        beta : float, optional
            Angle beta in degrees.
        gamma : float, optional
            Angle gamma in degrees.

        '''
        # Convert Angstrom input to Bohr radii
        self.set_a(a)
        self.set_c(c)
        
        # Set basis vectors
        self.set_vectors(matrix_3x3)
        
        # Set xstal system
        self.alpha = 90.0
        self.beta = 90.0
        self.gamma = 90.0
        
        # Update additional information
        self.__dict__.update(kwargs)

    # checks if two class instances are equal w.r.t. name, radius & valence
    def __eq__(self, other):
        if isinstance(other, Atom):
            return (self.a == other.a and 
                    self.c == other.c and
                    self.alpha == other.alpha and
                    self.beta == other.beta and 
                    self.gamma == other.gamma)
        else:
            return False
    
    # checks if two class instances are not equal w.r.t. name, radius & valence
    def __neq__(self, other):
        return (not self.__eq__(other))
    
    # reprinting of class object
    def __repr__(self):
        return (str(
            "Unitcell(a=%s, c=%s, alpha=%s, beta=%s, gamma=%s, basis=%s)")  
            % (self.a, self.c, self.alpha, self.beta, self.gamma, self.basis))
    
    # redefine hash method for checking uniqueness of class instance 
    def __hash__(self):
        return hash(self.__repr__())

    # set basis vectors from (3x3) matrix in fractional coordinates
    def set_vectors(self, m3x3):
        self.basis = m3x3

    # set a lattice parameter
    def set_a(self, a):
        """
        Description
        -----------
        Set the magnitude of the in-plane lattice vector a in Angstroms.
        
        Parameters
        ----------
        a: float
            The magnitude of the in-plane lattice vector in Angstroms  
        
        Notes
        -----
        To retrieve a in terms of Angstroms use 'unitcell.a', whereas the
        internal parameter 'unitcell._a' converts a into Bohr radii 
        (1 Bohr = 0.529Å), which is used for the muffin-tin potential
        calculations in libphsh (CAVPOT subroutine).
        
        """
        self.a = float(a)
        self._a = self.a / 0.529  # (1 Bohr = 0.529Å)

    # set c lattice parameter
    def set_c(self, c):
        """
        Description
        -----------
        Set the magnitude of the out-of-plane lattice vector a. 
        
        Parameters
        ----------
        c : float
            The magnitude of the in-plane lattice vector in Angstroms 
        
        Notes
        -----
        To retrieve c in terms of Angstroms use 'unitcell.c', whereas the
        internal parameter 'unitcell._c' converts c into Bohr radii 
        (1 Bohr = 0.529Å), which is used for the muffin-tin potential
        calculations in libphsh (CAVPOT subroutine).
        
        """
        self.c = float(c)
        self._c = self.c / 0.529  # (1 Bohr = 0.529Å)

    # set angle alpha in degrees 
    def set_alpha(self, alpha):
        self.alpha = float(alpha) % 360.0

    # set angle beta in degrees
    def set_beta(self, beta):
        self.beta = float(beta) % 360.0

    # set angle gamma in degrees
    def set_gamma(self, gamma):
        self.gamma = float(gamma) % 360.0


class CoordinatesError(Exception):
    '''Coordinate exception to raise and log duplicate coordinates.'''
    def __init__(self, msg):
        super(CoordinatesError).__init__(type(self))
        self.msg = "CoordinatesError: %s" % msg
        
    def __str__(self):
        return self.msg
    
    def __unicode__(self):
        return self.msg
