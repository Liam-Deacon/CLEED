#!/usr/bin/env python
# encoding: utf-8

'''

'''
from __future__ import print_function, unicode_literals
from __future__ import absolute_import, division, with_statement

from PyQt4 import QtGui, QtCore
from PyQt4.QtCore import SIGNAL, SLOT


class ProjectTreeView(QtGui.QTreeView):
    '''
    Dialog class for updating sequences 
    '''
    def __init__(self, parent=None):
        super(QtGui.QTreeView, self).__init__(parent)
        
        #create contex menu
        self.menu = QtGui.QMenu(self)
        self.menu.addAction("testEntry", self.slotShow500x500, "CTRL+T")
        
        self.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        #self.customContextMenuRequested.connect(self.execContextMenu)
        self.connect(self, 
                 SIGNAL('customContextMenuRequested(QPoint)'), 
                 self.contextMenuRequested)
                # create context menu
        self.popMenu = QtGui.QMenu(self)
        self.popMenu.addAction(QtGui.QAction('test0', self))
        self.popMenu.addAction(QtGui.QAction('test1', self))
        self.popMenu.addSeparator()
        self.popMenu.addAction(QtGui.QAction('test2', self))       
            
    @QtCore.pyqtSlot(QtCore.QPoint)
    def contextMenuRequested(self, point):
        self.menu = QtGui.QMenu()
        
        action1 = menu.addAction("Set Size 100x100")
        action2 = menu.addAction("Set Size 500x500") 
    
        self.connect(action2, SIGNAL("triggered()"),
                    self, SLOT("slotShow500x500()"))
        self.connect(action1, SIGNAL("triggered()"),
                    self, SLOT("slotShow100x100()"))                    
        menu.exec_(self.mapToGlobal(point))

    
    @QtCore.pyqtSlot()
    def slotShow500x500(self):
        self.setFixedSize(500, 500)
        self.show()

    @QtCore.pyqtSlot()
    def slotShow100x100(self):
        self.setFixedSize(100, 100)
        self.show()

    def execContextMenu(self, point):
        self.menu.exec_(self.mapToGlobal(point))
        
    def showCustomContextMenu(self, pos):
        index = self.treeview.indexAt(pos)

        if not index.isValid():
            print(False)

        item = self.treeview.itemAt(pos)
        name = item.text(0)

        menu = QMenu(self)

       # your menu...

        menu.popup(QCursor.pos()) 
