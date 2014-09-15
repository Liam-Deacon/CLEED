from reflex import Reflex 

class IV(Reflex):
    def __init__(self, reflex=Reflex(), data=None, filename=None):
        if isinstance(filename, str):
            # attempt to load data from filename string
                self.filename = filename
                self.loadFromFile(data)
        else:
            self.setData(data)
    
    def __repr__(self):
        return "IV(reflex=%s, data=%s)" % (str(self.reflex), str(data))
    
    def setData(self, data):
        self.data = data
        
    def loadFromFile(self, filename=None):
        '''load iv data from file'''
        if not filename:
            if self.filename:
                filename = self.filename
            else:
                raise IOError('filename is NoneType')
            
        try:
            import numpy as np
            
            self. data = np.loadtxt(filename, dtype=float, 
                                    comments='#', delimiter=' ')
        except ImportError:
            with open(filename, 'r') as f:
                lines = [line.lstrip().rstrip('#').rstrip() for line in f
                         if not line.lstrip().startswith('#')]
                x = [0] * len(lines)
                y = [0] * len(lines)
                for i, line in enumerate(lines):
                    x[i], y[i], = [t(s) for (t, s) in zip((float, float), 
                                                          line.split())] 
            self.data = [x, y]
        except IOError:
            raise IOError("Unable to open IV file: '%s'" % filename) 
                    
                
    def saveFile(self, filename=None):
        '''save iv data to disk'''
        if not filename:
            if self.filename:
                filename = self.filename
            else:
                raise IOError('filename is NoneType')
        
        try:
            import numpy as np
            
            np.savetxt(filename, self.data, delimiter=' ', 
                       header=str(self.reflex), comments='#')
            
        except ImportError:
            with open(filename, 'w') as f:
                f.write('# %s\n' % str(self.reflex))
                for i in len(data):
                    f.write('%f %f\n' % (self.data[0][i], self.data[1][i]))
        
        except IOError:
            raise IOError("Unable to write IV data to file: '%s'" % filename)
        