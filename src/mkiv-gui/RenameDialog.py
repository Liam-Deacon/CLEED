'''
Created on 11 Oct 2013

@author: Liam Deacon
'''

import os, sys, ntpath

# Import Qt modules
from PyQt4 import QtCore,QtGui,uic
from PyQt4.QtCore import *
from PyQt4.QtGui import *

import res_rc
import renameSequence

class RenameDialog(QtGui.QDialog):
    '''
    Dialog class for renaming file sequences 
    '''
    renamer = renameSequence.renameSequence()
    
    def __init__(self, parent=None):
        super(RenameDialog, self).__init__(parent)
        
         # Or more dynamically
        self.ui = uic.loadUi("RenameDialog.ui", self)
        self.ui.show()
        
        self.initUI()
        self.initRenamer()
        
        
    def initUI(self):
        # Setup slots and signals
        self.ui.checkPrefix.clicked.connect(self.ui.linePrefix.setEnabled)
        self.ui.checkSuffix.clicked.connect(self.ui.lineSuffix.setEnabled)
        self.ui.checkPrefix.stateChanged.connect(self.ui.linePrefix.setEnabled)
        self.ui.checkSuffix.stateChanged.connect(self.ui.lineSuffix.setEnabled)
        self.ui.pushRename.clicked.connect(self.rename)
        
        self.ui.linePrefix.editingFinished.connect(self.validatePrefix)
        self.ui.lineSuffix.editingFinished.connect(self.validateSuffix)
        
        # Set GUI presets
        self.ui.linePrefix.setEnabled(False)
        self.ui.lineSuffix.setEnabled(False)
        self.ui.progressBar.hide()
        

        
        
    def initRenamer(self):
        pass
        
    def rename(self):
        
        self.renamer.setAppendExtension(self.ui.checkExtension.isChecked())
        if self.ui.comboDuplicatePolicy.currentIndex() == 1:
            duplicatePolicy = 'i'
        elif self.ui.comboDuplicatePolicy.currentIndex() == 2:
            duplicatePolicy = 'ui'
        else:
            duplicatePolicy = 'ignore'
        self.renamer.setDuplicatePolicy(duplicatePolicy)
        self.renamer.setFinal(self.ui.dblEnd.value())
        self.renamer.setInitial(self.ui.dblStart.value())
        if self.ui.comboMode.currentIndex() == 1:
            self.renamer.setPrecision(precision=2) # set to 2 decimal places
        if self.ui.checkPrefix.isChecked():
            self.renamer.setPrefix(str(self.ui.linePrefix.text()))
        if self.ui.checkSuffix.isChecked():
            self.renamer.setSuffix(str(self.ui.lineSuffix.text()))
            
        filters = ["All files (*.*)", "Bitmap (*.bmp)", "Tag Image Format (*.tif *.tiff)",
                  "JPEG (*.jpg *.jpeg)", "Portable Network Graphics (*.png)",
                  "Flexible Image Transport System (*.fit *.fits)",
                  "Text files (*.txt)", 'DAT files (*.dat)' ]
        if self.renamer.files == None or self.renamer.files == []:
            files = QtGui.QFileDialog.getOpenFileNames(self.parent(), 
                                                       caption='Select Input Files...',
                                                       directory='',
                                                       filter=";;".join(filters),
                                                       selectedFilter=None)
            self.renamer.setFiles([str(file) for file in files if os.path.isfile(str(file))])
        if not os.path.isdir(str(self.renamer.output_path)):
            path = str(self.renamer.files[0])
        else:
            path = str(self.renamer.output_path)
        path = QtGui.QFileDialog.getExistingDirectory(self.parent(), 
                                                      caption='Specify Output Directory...', 
                                                      directory=os.path.dirname(path))
        self.renamer.setOutput(str(path))
        #execute renaming
        self.ui.pushRename.setText('&Cancel')
        self.renamer.rename()
        self.ui.pushRename.setText('&Rename')
        self.renamer.setFiles([])
        self.renamer.setOutput(None)
            
    def validatePrefix(self):
        validatedString = ''.join(ch for ch in str(self.ui.linePrefix.text()) if ch.isalnum() or ch in ['_', ' ', '.', '-'])
        self.ui.linePrefix.setText(validatedString)
        
        
    def validateSuffix(self):
        validatedString = ''.join(ch for ch in str(self.ui.lineSuffix.text()) if ch.isalnum() or ch in ['_', ' ', '.', '-'])
        self.ui.lineSuffix.setText(validatedString)