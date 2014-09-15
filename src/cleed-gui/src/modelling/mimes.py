from PyQt4.QtCore import Qt, QString, QIODevice, QByteArray, QMimeData
from PyQt4.QtGui import QIcon

class MIME(QObject):
    '''base MIME class'''
    def __init__(self, object=None, mime="application/x-default-mime"):
        self.MimeData = QMimeData()
        self.icon = QIcon()
        self.mime = mime
        self.setData(object)
    
    def setData(self, object):
        '''set MIME data'''
        self.data = object
        self._updateData()
        
    def setIcon(self, icon):
        '''set icon for MIME'''
        if isinstance(icon, QIcon):
            self.icon = icon
        elif isinstance(icon, str) or isinstance(icon, QString):
            self.icon = QIcon(icon)
    
    def _updateData(self):
        '''update mime data'''
        data = QByteArray()
        stream = QDataStream(data, QIODevice.WriteOnly)
        stream << str(self.data) << self.icon
        self.MimeData.setData(self.mime, data)
        