'''

@author: Liam Deacon

@contact: liam.deacon@diamond.ac.uk

@copyright: 2014 Liam Deacon

@license: GNU General Public License 3.0

'''

# Python version compatibility
from __future__ import print_function, with_statement

# These are only needed for Python v2 but are harmless for Python v3.
import sip
sip.setapi('QString', 2)
sip.setapi('QVariant', 2)

# Import standard library modules
import logging
import os
import platform
import sys
from collections import OrderedDict

# import package modules
from interceptor import OutputInterceptor
from gui.ProjectTreeView import ProjectTreeView
from gui.mdichild import MdiChild
from gui.ExplorerItems import ProjectItem, ModelItem

from project import Project

# Import Qt modules
import PyQt4
from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtGui import (QApplication, QColor, QDesktopServices, QDialog, 
                         QFileDialog, QMainWindow, QMessageBox, QPrinter, 
                         QPrintDialog, QPixmap, QSplashScreen, QTreeWidgetItem, 
                         QTreeWidget) 
import res_rc  # note this requires compiled resource file res_rc.py
__QT_TYPE__ = 'PyQt4' 

# other modules
from settings import Settings
from ImportDialog import ImportDialog

# Define globals
__APP_AUTHOR__ = 'Liam Deacon'
__APP_COPYRIGHT__ = '\xa9' + '2013 {0}'.format(__APP_AUTHOR__)
__APP_DESCRIPTION__ = ('CLEED - Interactive Visualiser (IV) \n '
                        '- a GUI front end to the CLEED package')
__APP_DISTRIBUTION__ = 'cleed-gui'
__APP_EMAIL__ = 'liam.deacon@diamond.ac.uk'
__APP_LICENSE__ = 'GNU General Public License 3.0'
__APP_NAME__ = 'CLEED-IV'
__APP_VERSION__ = '0.1.0-dev'
__PYTHON__ = "{0}.{1}.{2}".format(sys.version_info.major,
                                         sys.version_info.minor,
                                         sys.version_info.micro, 
                                         sys.version_info.releaselevel)
__UPDATE_URL__ = ""

__DEBUG__ = True

# Platform specific setup
if platform.system() is 'Windows':
    from ctypes import windll
    # Tell Windows Python is merely hosting the application (taskbar icon fix)
    windll.shell32.SetCurrentProcessExplicitAppUserModelID(__APP_NAME__)
        
#==============================================================================
# BEGIN GUI WIDGET PROGRAMMING
#==============================================================================


class MainWindow(QMainWindow):
    '''Class for main application widget'''
    
    maxRecentFiles = 10
    lastDirectory = QtGui.QDesktopServices.HomeLocation
    models = []
    
    class StreamProxy(QtCore.QObject):
        # only the GUI thread is allowed to write messages in the
        # LoggerWindow, so this class acts as a proxy, passing messages
        # over Qt signal/slot for thread safety
        write_text = QtCore.pyqtSignal(str)
    
        def write(self, msg):
            msg = msg.strip()
            if msg:
                self.write_text.emit(msg)

        flush_text = QtCore.pyqtSignal()
        
        def flush(self):
            self.flush_text.emit()
    
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)
        
        # dynamically load ui
        uiFile = "MDIMainWindow.ui"  # change to desired relative ui file path
        if not os.path.isfile(uiFile):
            uiFile = os.path.join('gui', "MDIMainWindow.ui")
            
        self.ui = QMainWindow()
        self.ui = uic.loadUi(uiFile, self) 
        
        self.init()
        self.initUi()
        
        self.ui.show()

    def shutdown(self):
        logger = logging.getLogger(__APP_NAME__)
        for handler in logger.handlers:
            logger.removeHandler(handler)

    @QtCore.pyqtSlot(str)
    def write(self, msg):
        if 'ERROR' in msg:
            color = QColor('Red')
        elif 'WARNING' in msg:
            color = QColor('Orange')
        elif 'INFO' in msg:
            color = QColor('Blue')
        else:
            color = QColor('Black')
        self.ui.textEditLog.setTextColor(color)
        self.ui.textEditLog.append(msg)
        self.ui.textEditLog.setTextColor(QColor('Black'))
        
    @QtCore.pyqtSlot()
    def flush(self):
        if self.isHidden():
            self.show()
        self.raise_()

    # initialise class
    def init(self, verbose=True):
        '''initialise logging and objects before gui''' 

        ######################################
        # APP LOGGING
        ######################################
        
        # create logger with 'spam_application'
        self.logger = logging.getLogger(__APP_NAME__)
        self.logger.setLevel(logging.DEBUG)
        
        # create file handler which logs all messages
        fh = logging.FileHandler(os.path.join(os.environ['TEMP'], __APP_NAME__ 
                + str('.log')))  # temp directory is emptied on system reboot
        formatter = logging.Formatter(
                        '%(asctime)s - %(name)s - %(levelname)s - %(message)s',
                         "%Y-%m-%d %H:%M:%S")
        fh.setFormatter(formatter)
        fh.setLevel(logging.INFO)  # change to taste
        
        # create console handler with a higher log level
        ch = logging.StreamHandler()
        ch.setLevel(logging.WARNING)
        ch.setFormatter(formatter)
        
        # add the handlers to the logger
        self.logger.addHandler(ch)
        self.logger.addHandler(fh)
        
        if __DEBUG__:
            pwd = os.path.dirname(__file__)
            os.system('pyrcc4 %s -o %s' % (
                                       os.path.join(pwd, 'res', 'res.qrc'),
                                       os.path.join(pwd, 'res_rc.py')))
        
        # create proxy stream layer
        stream_proxy = self.StreamProxy(self)
        stream_proxy.write_text.connect(self.write)
        stream_proxy.flush_text.connect(self.flush)
        handler = logging.StreamHandler(stream_proxy)
        handler.setFormatter(formatter)
        self.logger.addHandler(handler)
        
        # intercept stdout and stderr
        sys.stderr = OutputInterceptor('stderr', sys.stderr)
        sys.stdout = OutputInterceptor('stdout', sys.stdout)
        
        # other variables
        self.projects = []
        
    # initialise UI
    def initUi(self):
        '''Class to initialise the Qt Widget and setup slots and signals'''

        # recent files
        self.recentFileActions = []

        # Setup MDI area
        self.mdiArea = QtGui.QMdiArea()
        self.mdiArea.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAsNeeded)
        self.mdiArea.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAsNeeded)
        self.setCentralWidget(self.mdiArea)

        self.mdiArea.subWindowActivated.connect(self.updateMenus)
        self.windowMapper = QtCore.QSignalMapper(self)
        self.windowMapper.mapped[QtGui.QWidget].connect(
                                                    self.setActiveSubWindow)
        
        # setup docks
        self.ui.treeWidgetFiles.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.ui.treeWidgetFiles.customContextMenuRequested.connect(
                                                    self.explorerPopupMenu) 

        # Setup 
        self.createActions()
        self.createMenus()
        self.createToolBars()
        self.createTrayIcon()
        self.createStatusBar()
        self.updateMenus()

        self.readSettings()

        self.setUnifiedTitleAndToolBarOnMac(True)

        # ============
        # Setup slots
        # ============
        
        # file actions
        self.ui.exitAction.triggered.connect(self.close)
        self.ui.exportXYZAction.triggered.connect(self.todo)
        self.ui.exportIVCurvesAction.triggered.connect(self.todo)
        self.ui.importAction.triggered.connect(self.importModel)
        self.ui.newAction.triggered.connect(self.newDialog)
        self.ui.openAction.triggered.connect(self.open)
        self.ui.printAction.triggered.connect(self.printer)
        for i in range(MainWindow.maxRecentFiles):
            self.recentFileActions.append(
                          QtGui.QAction(self, visible=False,
                                        triggered=self.openRecentFile))
        self.ui.restoreSessionAction.triggered.connect(self.restoreState)
        self.ui.saveAction.triggered.connect(self.save)
        self.ui.saveAllAction.triggered.connect(self.saveAll)
        self.ui.saveAsAction.triggered.connect(self.saveAs)
        self.ui.saveSessionAction.triggered.connect(self.saveState)
        self.ui.settingsAction.triggered.connect(self.settingsDialog)
        
        # edit actions
        self.ui.copyAction.triggered.connect(self.todo)
        self.ui.cutAction.triggered.connect(self.todo)
        self.ui.pasteAction.triggered.connect(self.todo)
        self.ui.redoAction.triggered.connect(self.todo)
        self.ui.selectAllAction.triggered.connect(self.todo)
        self.ui.undoAction.triggered.connect(self.todo)
        
        # view actions
        self.ui.hideAllAction.triggered.connect(self.todo)
        self.ui.showAllAction.triggered.connect(self.todo)
        self.ui.showExplorerAction.triggered.connect(self.todo)
        self.ui.showPropertiesAction.triggered.connect(self.todo)
        self.ui.showLogAction.triggered.connect(self.todo)
        
        # process actions
        
        # window actions
        self.ui.minimiseAction.triggered.connect(self.todo)
        
        # help actions
        self.ui.aboutAction.triggered.connect(self.about)
        self.ui.aboutQtAction.triggered.connect(self.aboutQt)
        self.ui.contactAction.triggered.connect(self.contactDeveloper)
        self.ui.helpAction.triggered.connect(self.help)
        self.ui.helpCleedAction.triggered.connect(self.cleedHelp)
        self.ui.updateAction.triggered.connect(self.getUpdate)
        self.ui.websiteAction.triggered.connect(self.visitWebsite)
        
        # explorer dock
        
        # log dock
         
        # properties dock
        
        # main widget
        
        # systray
        self.trayIcon.messageClicked.connect(self.trayMessageClicked)
        self.trayIcon.activated.connect(self.trayIconActivated)
    
    # Show about dialog
    def about(self):
        """Display 'About' dialog"""
        text = __APP_DESCRIPTION__
        text += '\n\nAuthor: {0} \nEmail: {1}'.format(__APP_AUTHOR__, 
                                                      __APP_EMAIL__)
        text += '\n\nApp version: {0}'.format(__APP_VERSION__)
        text += '\n{0}'.format(__APP_COPYRIGHT__)
        text += '\n' + __APP_LICENSE__
        text += '\n\nPython: {0}'.format(__PYTHON__)
        text += '\nGUI frontend: {0} {1}'.format(__QT_TYPE__, 
                                                 QtCore.QT_VERSION_STR)

        QMessageBox.about(self, self.ui.windowTitle(), text)
    
    # Display about dialog
    def aboutQt(self):
        """Display Qt dialog"""
        QApplication.aboutQt()
    
    def activeMdiChild(self):
        '''return handle to active MDI child window'''
        activeSubWindow = self.mdiArea.activeSubWindow()
        if activeSubWindow:
            return activeSubWindow.widget()
        return None
    
    def cleedHelp(self):
        '''display CLEED help in browser'''
        help_file = str((os.path.abspath(os.path.join(
                        '.', 'gui', 'res', 'help', 'html', 'index.html'))))
        if not QDesktopServices.openUrl(
                            QtCore.QUrl.fromLocalFile(help_file)):
            self.logger.error(
                "Could not open CLEED help file '%s' in browser" % help_file)

    def closeEvent(self, event):
        '''override closeEvent'''
        if not self.trayIcon.isVisible():
            self.hide()
            self.trayIcon.show()
            self.trayIcon.showMessage("CLEED-IV",
                    "The program will keep running in the system tray. To "
                    "terminate the program, choose 'Quit' in the "
                    "context menu of the system tray entry.",
                    self.trayIcon.Information, 25000)
            event.ignore()
        else:
            self.mdiArea.closeAllSubWindows()
            if self.mdiArea.currentSubWindow():
                event.ignore()
            else:
                self.writeSettings()
                event.accept()
                sys.exit(0)  # force app to exit

    # Report bug / email devs
    def contactDeveloper(self):
        '''open email client with email to developer''' 
        QDesktopServices.openUrl(QtCore.QUrl(
                str("mailto: {email}?subject={name} feedback&body=").format(
                            email=__APP_EMAIL__, name=__APP_NAME__)))

    def copy(self):
        '''use MDI child copy method'''
        if self.activeMdiChild():
            self.activeMdiChild().copy()
        else:
            self.todo()

    def createMdiChild(self):
        '''create new MDI child'''
        child = MdiChild()
        self.mdiArea.addSubWindow(child)

        child.copyAvailable.connect(self.cutAction.setEnabled)
        child.copyAvailable.connect(self.copyAction.setEnabled)

        return child

    def createActions(self):
        # generic actions
        self.renameAction = QtGui.QAction(
                                      QtGui.QIcon(":/tag_stroke.svg"),
                                      "Re&name", self,
                                      triggered=self.rename)
        
        self.refreshAction = QtGui.QAction(
                                      QtGui.QIcon(":/reload.svg"),
                                      "&Refresh", self,
                                      triggered=self.refresh,
                                      shortcut="F5")
        
        # explorer actions        
        self.newProjectAction = QtGui.QAction(
                                      QtGui.QIcon(":/document_alt_stroke.svg"),
                                      "&New Project", self,
                                      triggered=self.newProject)
        self.newProjectAction.setToolTip("Create new project...")
        
        self.importProjectAction = QtGui.QAction(QtGui.QIcon(":/import.svg"),
                                "&Import Project", self,
                                triggered=self.importProject)
        self.importProjectAction.setToolTip("Import existing project...")    
        
        self.newModelAction = QtGui.QAction(
                                      QtGui.QIcon(":/atoms.svg"),
                                      "&New Model", self,
                                      triggered=self.newModel)
        self.newModelAction.setToolTip("Create new model...")
        
        self.importModelAction = QtGui.QAction(QtGui.QIcon(":/import.svg"),
                                "&Import Model", self,
                                triggered=self.importModel)
        self.importModelAction.setToolTip("Import existing model...")      
        
        # system tray
        self.killAllAction = QtGui.QAction(QtGui.QIcon(':/x.svg'),
                                "&Kill all processes", self, 
                                triggered=self.hide)
        
        self.minimizeAction = QtGui.QAction(QtGui.QIcon(':/minus.svg'),
                                    "Mi&nimize", self, triggered=self.hide)
        
        self.maximizeAction = QtGui.QAction(QtGui.QIcon(':/new_window.svg'),
                                            "Ma&ximize", self, 
                                            triggered=self.showMaximized)
        
        self.restoreAction = QtGui.QAction(QtGui.QIcon(':/reload.svg'),
                                           "&Restore", self,
                                           triggered=self.showNormal) 

        # MDI windows
        self.closeMdiAction = QtGui.QAction(QtGui.QIcon(":/denied.svg"),
                             "&Close", self, 
                             statusTip="Close active MDI window",
                             triggered=self.mdiArea.closeActiveSubWindow)

        self.closeAllMdiAction = QtGui.QAction(QtGui.QIcon(":/denied.svg"),
                             "&Close All", self, 
                             statusTip="Close active MDI window",
                             triggered=self.mdiArea.closeAllSubWindows)                             
        
        self.tileMdiAction = QtGui.QAction(
                         "&Tile", self, statusTip="Tile the windows",
                         triggered=self.mdiArea.tileSubWindows)

        self.cascadeMdiAction = QtGui.QAction("&Cascade", self,
                statusTip="Cascade the windows",
                triggered=self.mdiArea.cascadeSubWindows)

        self.nextMdiAction = QtGui.QAction(QtGui.QIcon(":/arrow_right.svg"),
                            "Ne&xt", self,
                            shortcut=QtGui.QKeySequence.NextChild,
                            statusTip="Move the focus to the next window",
                            triggered=self.mdiArea.activateNextSubWindow)

        self.previousMdiAction = QtGui.QAction(QtGui.QIcon(":/arrow_left.svg"),
                            "Pre&vious", self,
                            shortcut=QtGui.QKeySequence.PreviousChild,
                            statusTip="Move the focus to the previous window",
                            triggered=self.mdiArea.activatePreviousSubWindow)
        
        self.separatorMdiAction = QtGui.QAction(self)
        self.separatorMdiAction.setSeparator(True)

    def createMenus(self):
        '''create additional menus not in ui file or alter existing'''
        self.updateWindowMenu()
        self.windowMenu.aboutToShow.connect(self.updateWindowMenu)

        self.menuBar().addSeparator()
        
        # recent files
        for i in range(MainWindow.maxRecentFiles):
            try:
                self.recentFilesMenu.addAction(self.recentFileActions[i])
            except IndexError:
                pass
        
        # add icons to menus
        self.recentFilesMenu.setIcon(QtGui.QIcon(":/clock.svg"))
        self.exportMenu.setIcon(QtGui.QIcon(":/export.svg"))
        self.sessionMenu.setIcon(QtGui.QIcon(":/session.svg"))
        self.scriptingMenu.setIcon(QtGui.QIcon(":/cog.svg"))
        
        # explorer menus
        self.explorerDefaultMenu = QtGui.QMenu()
        self.explorerDefaultMenu.addAction(self.newProjectAction)
        self.explorerDefaultMenu.addAction(self.importProjectAction)
        self.explorerDefaultMenu.addSeparator()
        self.explorerDefaultMenu.addAction(self.copyAction)
        self.explorerDefaultMenu.addAction(self.cutAction)
        self.explorerDefaultMenu.addAction(self.pasteAction)
        #self.explorerDefaultMenu.addAction(self.renameAction)
        self.explorerDefaultMenu.addSeparator()
        self.explorerDefaultMenu.addAction(self.refreshAction)
        
        self.explorerProjectMenu = QtGui.QMenu()
        self.explorerProjectMenu.addAction(self.newModelAction)
        self.explorerProjectMenu.addAction(self.importModelAction)
        self.explorerProjectMenu.addSeparator()
        self.explorerProjectMenu.addAction(self.copyAction)
        self.explorerProjectMenu.addAction(self.cutAction)
        self.explorerProjectMenu.addAction(self.pasteAction)
        self.explorerProjectMenu.addAction(self.renameAction)
        self.explorerProjectMenu.addSeparator()
        self.explorerProjectMenu.addAction(self.refreshAction)
        
        self.explorerFileMenu = QtGui.QMenu()
        self.explorerFileMenu.addAction(self.newAction)
        self.explorerFileMenu.addAction(self.refreshAction)
        
    def createToolBars(self):
        '''create custom toolbars here'''
        pass

    def createTrayIcon(self):
        '''create system tray icon'''
        self.trayIconMenu = QtGui.QMenu(self)
        self.trayIconMenu.addAction(self.minimizeAction)
        self.trayIconMenu.addAction(self.maximizeAction)
        self.trayIconMenu.addAction(self.restoreAction)
        self.trayIconMenu.addSeparator()
        self.trayIconMenu.addAction(self.exitAction)

        self.trayIcon = QtGui.QSystemTrayIcon(self)
        self.trayIcon.setContextMenu(self.trayIconMenu)
         
        icon = self.windowIcon()
        self.trayIcon.setIcon(icon)
        self.setWindowIcon(icon)
        self.trayIcon.setToolTip("CLEED-IV")
        self.trayIcon.setVisible(False)

    def createStatusBar(self):
        '''create custom status bar'''
        statusBar = QtGui.QStatusBar()
        statusBar = self.statusBar()
        statusBar.showMessage("Ready")
        #statusBar.addWidget(QtGui.QLabel("test"), 1)
        #=======================================================================
        # m_statusLeft = QtGui.QLabel("Left", self)
        # m_statusLeft.setFrameStyle(QtGui.QFrame.Panel | QtGui.QFrame.Sunken)
        # m_statusMiddle = QtGui.QLabel("Middle", self)
        # m_statusMiddle.setFrameStyle(QtGui.QFrame.Panel | QtGui.QFrame.Sunken)
        # m_statusRight = QtGui.QLabel("Right", self)
        # m_statusRight.setFrameStyle(QtGui.QFrame.Panel | QtGui.QFrame.Sunken)
        # statusBar.addPermanentWidget(m_statusLeft, 1)
        # statusBar.addPermanentWidget(m_statusMiddle, 1)
        # statusBar.addPermanentWidget(m_statusRight, 2)
        #=======================================================================
    
    def cut(self):
        '''use MDI child cut method'''
        if self.activeMdiChild():
            self.activeMdiChild().cut()
        else:
            self.todo()

    def explorerPopupMenu(self, point):
        '''popup menu for explorer widget'''
        index = self.ui.treeWidgetFiles.indexAt(point)
        if index.isValid():
            # show custom menu for file type held at given index
            item = self.ui.treeWidgetFiles.itemFromIndex(index)
            if self.ui.treeWidgetFiles.indexOfTopLevelItem(item) > -1:
                # then its a top-level item
                self.ui.treeWidgetFiles.selectionModel().setCurrentIndex(index, 
                                            QtGui.QItemSelectionModel.NoUpdate)
                self.explorerProjectMenu.popup(
                        self.ui.treeWidgetFiles.viewport().mapToGlobal(point))
            else:
                self.explorerFileMenu.popup(
                        self.ui.treeWidgetFiles.viewport().mapToGlobal(point))
                print('another item: %s' % item.text(0))
        else:
            # provide default menu
            self.explorerDefaultMenu.popup(
                        self.ui.treeWidgetFiles.viewport().mapToGlobal(point))
            
    def findMdiChild(self, fileName):
        '''find mdi child'''
        canonicalFilePath = QtCore.QFileInfo(fileName).canonicalFilePath()

        for window in self.mdiArea.subWindowList():
            if window.widget().currentFile() == canonicalFilePath:
                return window
        return None
    
    # check for update
    def getUpdate(self):
        """Check for app updates"""
        from UpdateDialog import UpdateDialog
        updateDialog = UpdateDialog(parent=self)
        updateDialog.setAttribute(QtCore.Qt.WA_DeleteOnClose)
        updateDialog.exec_()
    
    # export model as text file
    def exportModel(self):
        '''Export model as text file'''
        pass
    
    def importDialog(self):
        '''Open dialog and radio options'''
        importDialog = ImportDialog(parent=self, 
                            model=str(self.ui.tabWidget.tabText(
                                    self.ui.tabWidget.currentIndex())).lower())
        importDialog.setAttribute(QtCore.Qt.WA_DeleteOnClose)
        importDialog.finished.connect(self.parseInput)
        importDialog.exec_()
    
    # import model from text file
    def importModel(self):
        '''Import model from text file'''
        pass
    
    def importProject(self):
        '''Import a project'''
        project = QtGui.QFileDialog.getExistingDirectory(parent=self, 
                            caption="Select CLEED project directory...")
        if os.path.isdir(project) and not project in self.projects:
            self.projects.append(project)
            
         #"Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)"
    
    def loadFile(self, fileName):
        file = QtCore.QFile(fileName)
        if not file.open(QtCore.QFile.ReadOnly | QtCore.QFile.Text):
            QtGui.QMessageBox.warning(self, "Recent Files",
                    "Cannot read file %s:\n%s." % (fileName, 
                                                   file.errorString()))
            return

        instr = QtCore.QTextStream(file)
        QtGui.QApplication.setOverrideCursor(QtCore.Qt.WaitCursor)
        self.textEdit.setPlainText(instr.readAll())
        QtGui.QApplication.restoreOverrideCursor()

        self.setCurrentFile(fileName)
        self.statusBar().showMessage("Loaded: %s" % fileName, 2000)
        
    def help(self):
        """Display help"""
        help_file = str((os.path.abspath(os.path.join(
                        '.', 'gui', 'res', 'help', 'html', 'index.html'))))
        if not QDesktopServices.openUrl(
                            QtCore.QUrl.fromLocalFile(help_file)):
            self.logger.error(
                    "Could not open help file '%s' in browser" % help_file)

    def getInputFile(self, startpath=str(
                             QDesktopServices.storageLocation(
                                  QDesktopServices.HomeLocation)), model=None):
        '''returns file path of input'''
        if model == None:
            model = ''
        else:
            model += ' '
        
        model = model.capitalize()
        
        # start at last known directory
        if os.path.exists(self.lastpath):
            if os.path.isfile(self.lastpath):
                startpath = os.path.dirname(self.lastpath)
            else:
                startpath = self.lastpath
        
        filepath = str(QFileDialog.getOpenFileName(parent=None, 
                     caption='Open %sInput File' % model, directory=startpath))
        
        return filepath

    def openCLEEDManual(self):
        '''open pdf manual'''
        manual = os.path.abspath(os.path.join(
                    '.', 'gui', 'help', 'pdf', 'CLEED_manual.pdf'))
        if sys.platform.startswith('darwin'):
            success = os.system(manual)
        elif sys.platform.startswith('linux'):
            success = os.system(manual)
        elif sys.platform.startswith('win32'):
            success = os.system('start ' + manual)
            
        if not success:
            self.logger.error("Could not open pdf manual: '%s'" % manual)
    
    def newDialog(self, filename=None):
        '''Open New file dialog'''
        self.newModel(filename)
        #self.newFile()
        #self.newInputFile(filename)
        
    def newInputFile(self, filename=None):
        '''create new MDI child window of *.inp'''
        pass
    
    def newGraphFile(self, filename=None):
        pass
    
    def newPyMolFile(self, filename=None):
        child = self.createPyMolMdiChild()
        child.newFile()
        child.show()
    
    def newFile(self, filename=None):
        child = self.createMdiChild()
        child.newFile()
        child.show()

    def newProject(self, projectName=None):
        if not projectName:
            projectName = "Untitle_Project"
        
        # get storage location for project
        homePath = QtGui.QDesktopServices.storageLocation(
                                        QtGui.QDesktopServices.HomeLocation)
        projectDir = os.path.join(homePath, "CLEED", "models")
        folder = QFileDialog.getExistingDirectory(parent=self, 
                            caption="Select Project Base Directory",
                              directory=projectDir, 
                              options=QFileDialog.ShowDirsOnly | 
                                      QFileDialog.DontResolveSymlinks)    
        if folder:
            # do stuff
            items = [self.treeWidgetFiles.topLevelItem(i).Path for i 
                     in range(self.treeWidgetFiles.topLevelItemCount())]
            if folder not in items:
                proj = ProjectItem(self.ui.treeWidgetFiles, path=folder)
            else:
                pass
                self.treeWidgetFiles.setCurrentIndex(0, items.index(folder, ))

    def newModel(self, project, modelName=None):
        if not modelName:
            text, ok = QtGui.QInputDialog.getText(self, 'Input Dialog', 
                                              'Enter model name:')
            if not ok:
                return
            
            modelName = text
        
        try:
            index = self.ui.treeWidgetFiles.selectedIndexes()[0]
            parent = self.ui.treeWidgetFiles.itemFromIndex(index)
            path = os.path.join(parent.Path, modelName)
             
            if not modelName:
                modelName = "New_Model"
                i = 1
                path = os.path.join(parent.Path, modelName)
                while os.path.isdir(modelName):
                    modelName = "New_Model%i" % i
                    path = os.path.join(parent.Path, modelName)
                    i += 1
            
            model = ModelItem(path)
            a = parent.addChild(model)
            print(a)    
            if not os.path.exists(path):
                os.makedirs(path, 755)
                # add new input files
                
            else:
                pass
        
        except IndexError:
            # no index selected (or created?)
            pass    
        
        

    def open(self):
        fileName = QtGui.QFileDialog.getOpenFileName(self)
        if fileName:
            existing = self.findMdiChild(fileName)
            if existing:
                self.mdiArea.setActiveSubWindow(existing)
                return

            child = self.createMdiChild()
            if child.loadFile(fileName):
                self.statusBar().showMessage("File loaded", 2000)
                child.show()
            else:
                child.close()
    
    def openRecentFile(self):
        action = self.sender()
        if action:
            self.loadFile(action.data())
    
    def paste(self):
        '''use MDI child paste method'''
        if self.activeMdiChild():
            self.activeMdiChild().paste()
        else:
            self.todo()
    
    # check type of input file
    def parseInput(self):
        if isinstance(self.sender(), ImportDialog):
            # check user did not abort
            if self.sender().action == 'cancel':
                print('cancel') 
                return
            
            # determine file type
            if self.sender().ui.radioBulk.isChecked():
                model = 'bulk'
            else:
                model = 'slab'
        
        else:  # guess from active tab
            tabText = str(self.ui.tabWidget.tabText(
                                self.ui.tabWidget.currentIndex())).lower()
            if tabText == 'bulk' or tabText == 'slab':
                model = tabText
            else:  # unknown
                return self.importDialog()  # start dialog
        
        filename = self.getInputFile(model=model)

        if not os.path.exists(filename):
            return  # user aborted
        else:
            self.lastpath = filename
        
        try:
            pass
            
        except IOError:
            self.logger.error("IOError: Unable to open input file '%s'" 
                              % filename)
    
    def printer(self):
        '''prints MDI window'''
        if self.activeMdiChild():
            self.activeMdiChild().printer()
        else:
            self.todo()

    def printPreview(self):
        if self.activeMdiChild():
            self.activeMdiChild().printPreview()
        else:
            self.todo()
    
    def readStdOutput(self):
        self.ui.textEditLog.append(str(self.readAllStandardOutput()))
    
    def rename(self):
        self.todo()
        
    def refresh(self):
        self.todo()
    
    def save(self):
        '''save current window'''
        if self.activeMdiChild() and self.activeMdiChild().save():
            msg = "Saved file: '%s'" % self.activeMdiChild()
            self.statusBar().showMessage(msg, 2000)
            self.logger.info(msg)
        
    def saveAs(self):
        '''open saveAs dialog of MDI child''' 
        if self.activeMdiChild() and self.activeMdiChild().saveAs():
            msg = "Saved file: '%s'" % self.activeMdiChild()
            self.statusBar().showMessage(msg, 2000)
            self.logger.info(msg)
        
    def saveAll(self):
        '''save all objects'''
        if len(self.mdiArea.subWindowList()) < 1:
            self.logger.warning("SaveAll aborted: No child windows")
            return
        for window in self.mdiArea.subWindowList():
            if not window.save():
                msg = "Failed to save file: '%s'" % window
                self.logger.error(msg)

    def setActiveSubWindow(self, window):
        '''set active MDI child'''
        if window:
            self.mdiArea.setActiveSubWindow(window)

    def setCurrentFile(self, fileName):
        '''update recent files to current file'''
        self.curFile = fileName
        if self.curFile:
            self.setWindowTitle("%s - Recent Files" 
                                % self.strippedName(self.curFile))
        else:
            self.setWindowTitle("Recent Files")

        settings = QtCore.QSettings(__APP_DISTRIBUTION__, __APP_NAME__)
        files = settings.value('recentFileList')

        try:
            files.remove(fileName)
        except ValueError:
            pass

        files.insert(0, fileName)
        del files[MainWindow.MaxRecentFiles:]

        settings.setValue('recentFileList', files)

        for widget in QtGui.QApplication.topLevelWidgets():
            if isinstance(widget, MainWindow):
                widget.updateRecentFileActions()

    def setVisible(self, visible):
        self.minimizeAction.setEnabled(visible)
        self.maximizeAction.setEnabled(not self.isMaximized())
        self.restoreAction.setEnabled(self.isMaximized() or not visible)
        super(MainWindow, self).setVisible(visible)

    def settingsDialog(self):
        """Launch settings dialog"""
        from SettingsDialog import SettingsDialog
        settingsDialog = SettingsDialog(parent=self)
        settingsDialog.setAttribute(QtCore.Qt.WA_DeleteOnClose)
        settingsDialog.finished.connect(self.updateSettings)
        settingsDialog.exec_()

    def showTrayMessage(self, msg, 
                        icon=QtGui.QSystemTrayIcon.Information, msecs=5000):
        '''display system tray message'''
        self.trayIcon.showMessage(__APP_NAME__, msg, icon, msecs)
        
    def strippedName(self, fullFileName):
        '''return filename without path using QFileInfo'''
        return QtCore.QFileInfo(fullFileName).fileName()
        
    def todo(self):
        '''dummy function if not implemented'''
        self.logger.warning("'%s' is not implemented yet" % str(
                                                self.sender().objectName()))
   
    def trayIconActivated(self, reason):
        '''systray icon activated'''
        if reason == QtGui.QSystemTrayIcon.Trigger:
            self.logger.info("Systray icon triggered")
        elif reason == QtGui.QSystemTrayIcon.DoubleClick:
            self.setVisible(True)
            self.setWindowState(QtCore.Qt.WindowMaximized)
        elif reason == QtGui.QSystemTrayIcon.MiddleClick:
            self.logger.info("Systray icon activated by middle click")
   
    def trayMessageClicked(self):
        self.logger.debug("Tray message clicked")
    
    def readSettings(self):
        settings = QtCore.QSettings(__APP_DISTRIBUTION__, __APP_NAME__)
        
        # window geometry settings
        pos = settings.value("mainwindow/position", QtCore.QPoint(200, 200))
        size = settings.value("mainwindow/size", QtCore.QSize(400, 400))
        max = settings.value("mainwindow/maximized", False)
        self.move(pos)
        self.resize(size)
        if max:
            self.showMaximized()
            
        # dock widgets
        area = settings.value("docks/properties/area", 
                              QtCore.Qt.RightDockWidgetArea)
        geometry = settings.value("docks/properties/geometry", 
                             self.dockWidgetProperties.geometry())
        self.addDockWidget(area, self.dockWidgetProperties)
        self.dockWidgetProperties.setGeometry(geometry)
            
        if settings.value("docks/properties/visible", True):
            self.dockWidgetProperties.show()
        else:
            self.dockWidgetProperties.hide()
            
        if settings.value("projectsDockVisible", True):
            self.dockWidgetProjects.show()
        else:
            self.dockWidgetProjects.hide()
            
        if settings.value("logDockVisible", True):
            self.dockWidgetLog.show()
        else:
            self.dockWidgetLog.hide()
        
        # print all saved settings
        for i in settings.allKeys():
            pass
            #print(i, settings.value(i))
    
    def writeSettings(self):
        settings = QtCore.QSettings(__APP_DISTRIBUTION__, __APP_NAME__)
        
        # window geometry settings
        settings.setValue("mainwindow/position", self.pos())
        settings.setValue("mainwindow/size", self.size())
        settings.setValue("mainwindow/maximized", self.isMaximized())
        
        # dock widgets
        settings.setValue("docks/properties/visible",  
                          self.dockWidgetProperties.isVisible())
        settings.setValue("docks/properties/position", 
                          self.dockWidgetProperties.pos())
        settings.setValue("docks/properties/size", 
                          self.dockWidgetProperties.size())
        settings.setValue("docks/properties/floating",
                          self.dockWidgetProperties.isFloating())
        settings.setValue("docks/properties/area", 
                          self.dockWidgetArea(self.dockWidgetProperties))
        settings.setValue("docks/properties/geometry",
                          self.dockWidgetProperties.geometry())
        
        settings.setValue('projectsDockVisible', 
                          self.dockWidgetProjects.isVisible())
        settings.setValue('logDockVisible', 
                          self.dockWidgetLog.isVisible())
        
    def resetSettings(self):
        settings.setValue("propertiesDockVisible", False) 
        settings.setValue("projectsDockVisible", False) 
        settings.setValue("logDockVisible", False) 
    
    def updateModelUi(self, model=None):
        """Update model in gui""" 
        pass
    
    def updateRecentFileActions(self):
        '''update recent file list'''
        settings = QtCore.QSettings(__APP_DISTRIBUTION__, __APP_NAME__)
        files = settings.value('recentFileList')

        numRecentFiles = min(len(files), MainWindow.maxRecentFiles)

        for i in range(numRecentFiles):
            text = "&%d %s" % (i + 1, self.strippedName(files[i]))
            self.recentFileActions[i].setText(text)
            self.recentFileActions[i].setData(files[i])
            self.recentFileActions[i].setVisible(True)

        for j in range(numRecentFiles, MainWindow.maxRecentFiles):
            self.recentFileActions[j].setVisible(False)

        self.separatorAct.setVisible((numRecentFiles > 0))
    
    def updateMenus(self):
        hasMdiChild = (self.activeMdiChild() is not None)
        self.saveAction.setEnabled(hasMdiChild)
        self.saveAsAction.setEnabled(hasMdiChild)
        self.pasteAction.setEnabled(hasMdiChild)
        self.closeMdiAction.setEnabled(hasMdiChild)
        self.closeAllMdiAction.setEnabled(hasMdiChild)
        self.tileMdiAction.setEnabled(hasMdiChild)
        self.cascadeMdiAction.setEnabled(hasMdiChild)
        self.nextMdiAction.setEnabled(hasMdiChild)
        self.previousMdiAction.setEnabled(hasMdiChild)
        self.separatorMdiAction.setVisible(hasMdiChild)

        hasSelection = (self.activeMdiChild() is not None and
                        self.activeMdiChild().textCursor().hasSelection())
        self.cutAction.setEnabled(hasSelection)
        self.copyAction.setEnabled(hasSelection)

    def updateWindowMenu(self):
        self.windowMenu.clear()
        self.windowMenu.addAction(self.closeMdiAction)
        self.windowMenu.addAction(self.closeAllMdiAction)
        self.windowMenu.addSeparator()
        self.windowMenu.addAction(self.tileMdiAction)
        self.windowMenu.addAction(self.cascadeMdiAction)
        self.windowMenu.addSeparator()
        self.windowMenu.addAction(self.nextMdiAction)
        self.windowMenu.addAction(self.previousMdiAction)
        self.windowMenu.addAction(self.separatorMdiAction)

        windows = self.mdiArea.subWindowList()
        self.separatorMdiAction.setVisible(len(windows) != 0)

        for i, window in enumerate(windows):
            child = window.widget()

            text = "%d %s" % (i + 1, child.userFriendlyCurrentFile())
            if i < 9:
                text = '&' + text

            action = self.windowMenu.addAction(text)
            action.setCheckable(True)
            action.setChecked(child is self.activeMdiChild())
            action.triggered.connect(self.windowMapper.map)
            self.windowMapper.setMapping(action, window)
    
    def updateSettings(self):
        '''update the application settings'''
        self.settings = self.sender().settings
        print(self.settings.__dict__)
        
    def visitWebsite(self):
        '''open link to CLEED website'''
        self.todo()

    def getChildItemsDict(self, obj):
        try:
            if isinstance(obj, QTreeWidget):
                root = obj.invisibleRootItem()
            elif isinstance(obj, QTreeWidgetItem):
                root = obj
            child_count = root.childCount()
            topLevelDict = {}
            for i in range(child_count):
                item = root.child(i)
                var = str(item.text(0))
                exec('%s = i' % var)
                topLevelDict.update({var: eval(var)})
            return topLevelDict
        except any as e:
            self.logger.error(e.msg)
            
    def getChildItemHandle(self, obj, name=str):
        if isinstance(obj, QTreeWidget):
            root = obj.invisibleRootItem()
        elif isinstance(obj, QTreeWidgetItem):
            root = obj
        
        if isinstance(name, int):
            return root.child(name)
        elif isinstance(name, str):
            for i in range(root.childCount()):
                item = root.child(i)
                if str(item.text(0)) == name:
                    return item 
            
            
# boilerplate function - should be applicable to most applications
def main(argv=None):
    '''Entry point if executing as standalone''' 
    if argv is None:
        argv = sys.argv
    
    app = QApplication(argv)
    app.processEvents()
    pixmap = QPixmap(os.path.abspath(
                               os.path.join("res", "CLEED_logo.png")))
    splash = QSplashScreen(pixmap, QtCore.Qt.WindowStaysOnTopHint)
    splash.setMask(pixmap.mask())  # this is useful if splash isn't a rectangle
    splash.showMessage((u'Starting %s...' % __APP_NAME__), 
                       QtCore.Qt.AlignRight | QtCore.Qt.AlignBottom, 
                       QtCore.Qt.yellow)
    splash.show()

    # make sure Qt really display the splash screen 
    app.processEvents()

    app.setQuitOnLastWindowClosed(False)
    
    window = MainWindow()
    
    if not QtGui.QSystemTrayIcon.isSystemTrayAvailable():
        window.logger.warning("Unable to create a Systray on this system")
    
    # now kill the splash screen
    splash.finish(window)
    splash.close()
    
    window.show()
    sys.exit(app.exec_())

# Execute main function if running as standalone module
if __name__ == '__main__':
    main()
