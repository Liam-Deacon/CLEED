class Reflex(object):
    '''spot class'''
    def __init__(self, h=0., k=1.):
        self.h = float(h)
        self.k = float(k)
    
    def __str__(self):
        return "(%s, %s)" % (str(self.h), str(self.k))
    
    def __repr__(self):
        return "Reflex(h=%s, k=%s)" % (str(self.h), str(self.k))
    
    def setH(self, h):
        '''set h Millar index'''
        try:
            self.h = float(h)
        except ValueError:
            pass

    def setK(self, k):
        '''set k Millar index'''
        try:
            self.k = float(k)
        except ValueError:
            pass        
