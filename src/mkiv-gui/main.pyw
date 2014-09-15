#!/usr/local/bin/python2.7
# encoding: utf-8
'''
Created on 9 Oct 2013

@author: Liam Deacon
'''

# Import standard libraries
import logging, ntpath, os, platform, sys

# Define globals
__APP_AUTHOR__ = 'Liam Deacon'
__APP_COPYRIGHT__ = '\xa9'+'2013 {0}'.format(__APP_AUTHOR__)
__APP_DESCRIPTION__ = 'A simple Python-based program \nfor LEED-IV data extraction'
__APP_EMAIL__ = 'liam.m.deacon@gmail.com'
__APP_LICENSE__ = 'MIT License'
__APP_NAME__ = 'Extract-PyV'
__APP_VERSION__ = '0.1-alpha'
__PYTHON__ = "{0}.{1}.{2}".format(sys.version_info.major,
                                         sys.version_info.minor,
                                         sys.version_info.micro, 
                                         sys.version_info.releaselevel)


# Import Scientific libraries
import numpy as np
import scipy as sp

try:
    import configparser
except ImportError:
    import ConfigParser as configparser  # Python 2

# Import Image modules
import skimage as ski
from skimage import io

try:
    
    from PIL import Image, ImageQt
except ImportError:
    try: 
        import Image
    except:
        from subprocess import call
        try:
            sys.stderr.write('PIL module not found - attempting to resolve automatically...\n')
            sys.stderr.flush()
            call('pip install', 'PIL')
        except:
            sys.stderr.write('PIL module not installed (essential) - exiting...\n')
            sys.stderr.flush()
            sys.exit(-1)

SUPPORTED_FORMATS = ['.bmp', '.jpeg', '.jpg', '.png', '.tif', '.tiff']
IV_FORMATS = ['*.dat', '*.iv', '*.fsm', '*.txt']



try:
    import pyfits
    SUPPORTED_FORMATS.append('.fit')
    SUPPORTED_FORMATS.append('.fits')
except ImportError:
    from subprocess import call
    try:
        sys.stderr.write('FITS module not found - attempting to resolve automatically...\n')
        sys.stderr.flush()
        call('pip install', 'pyfits')
    except:
        sys.stderr.write('FITS module not installed - support disabled\n')
        sys.stderr.write('Please try manually installing the pyfits or astropy modules\n')
        sys.stderr.flush()

# Platform specific setup
if platform.system() is 'Windows':
    from ctypes import windll
    # Tell Windows Python is merely hosting the application (taskbar icon fix)
    windll.shell32.SetCurrentProcessExplicitAppUserModelID(__APP_NAME__)

# Import Qt modules
from PyQt4 import QtCore,QtGui,uic
from PyQt4.QtCore import *
from PyQt4.QtGui import *

def determineGuiFrontend():
    """Determine which GUI toolkit to use"""
    try:
        guiFrontend = 'PySide {0}'.format(PySide.__version__)
    except NameError:
        guiFrontend = None
    
    if guiFrontend is None:
        try:
            guiFrontend = 'PyQt {0}'.format(PYQT_VERSION_STR)
        except NameError:
            guiFrontend = None
            
    if guiFrontend != None:
        return "Qt {0} (PyQt {1})".format(QtCore.qVersion(),guiFrontend)

__APP_GUI__ = determineGuiFrontend()

# Load resources & local modules
import res_rc
import RenameDialog, ConvertDialog
from fixpath import * 

# Create a class for our main window
class Main(QtGui.QMainWindow):
    """Main Class for creation of GUI"""
    def __init__(self):
        """MainWindow initialisation"""
        QtGui.QMainWindow.__init__(self)

        # This is always the same
        #self.ui=Ui_MainWindow()
        #self.ui.setupUi(self)
        
        # Or more dynamically
        self.ui = uic.loadUi("MainWindow.ui", self)
        self.ui.show()
        
        # Setup variables
        self.logger = logging.getLogger(__APP_NAME__)
        self.spots = []
        self.images = []
        self.imageFiles = []
        self.ivs = []
        self.ivFiles = []
        self.reference = ''
        self.startEnergy = None
        self.finalEnergy = None
        self.gun = [None, None]
        self.maxWidth = 0
        self.maxHeight = 0
        self.lastDirectory = None 
        self.lastImportFilter = None
        self.maskFilePath = None
        self.config = configparser.SafeConfigParser()
        self.configFilePath = fix_path(ntpath.abspath(__APP_NAME__+'.conf'))
        self.renameDialog = None
        self.convertDialog = None
        self.view = 0
        
        
        self.init()
        self.initUi()

        if 'APPDATA' in os.environ:
            appConfigPath = ntpath.join(os.environ['APPDATA'], __APP_NAME__)
        elif 'XDG_CONFIG_HOME' in os.environ:
            appConfigPath = ntpath.join(os.environ['XDG_CONFIG_HOME'], __APP_NAME__)
        else:
            appConfigPath = ntpath.join(os.environ['HOME'], '.config', __APP_NAME__)
        
        self.configFilePath = fix_path(ntpath.join(appConfigPath, __APP_NAME__+'.conf'))
        self.config.read(self.configFilePath)
        self.loadConfig()
    
    
    # Show about dialog
    def about(self):
        """Display 'About' dialog"""
        text = __APP_DESCRIPTION__
        text += '\n\nAuthor: {0} \nEmail: {1}'.format(__APP_AUTHOR__, __APP_EMAIL__)
        text += '\n\nApp version: {0}'.format(__APP_VERSION__)
        text += '\n{0}'.format(__APP_COPYRIGHT__)
        text += '\n' + __APP_LICENSE__
        text += '\n\nPython: {0}'.format(__PYTHON__)
        text += '\nGUI frontend: {0}'.format(__APP_GUI__)

        msg = QMessageBox.about(self, 'Extract-PyV', text)
    
    # Display about dialog
    def aboutQt(self):
        """Display Qt dialog"""
        QtGui.QApplication.aboutQt()
    
    
    # Select mask file using filedialog
    def addMask(self):
        filters = ["All files (*.*)", "Bitmap (*.bmp)", "Tag Image Format (*.tif *.tiff)",
                  "JPEG (*.jpg *.jpeg)", "Portable Network Graphics (*.png)"]
        if '.fits' in SUPPORTED_FORMATS:
            filters.append("Flexible Image Transport System (*.fit *.fits)")
            
        mask = QtGui.QFileDialog.getOpenFileName(parent=self, 
                                                      caption='Select Mask...',
                                                      directory=str(self.maskFilePath),
                                                      filter=";;".join(filters))
        # Only add if new path is a file
        if os.path.isfile(str(mask)):
            self.maskFilePath = str(mask)
            self.ui.statusbar.showMessage('Mask set as \"{0}\"'.format(self.maskFilePath), 5000)
    
    
    # Overload exit event to write configuration before exiting app
    def closeEvent(self, evnt):
        self.writeConfig()
        sys.exit(0)

       
    def imageContextMenuEvent(self, event):
        """Custom context menu event for images"""
        menu = QtGui.QMenu(self)

        action = menu.addAction(self.ui.actionAboutQt)

        menu.exec_(event.globalPos())
 
        
    # Check for app updates
    def getUpdate(self):
        """Check for app updates"""
        QtGui.QMessageBox.information(self, "Update Extract-PyV", "No updates available")
        
        
    def dialogConvert(self):
        """Convert dialog for sequence of images"""
        #Initialise
        dialog = ConvertDialog.ConvertDialog(parent=self)
        dialog.setAttribute(QtCore.Qt.WA_DeleteOnClose)
        dialog.finished.connect(self.dialogRenameWritePrefs)
        
        # Load preferences
        for key in ['Grayscale', 'BitDepth', 'Format', 'Type', 'OutputPath', 'LastDirectory']:
            try:
                if key == 'Prefix':
                    dummy = self.config.get('RenameDialog', key)
                    if dummy != '' and dummy != 'None' and dummy != str(None): 
                        dialog.ui.linePrefix.setText(dummy)
                        dialog.ui.checkPrefix.setChecked(True)
                elif key == 'Suffix':
                    dummy = self.config.get('RenameDialog', key)
                    if dummy != '' and dummy != 'None' and dummy != str(None): 
                        dialog.ui.lineSuffix.setText(dummy)
                        dialog.ui.checkSuffix.setChecked(True)
                elif key == 'Extension':
                    dialog.ui.checkExtension.setChecked(self.config.getboolean('ConvertDialog', key))
                elif key == 'Start':
                    dialog.ui.dblStart.setValue(self.config.getfloat('ConvertDialog', key))
                elif key == 'End':
                    dialog.ui.dblEnd.setValue(self.config.getfloat('RenameDialog', key))
                elif key == 'OutputPath':
                    dummy = self.config.get('RenameDialog', key)
                    if dummy != '' and dummy != 'None' and dummy != str(None): 
                        dialog.renamer.setOutput(dummy)
                elif key == 'Mode':
                    dialog.ui.comboMode.setCurrentIndex(self.config.getint('RenameDialog', key))
                elif key == 'DuplicatePolicy':
                    dialog.ui.comboDuplicatePolicy.setCurrentIndex(self.config.getint('RenameDialog', key))
            except configparser.Error as e:
                self.logger.debug(str(e))
            
        # Execute
        self.convertDialog = dialog
        dialog.exec_()
        
    def dialogRename(self):
        """Rename dialog for sequence of images"""
        #Initialise
        dialog = RenameDialog.RenameDialog(parent=self)
        dialog.renamer.setFiles(self.imageFiles)
        dialog.setAttribute(QtCore.Qt.WA_DeleteOnClose)
        dialog.finished.connect(self.dialogRenameWritePrefs)
        
        # Load preferences
        for key in ['Prefix', 'Suffix', 'Extension', 'OutputPath', 'Start', 'End', 'Mode', 'DuplicatePolicy']:
            try:
                if key == 'Prefix':
                    dummy = self.config.get('RenameDialog', key)
                    if dummy != '' and dummy != 'None' and dummy != str(None): 
                        dialog.ui.linePrefix.setText(dummy)
                        dialog.ui.checkPrefix.setChecked(True)
                elif key == 'Suffix':
                    dummy = self.config.get('RenameDialog', key)
                    if dummy != '' and dummy != 'None' and dummy != str(None): 
                        dialog.ui.lineSuffix.setText(dummy)
                        dialog.ui.checkSuffix.setChecked(True)
                elif key == 'Extension':
                    dialog.ui.checkExtension.setChecked(self.config.getboolean('RenameDialog', key))
                elif key == 'Start':
                    dialog.ui.dblStart.setValue(self.config.getfloat('RenameDialog', key))
                elif key == 'End':
                    dialog.ui.dblEnd.setValue(self.config.getfloat('RenameDialog', key))
                elif key == 'OutputPath':
                    dummy = self.config.get('RenameDialog', key)
                    if dummy != '' and dummy != 'None' and dummy != str(None): 
                        dialog.renamer.setOutput(dummy)
                elif key == 'Mode':
                    dialog.ui.comboMode.setCurrentIndex(self.config.getint('RenameDialog', key))
                elif key == 'DuplicatePolicy':
                    dialog.ui.comboDuplicatePolicy.setCurrentIndex(self.config.getint('RenameDialog', key))
            except configparser.Error as e:
                self.logger.debug(str(e))
            
        # Execute
        self.renameDialog = dialog
        dialog.exec_()

    
    # Store last preferences to file
    def dialogRenameWritePrefs(self):
        try:
            self.config.add_section('RenameDialog')
        except configparser.DuplicateSectionError:
            pass
        section = 'RenameDialog'
        self.config.set(section, 'Prefix', str(self.renameDialog.ui.linePrefix.text()))
        self.config.set(section, 'Suffix', str(self.renameDialog.ui.lineSuffix.text()))
        self.config.set(section, 'Extension', str(bool(self.renameDialog.ui.checkExtension.isChecked())))
        dummy = str(self.renameDialog.renamer.output_path) 
        if dummy != '' and dummy != 'None' and dummy != str(None): 
            self.config.set(section, 'OutputPath', str(self.renameDialog.renamer.output_path))
        self.config.set(section, 'Start', str(self.renameDialog.ui.dblStart.value()))
        self.config.set(section, 'End', str(self.renameDialog.ui.dblEnd.value()))
        self.config.set(section, 'Mode', str(self.renameDialog.ui.comboMode.currentIndex()))
        self.config.set(section, 'DuplicatePolicy', str(self.renameDialog.ui.comboDuplicatePolicy.currentIndex()))


    def help(self):
        """Display help"""
        QtGui.QMessageBox.information(self, 'Help', 'Help is not currently available')
    
        
    def imageContextMenu(self):
        menu = QtGui.QMenu()
        menu.addAction(self.ui.actionRename)
        menu.show()
        
    
    # Initialise non-GUI components
    def init(self):
        """Initialise non-GUI components"""
        ######################################
        # APP LOGGING
        ######################################
        
        # create logger with 'spam_application'
        self.logger.setLevel(logging.DEBUG)
        
        # create file handler which logs all messages
        
        fh = logging.FileHandler(os.path.join(os.environ['TEMP'],__APP_NAME__+str('.log')))
        
        formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        fh.setFormatter(formatter)
        fh.setLevel(logging.INFO)
        
        # create console handler with a higher log level
        ch = logging.StreamHandler()
        ch.setLevel(logging.WARNING)
        ch.setFormatter(formatter)
        
        # add the handlers to the logger
        self.logger.addHandler(ch)
        self.logger.addHandler(fh)
    
    def wheelEvent(self, event):
        """Custom wheelEvent for image scrolling"""
        if self.view == 0:
            delta = np.sign(event.delta())*-1
            index = self.ui.imageSlider.value()
            min = self.ui.imageSlider.minimum()
            max = self.ui.imageSlider.maximum()
            if delta+index >= min and delta+index <= max:
                self.ui.imageSlider.setValue(delta+index)
            print(self.editer)
        
    # Setup extra UI elements
    def initUi(self):
        self.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        
        # Setup slots
        self.ui.actionAbout.triggered.connect(self.about)
        self.ui.actionAboutQt.triggered.connect(self.aboutQt)
        self.ui.actionClose.triggered.connect(self.closeEvent)
        self.ui.actionConvert.triggered.connect(self.dialogConvert)
        self.ui.actionHelp.triggered.connect(self.help)
        self.ui.actionOpen.triggered.connect(self.importSequence)
        self.ui.actionAddMask.triggered.connect(self.addMask)
        self.ui.imageSlider.valueChanged[int].connect(self.showImage)
        self.ui.actionRename.triggered.connect(self.dialogRename)
        self.ui.actionReportBug.triggered.connect(self.reportBug)
        self.ui.actionUpdate.triggered.connect(self.getUpdate)
        
        self.ui.label.customContextMenuRequested.connect(self.imageContextMenuEvent)
        
        self.ui.actionRename.setEnabled(True)
        self.ui.stack.currentChanged.connect(self.setView)
        
        # Gui customisation
        self.ui.imageSlider.hide()
        self.ui.mplwidget.hide()
        self.ui.listIVs.hide()
        #self.ui.mplwidget.figure.set_facecolor('Blue')
            
    # Import image sequence and keep in memory (may be large footprint...)
    def importSequence(self):
        """Dialog to import a sequence of images"""
        # Set file filters of importable types
        filters = ["All files (*.*)", "Bitmap (*.bmp)", "Tag Image Format (*.tif *.tiff)",
                  "JPEG (*.jpg *.jpeg)", "Portable Network Graphics (*.png)"]
        if '.fits' in SUPPORTED_FORMATS:
            filters.append("Flexible Image Transport System (*.fit *.fits)")
            
        filters.append("IV curves (*.dat *.iv *.fsm *.txt)")
            
        # Get filenames from filedialog
        fileDialog = QtGui.QFileDialog()
        filenames = fileDialog.getOpenFileNames(parent=self, 
                                                       caption='Open Sequence...',
                                                       directory=str(self.lastDirectory),
                                                       filter=";;".join(filters),
                                                       selectedFilter=self.lastImportFilter)
        self.lastImportFilter = str(fileDialog.selectedFilter())
        # File preprocessing
        total = len(filenames)
        if total < 1:
            return
        else:
            lastFile = str(filenames[total-1]) 
            self.lastDirectory = os.path.dirname(lastFile)
            
        tempImages = []
        tempIVs = []
        
        # Prepare GUI
        self.setCursor(Qt.WaitCursor)
        self.ui.statusbar.showMessage('Loading files...')
        progressbar = QtGui.QProgressBar()
        progressbar.setValue(0)
        self.ui.statusbar.addPermanentWidget(progressbar)
        
        # Loop through each file in list
        for i,file in enumerate(filenames):
            try:
                progressbar.setValue(100*i/(total-1))
                progressbar.update()
            except ZeroDivisionError:
                pass
            extension = ntpath.splitext(str(file))[1]
            if extension.lower() in SUPPORTED_FORMATS:
                #import image using PIL
                tempImages.append(ski.io.imread(str(file), as_grey=True, plugin='qt'))
            elif extension.lower() in IV_FORMATS:
                if extension.lower() in  ['*.dat', '*.txt']:
                    # assume simple dat file - maybe use numpy for this
                    tempIV = np.genfromtxt(str(file), dtype=np.float, comments='#')
                elif extension.lower() in ['*.iv', '*.fsm']:
                    # assume simple 2-column file with comments marked by hash - Held style
                    # first line comment will be used to derive spot index, else use filename
                    tempIV = np.genfromtxt(str(file), dtype=np.float, comments='#')
                tempIVs.append(tempIV)
            else:
                try:
                    tempImages.append(tempImage = ski.io.imread(str(file), as_grey=True, plugin='qt'))
                except:
                    msg = QtGui.QMessageBox.critical(self, 'Error', 'Image format is not supported for \"'
                                                     + ntpath.basename(str(file)) + '\"')
                    self.ui.statusbar.removeWidget(progressbar)
                    self.setCursor(Qt.ArrowCursor)
                    self.ui.statusbar.clearMessage()
                    return
                
        # Post import and clean up
        self.ui.statusbar.removeWidget(progressbar)
        if tempImages is not None:
            self.images = tempImages
            self.imageFiles = [str(file) for file in filenames if ntpath.splitext(str(file))[1] in SUPPORTED_FORMATS]
        if tempIVs is not None:
            self.ivs = tempIVs
            self.ivFiles = [str(file) for file in filenames if ntpath.splitext(str(file))[1] in IV_FORMATS]
            
        self.updateGui()
        self.updateImages()
        self.setCursor(Qt.ArrowCursor)
        self.ui.statusbar.showMessage('Sequence loaded', 2)
        
    # Update view
    def setView(self):
        self.view = self.ui.stack.currentIndex()
    
    
    # Show the specified image on screen
    def showImage(self, index):
        # Try to load the image assuming PIL -> QImage.QImage() method
        try:
            im = ski.img_as_ubyte(self.images[index], force_copy=True)
            image = QImage(im.data, im.shape[1], im.shape[0], QImage.Format_Indexed8)
            pixmap = QtGui.QPixmap.fromImage(image)
            scaledPixmap = QtGui.QPixmap.fromImage(image).scaled(self.label.size(), Qt.KeepAspectRatio)
            
            self.ui.label.setPixmap(scaledPixmap)

            self.ui.imageSlider.setToolTip('Image {0} of {1}'.format(str(index+1), str(len(self.images))))
            self.ui.label.setToolTip(str(self.imageFiles[index]))
            self.resizeImage()
        except IndexError:
            self.logger.error('image at index {0} does not exist'.format(str(index)))
            
        
    def resizeEvent(self, resizeEvent):    
        self.showImage(self.ui.imageSlider.value())
        
    # Resize label dimensions to fit app geometry
    def resizeImage(self, evnt=None):
        return

        
    # Report bug / email devs
    def reportBug(self):
        QtGui.QDesktopServices.openUrl(QUrl(str("mailto: {0}?subject={1} feedback&body=").format(__APP_EMAIL__,__APP_NAME__)))
               
        
    # Update class elements after loading new sequence of images
    def updateGui(self):
        if len(self.images) > 0:
            # Setup image related processes
            self.ui.actionImages.setEnabled(True)
            self.ui.actionRename.setEnabled(True)
            self.ui.actionConvert.setEnabled(True)
            self.ui.actionMarkReference.setEnabled(True)
            self.ui.actionMarkReflexes.setEnabled(True)
            self.ui.actionExtract.setEnabled(True)
            self.ui.actionCopy.setEnabled(True)
            self.ui.actionCopyRaw.setEnabled(True)
            self.ui.stack.setCurrentIndex(0)
            # Enable slider if more than multiple images
            if len(self.images) > 1:
                self.ui.imageSlider.show()
            else:
                self.ui.imageSlider.hide()
            # Show first image by default
            self.showImage(0)
        if len(self.ivs) > 0:
            # Setup IV related processes
            self.ui.actionIVs.setEnabled(True)
            self.ui.actionSmooth.setEnabled(True)
            self.ui.actionTrim.setEnabled(True)
            self.ui.actionOffset.setEnabled(True)
            self.ui.actionExportIVs.setEnabled(True)
            self.ui.stack.setCurrentIndex(1)
            
            
    def updateIVs(self):
        pass
    
    def updateImages(self):
        self.ui.imageSlider.setMinimum(0)
        self.ui.imageSlider.setValue(0)
        self.ui.imageSlider.setMaximum(len(self.images)-1)
        
        
    # Parse INI configuration to class
    def loadConfig(self):
        cfg = {'LastDir':'Recent', 'LastFilter':'Recent', 'MaskPath':'Config'}
        for key in cfg:
            try:
                if key == 'LastDir':
                    self.lastDirectory = self.config.get(cfg.get(key), key)
                elif key == 'MaskPath':
                    self.maskFilePath = self.config.get(cfg.get(key), key)
                elif key == 'LastFilter':
                    self.lastImportFilter = self.config.get(cfg.get(key), key)
            except configparser.NoSectionError:
                self.logger.warn('no section {0} in INI file'.format(cfg.get(key)))
            except configparser.NoOptionError:
                self.logger.warn('no option {0} in INI file'.format(key))
            except any as e:
                pass
        self.logger.debug('Loaded config file \"{0}\"'.format(self.configFilePath))
            
    
    # Export configuration to file
    def writeConfig(self):
        #update configuration
        for section in ['Recent', 'Config']:
            try:
                self.config.add_section(section)
            except configparser.DuplicateSectionError:
                pass
        self.config.set('Recent', 'LastDir', self.lastDirectory)
        self.config.set('Recent', 'LastFilter', self.lastImportFilter)
        self.config.set('Config', 'MaskPath', self.maskFilePath)
        
        
        #save configuration
        try:
            if not ntpath.exists(ntpath.dirname(self.configFilePath)):
                os.makedirs(ntpath.dirname(self.configFilePath), mode=0777)
            with open(self.configFilePath, 'w') as configFile:
                self.config.write(configFile)
        except any as e:
            sys.stderr.write(e)

def main():
    # Again, this is boilerplate, it's going to be the same on
    # almost every app you write
    app = QtGui.QApplication(sys.argv)
    window=Main()
    window.show()
    # It's exec_ because exec is a reserved word in Python
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()