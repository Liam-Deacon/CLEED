class RotationalSymmetry(object):
    def __init__(self, n_fold=1, x=0., y=0.):
        self.setN(n_fold)
        self.setRotationalAxis(x, y)
        
    def setRotationalAxis(self, x, y):
        self.x = float(x)
        self.y = float(y)
        
    def setN(self, n_fold):
        self.n_fold = int(n_fold)