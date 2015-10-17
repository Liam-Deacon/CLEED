#!/usr/bin/env python
#encoding: utf-8

##############################################################################
# Author: Liam Deacon                                                        #
#                                                                            #
# Contact: liam.m.deacon@gmail.com                                           #
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

from modelling import elements

class Atom(object):
    '''
    Atom class for input into cluster model for muffin-tin potential
    calculations.
    '''

    def __init__(self, element, coordinates=[0., 0., 0.], **kwargs):
        '''
        Constructor for Atom class.
        
        Parameters
        ----------
        
        element : str or int
            This is either the elemental symbol, name or atomic number.
        coordinates : list[float, float, float] or ndarray
            The fractional coordinates within the unitcell in terms of the
            basis vector a.
        tag : str, optional
            Add a name tag to this element (useful if dealing with multiple
            atoms of the same element in a given model). Default is the
            symbol for that element - numeric ids may be appended in the model
            class.
        radius : float, optional
            The muffin-tin radius of the atom in Angstroms (default is to 
            lookup 'atmrad' in the element dictionary).
        valence : int, optional
            The valency of the atom (default is to assume neutral atom).
        occupancy : float, optional
            The fractional occupancy of the atom in the given site.
        
        '''
        self.element = elements.ELEMENTS[element]
        self.coordinates = None  # dummy
        self.set_coordinates(coordinates)
        self.name = self.element.name.title()
        self.tag = self.element.symbol.title()
        self.Z = self.element.protons
        self.radius = self.element.atmrad
        self.valence = 0
        if 'valence' in kwargs:
            if 'radius' not in kwargs:
                # assume covrad for non-zero valency
                self.radius = self.element.covrad
        self.__dict__.update(kwargs)
        
    
    # checks whether two atoms are equal w.r.t. name, radius and valence
    def __eq__(self, other):
        if isinstance(other, Atom):
            return (self.name == other.name and 
                    self.radius == other.radius and
                    self.valence == other.valence)
        else:
            return False
    
    # checks whether two atoms are not equal w.r.t. name, radius and valence
    def __neq__(self, other):
        return (not self.__eq__(other))
    
    # reprinting of Atom object
    def __repr__(self):
        return (str("Atom(%s, tag='%s', radius=%s, " 
                "valence=%s)") % (self.name, self.tag, 
                                  self.radius, self.valence))
    
    # redefine hash method for checking uniqueness of class instance 
    def __hash__(self):
        return hash(self.__repr__())
    
    # set coordinates of atom within unitcell in terms of a
    def set_coordinates(self, coordinates):
        try:
            self.coordinates = coordinates
            self._coordinates = [r / 0.529 for r in coordinates]
        except any as e:
            raise e
    
    # set valence of atom
    def set_valence(self, valency):
        '''Sets the valency of the atom'''
        self.valence = float(valency)
        
    # set muffin-tin radius of atom 
    def set_mufftin_radius(self, radius):
        """
        Sets the muffin-tin radius of the atom in Angstroms.
        """
        try:
            self.radius = float(radius)
            self._radius = self.radius / 0.529  # in Bohr radii
        except:
            pass