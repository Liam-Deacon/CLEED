##############################################################################
# Author: Liam Deacon                                                        #
#                                                                            #
# Contact: liam.deacon@diamond.ac.uk                                         #
#                                                                            #
# Created on 19 Jun 2014                                                     #
#                                                                            #
# Copyright: Copyright (C) 2014 Liam Deacon                                  #
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

from PyQt4 import QtGui
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import matplotlib as mpl
import matplotlib.pyplot as plt


class MplCanvas(FigureCanvas):
    '''class to provide a matplotlib PyQt canvas''' 
    def __init__(self):
        self.plt = plt
        self.fig = self.plt.figure()
        self.ax = self.fig.add_subplot(111)
 
        FigureCanvas.__init__(self, self.fig)
        FigureCanvas.setSizePolicy(self, 
                    QtGui.QSizePolicy.Expanding,
                    QtGui.QSizePolicy.Expanding)
        FigureCanvas.updateGeometry(self)
 
 
class matplotlibWidget(QtGui.QWidget):
    '''class to provide a matplotlib PyQt widget'''
    def __init__(self, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.canvas = MplCanvas()
        self.vbl = QtGui.QVBoxLayout()
        self.vbl.addWidget(self.canvas)
        self.setLayout(self.vbl)


class matplotlibWidgetDemo(QtGui.QMainWindow):
    '''demo showing matplotlibWidget class'''
    def __init__(self, parent=None):
        QtGui.QMainWindow.__init__(self)
        widget = matplotlibWidget()
        widget2 = matplotlibWidget()

        self.setCentralWidget(widget)
        import random
        widget.canvas.fig.suptitle('matplotlibWidget Demo')
        fig = widget.canvas.fig 
        fig.set_facecolor('white')
        widget.canvas.plt.xlabel('x')
        
        widget2.show()
        

if __name__ == '__main__':
    import sys
    
    app = QtGui.QApplication(sys.argv)

    main = matplotlibWidgetDemo()
    main.show()

    sys.exit(app.exec_())
