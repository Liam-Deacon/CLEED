##############################################################################
# Author: Liam Deacon                                                        #
#                                                                            #
# Contact: liam.deacon@diamond.ac.uk                                         #
#                                                                            #
# Created on 23 Jun 2014                                                       #
#                                                                            #
# Copyright: Copyright (C) 2014 Liam Deacon                                #
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
'''

'''
from __future__ import print_function, unicode_literals
from __future__ import absolute_import, division, with_statement

from PyQt4.QtGui import QTreeWidgetItem, QIcon, QFileDialog
from PyQt4.QtCore import QFileInfo, Qt
import os.path
from gui import res_rc 

class ProjectItem(QTreeWidgetItem):
    '''class for project items'''
    def __init__(self, parent=None, path=None):
        super(ProjectItem, self).__init__(parent)
        self.setProjectPath(path)
        self.setIcon(0, QIcon(":/folder_fill.svg"))
        self.Name = "New_Project"
        self.setFlags(self.flags() | Qt.ItemIsEditable)
        self.setProjectPath(path)
    
    def setProjectPath(self, path):
        self.Path = path
        self.Name = QFileInfo(path).baseName()
        self.setText(0, self.Name)
        self.setToolTip(0, path)
    
        
class ModelItem(QTreeWidgetItem):
    '''class for project items'''
    def __init__(self, parent=None, path=None):
        super(ModelItem, self).__init__(parent)
        self.setModelName(path)
        self.setIcon(0, QIcon(":/atoms.svg"))
        self.Name = "New_Model"
        self.setFlags(self.flags() | Qt.ItemIsEditable)
        self.setModelName(path)
    
    def setModelName(self, path):
        self.Path = path
        self.Name = QFileInfo(path).baseName()
        self.setText(0, self.Name)
        self.setToolTip(0, path)


class InputItem(QTreeWidgetItem):
    '''class for project items'''
    def __init__(self, parent=None, path=None):
        super(InputItem, self).__init__(parent)
        self.setModelName(path)
        self.setIcon(0, QIcon(":/atoms.svg"))
        self.Name = "New_Model"
        self.setFlags(self.flags() | Qt.ItemIsEditable)
        self.setModelName(path)
    
    def setModelName(self, path):
        self.Path = path
        self.Name = QFileInfo(path).baseName()
        self.setText(0, self.Name)
        self.setToolTip(0, path)

      
class BulkItem(QTreeWidgetItem):
    '''class for project items'''
    def __init__(self, parent=None, path=None):
        super(InputItem, self).__init__(parent)
        self.setModelName(path)
        self.setIcon(0, QIcon(":/atoms.svg"))
        self.Name = "New_Model"
        self.setFlags(self.flags() | Qt.ItemIsEditable)
        self.setModelName(path)
    
    def setModelName(self, path):
        self.Path = path
        self.setText(0, 'Bulk')
        self.setToolTip(0, path)

