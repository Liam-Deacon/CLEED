##############################################################################
# Author: Liam Deacon                                                        #
#                                                                            #
# Contact: liam.deacon@diamond.ac.uk                                         #
#                                                                            #
# Created on 20 Jun 2014                                                       #
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

from PyQt4 import QtCore, QtGui 

from gui.PymolQtWidget import PymolQtWidget
from gui.matplotlibQtWidget import matplotlibWidget
from gui import res_rc

class MdiChild(QtGui.QTextEdit):
    sequenceNumber = 1

    def __init__(self, widget=None):
        super(MdiChild, self).__init__()

        self.setAttribute(QtCore.Qt.WA_DeleteOnClose)
        self.isUntitled = True
        
    def newFile(self):
        self.isUntitled = True
        self.curFile = "document%d.txt" % MdiChild.sequenceNumber
        MdiChild.sequenceNumber += 1
        self.setWindowTitle(self.curFile + '[*]')

        self.document().contentsChanged.connect(self.documentWasModified)

    def loadFile(self, fileName):
        file = QtCore.QFile(fileName)
        if not file.open(QtCore.QFile.ReadOnly | QtCore.QFile.Text):
            QtGui.QMessageBox.warning(self, "MDI",
                "Cannot read file %s:\n%s." % (fileName, file.errorString()))
            return False

        instr = QtCore.QTextStream(file)
        QtGui.QApplication.setOverrideCursor(QtCore.Qt.WaitCursor)
        self.setPlainText(instr.readAll())
        QtGui.QApplication.restoreOverrideCursor()

        self.setCurrentFile(fileName)

        self.document().contentsChanged.connect(self.documentWasModified)

        return True

    def save(self):
        if self.isUntitled:
            return self.saveAs()
        else:
            return self.saveFile(self.curFile)

    def saveAs(self):
        fileName = QtGui.QFileDialog.getSaveFileName(self, "Save As",
                self.curFile)
        if not fileName:
            return False

        return self.saveFile(fileName)

    def saveFile(self, fileName):
        file = QtCore.QFile(fileName)

        if not file.open(QtCore.QFile.WriteOnly | QtCore.QFile.Text):
            QtGui.QMessageBox.warning(self, "MDI",
                    "Cannot write file %s:\n%s." % (fileName, file.errorString()))
            return False

        outstr = QtCore.QTextStream(file)
        QtGui.QApplication.setOverrideCursor(QtCore.Qt.WaitCursor)
        outstr << self.toPlainText()
        QtGui.QApplication.restoreOverrideCursor()

        self.setCurrentFile(fileName)
        return True

    def printer(self):
        '''send document to printer'''
        printer = QtGui.QPrinter()
        document  = QtGui.QTextDocument(self.userFriendlyCurrentFile())
        document.setHtml(self.renderHtml())
        document.print_(printer)
        dialog = QtGui.QPrintDialog(printer)
        dialog.setModal(True)
        dialog.setWindowTitle("Print %s" % self.userFriendlyCurrentFile())
        dialog.setWindowIcon(QtGui.QIcon(":print.svg"))
        pdialog = QtGui.QPrintPreviewDialog(printer)
        pdialog.setWindowIcon(QtGui.QIcon(":print.svg"))
        pdialog.setWindowFlags(QtCore.Qt.Window)
        pdialog.exec_()

    def undo(self):
        pass
        
    def redo(self):
        pass
    
    def renderHtml(self):
        html = u""
        title = "<p align=right><b>%s</b>" % self.curFile
        text = "<p>%s</p>" % str(self.document()).replace('\n', "<br>")
        return html + title + text

    def userFriendlyCurrentFile(self):
        return self.strippedName(self.curFile)

    def currentFile(self):
        return self.curFile

    def closeEvent(self, event):
        if self.maybeSave():
            MdiChild.sequenceNumber -= 1
            event.accept()
        else:
            event.ignore()

    def documentWasModified(self):
        self.setWindowModified(self.document().isModified())
        self.setWindowTitle(self.userFriendlyCurrentFile() + "[*]")

    def maybeSave(self):
        if self.document().isModified():
            ret = QtGui.QMessageBox.warning(self, "MDI",
                    "'%s' has been modified.\nDo you want to save your "
                    "changes?" % self.userFriendlyCurrentFile(),
                    QtGui.QMessageBox.Save | QtGui.QMessageBox.Discard |
                    QtGui.QMessageBox.Cancel)
            if ret == QtGui.QMessageBox.Save:
                return self.save()
            elif ret == QtGui.QMessageBox.Cancel:
                return False

        return True

    def setCurrentFile(self, fileName):
        self.curFile = QtCore.QFileInfo(fileName).canonicalFilePath()
        self.isUntitled = False
        self.document().setModified(False)
        self.setWindowModified(False)
        self.setWindowTitle(self.userFriendlyCurrentFile() + "[*]")

    def strippedName(self, fullFileName):
        return QtCore.QFileInfo(fullFileName).fileName()


class IVMdiChild(MdiChild):
    '''An IV curve MDI chid window class'''
    def __init__(self):
        pass
    
class PymolMdiChild(PymolQtWidget):
    '''A modelling MDI chid window class'''
    def __init__(self, parent, enableUi, File=""):
        super(PymolMdiChild, self).__init__()

    def newFile(self):
        pass
    
    def loadFile(self, fileName):
        self.load(fileName)
    
    def undo(self):
        pass
        
    def redo(self):
        pass
    
    def save(self):
        if self.isUntitled:
            return self.saveAs()
        else:
            return self.saveFile(self.curFile)

    def saveAs(self):
        fileName = QtGui.QFileDialog.getSaveFileName(self, "Save As",
                self.curFile)
        if not fileName:
            return False

        return self.saveFile(fileName)

    def saveFile(self, fileName):
        file = QtCore.QFile(fileName)

        if not file.open(QtCore.QFile.WriteOnly | QtCore.QFile.Text):
            QtGui.QMessageBox.warning(self, "MDI",
                    "Cannot write file %s:\n%s." % (fileName, file.errorString()))
            return False

        QtGui.QApplication.setOverrideCursor(QtCore.Qt.WaitCursor)
        QtGui.QApplication.restoreOverrideCursor()

        self.setCurrentFile(fileName)
        return True

    def printer(self):
        '''send document to printer'''
        pass