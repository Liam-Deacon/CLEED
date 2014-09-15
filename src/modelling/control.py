
import os.path

from modelling.reflex import Reflex    

class ControlItem(object):
    '''class to contain control file items'''
    def __init__(self, ivFile=None, reflexes=[Reflex()], id=0, weighting=1.):
        # initialise variables
        self.iv = None
        self.reflexes = [Reflex()]
        self.id = 0
        self.weighting = 1.
        
        # update variables
        self.setIV(ivFile)
        self.setReflexes(reflexes)
        self.setId(id)
        self.setWeighting(weighting)
    
    def __str__(self):
        return 'ef="%s":ti=%s:id=%s:wt=%s' % (self.iv, 
                            "+".join([str(reflex) for reflex in self.reflexes]), 
                            str(self.id), str(self.weighting))
    
    def __repr__(self):
        return ("ControlItem(ivFile='%s', reflexes=%s, id='%s', "
                "weighting=%s)" % (self.iv, str(self.reflexes), 
                                  str(self.id), str(self.weighting)))
     
    def __hash__(self):
        return hash(self.__repr__())
    
    def addReflex(self, reflex):
        '''append a reflex to reflexes'''
        if isinstance(reflex, Reflex):
            self.reflexes.append(reflex)
    
    def setWeighting(self, weighting):
        '''set weighting of IV curve'''
        try:
            if weighting <= 1. and weighting >= 0.:
                self.weighting = float(weighting)
        except ValueError:
            pass
        
    def setId(self, id):
        '''set identifier string or number'''
        self.id = id
        
    def setReflexes(self, reflexes):
        '''set Millar indices'''
        self.reflexes = []
        for reflex in reflexes:
            if isinstance(reflex, Reflex):
                self.reflexes.append(reflex)
            
    def setIV(self, filepath):
        '''set the IV file for the control item to point to'''
        # normalise path and make absolute, expanding environment variables 
        path = os.path.abspath(
                       os.path.expanduser(
                              os.path.expandvars(
                                     os.path.normpath(filepath))))
        if os.path.isfile(path):
            self.iv = path
    
    def ivExists(self):
        path = os.path.abspath(
               os.path.expanduser(
                      os.path.expandvars(
                             os.path.normpath(self.iv))))
        if os.path.isfile(path):
            return True
        else:
            return False
        
    def isValidIV(self):
        raise NotImplementedError
        
class Control(object):
    '''LEED control class'''
    def __init__(self, controlList=[]):
        '''contolList is an array of ControlItems'''
        self.controlItems = []
        self.setControlItems(controlList)
        
    def __repr__(self):
        return "Control(%s)" % ",".join([item.__repr__() 
                                         for item in self.controlItems])
     
    def __hash__(self):
        return hash(self.__repr__())
    
    def setComment(self, comment):
        '''set comment for control'''
        self.comment = str(comment)
    
    def setControlItems(self, items):
        self.controlItems = []
        for item in items:
            if not isinstance(item, ControlItem):
                raise TypeError('%s is not a valid ControlItem instance' 
                                % str(item))
            self.controlItems.append(item)
            
    def addControlItem(self, item):
        if not isinstance(item, ControlItem):
            raise TypeError('%s is not a valid ControlItem instance' 
                                % str(item))
        self.controlItems.append(item)
        
    def clear(self):
        self.controlItems = []
    
    def loadFromFile(self, filename):
        '''load a control structure from disk'''
        comment = ""
        try:
            with open(filename, 'r') as f:
                for line in f:
                    line = str(line)
                    if line.lstrip().startswith('#'):
                        comment += line.lstrip()  # may need to make this smarter
                    elif "ef=" in line or "id=" in line or "ti=" or "wt=" in line:
                        args = line.lstrip().split(':')
                        ef = None
                        ti = []
                        id = None
                        wt = 1.
                        
                        for arg in args:
                            # remove surrounding whitespace
                            arg = arg.lstrip().rstrip().rstrip('#')
                            if arg.startswith("ef="):
                                ef = arg.lstrip("ef=")
                            elif arg.startswith("id="):
                                id = arg.lstrip("id=")
                            elif arg.startswith("ti="):
                                reflexes = arg.lstrip("ti=").split('+')
                                for reflex in reflexes:
                                    # create tuple for reflex
                                    reflex = eval(reflex.lstrip().rstrip())
                                    # add tuple to reflexes
                                    ti.append(Reflex(*reflex))
                            elif arg.startswith("wt="):
                                wt = arg.lstrip("wt=")
                        
                        if not id:
                            id = os.path.splitext(os.path.basename(ef))[0]
                        
                        item = ControlItem(ef, ti, id, wt)
                        self.addControlItem(item)
                        
                    self.setComment(comment)
        except IOError:
            assert(IOError("Cannot open control file: '%s'" % filename))                        
                    
    def saveToFile(self, filename):
        '''save a control structure to disk'''
        try:
            with open(filename, 'w') as f:
                f.write(self.comment)
                for item in self.controlItems:
                    f.write(str(item) + '\n')
                f.write('\n')
        except IOError:
            assert(IOError("Cannot save control file: '%s'" % filename))  
    
            
if __name__ == '__main__':
    ctr = ControlItem('C:/')
    ctr = Control([ctr])
    print(ctr)