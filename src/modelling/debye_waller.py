class DebyeWaller(Object):
    def __init__(self):
        pass
    
class dr1(Object):
    def __init__(self, r1=0.05):
        self.r = r1
        
    def __str__(self):
        return "dr1 %9.6f" % float(self.r)
    
class dr3(Object):
    def __init__(self, r1=0.05, r2=0.05, r3=0.05):
        self.r = (r1, r2, r3)
        
    def __str__(self):
        return "dr3 %9.6f %9.6f %9.6f" % (float(self.r))
    