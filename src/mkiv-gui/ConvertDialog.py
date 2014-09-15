'''
Created on 11 Oct 2013

@author: Liam Deacon
'''

import os
import sys
import ntpath

try:
    import skimage
    from skimage import io
    
except:
    pass

# Import Qt modules
from PyQt4 import QtCore,QtGui,uic
from PyQt4.QtCore import *
from PyQt4.QtGui import *

import res_rc

class ConvertDialog(QtGui.QDialog):
    '''
    Dialog class for renaming file sequences 
    '''
    files = []
    format = 'bmp'
    bits = 16
    type = float
    grayscale = True
    
    def __init__(self, parent=None, files=[], grayscale=None, type=None, format='bmp', bit_depth=16):
        super(ConvertDialog, self).__init__(parent)
        
         # Or more dynamically
        self.ui = uic.loadUi("ConvertDialog.ui", self)
        self.initUI()
        
        self.setGrayscale(grayscale)
        self.setFormat(format)
        self.setType(type)
        self.setBitDepth(bit_depth)
        
        
        self.ui.show()
        
        
        
        
    def initUI(self):
        # Setup slots and signals
        self.ui.pushConvert.clicked.connect(self.convert)
        self.ui.checkGrayscale.stateChanged.connect(self.setGrayscale)
        self.ui.comboFormat.currentIndexChanged.connect(self.setFormat)
        
    def setBitDepth(self, bits=None):
        if bits != None:
            bits = int(bits)
            if bits == 8 or bits == 16 or bits == 32 or bits == 64:
                self.bits = bits
        else:
            try:
                self.bits = pow(2, self.ui.comboBitDepth.currentIndex+3)
            except:
                pass 
        
    def setFormat(self, string=None):
        if string != None:
            if string in ['bmp', 'fits', 'png', 'jpg', 'tif']:
                self.format = string
        else:
            self.format = str(self.ui.comboFormat.currentText())
        
    def setGrayscale(self, boolean=None):
        if boolean != None:
            self.grayscale = bool(boolean)
        else:
            self.grayscale = self.ui.checkGrayscale.isChecked()
        
        
    def setType(self, type=None):
        if type != None:
            pass
        
    def convert(self):
        if self.files == None:
            filters = ["All files (*.*)", "Bitmap (*.bmp)", "Tag Image Format (*.tif *.tiff)",
                       "JPEG (*.jpg *.jpeg)", "Portable Network Graphics (*.png)"]
            filters.append("Flexible Image Transport System (*.fit *.fits)")
            files = QtGui.QFileDialog.getOpenFileNames(parent=None, 
                                                       caption='Select files to convert...',
                                                       directory=str(self.lastDirectory),
                                                       filter=";;".join(filters),
                                                       selectedFilter=None)
            self.files = [str(file) for file in files if os.path.isfile(str(file))]
            if not self.files:
                sys.stderr.write('No files selected - aborting conversion\n')
                sys.stderr.flush()
                                 
        print('files', self.files)                       
        print('grayscale', self.grayscale)
        print('format', self.format)
        
        
def main():
    # Again, this is boilerplate, it's going to be the same on
    # almost every app you write
    app = QtGui.QApplication(sys.argv)
    window=ConvertDialog()
    window.show()
    # It's exec_ because exec is a reserved word in Python
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()