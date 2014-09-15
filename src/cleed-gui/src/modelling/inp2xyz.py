from elements import ELEMENTS
import os

def inp2xyz(filename, output=None):
    atoms = []
    try:
        with open(filename, 'r') as f:
            lines = [line for line in f]
   
    except IOError:
        assert IOError("***error (inp2xyz): Could not open file '%s'" 
                       % filename)
       
            
    for line in lines:
        if (line.lstrip().startswith('po:') or 
            line.lstrip().startswith('pb:')):
            line = line.lstrip()[3:].lstrip()
            element, x, y, z, = [t(s) for (t, s) in 
                                 zip((str, float, float, float),
                                 line.split()[0:5])]
            element = element.split('_')[0]
            try:
                if element in ELEMENTS:
                    atoms.append("%s %9.6f %9.6f %9.6f" 
                                 % (ELEMENTS[element].symbol, x, y, z))
            except KeyError:
                pass

    try:
        with open(filename+".xyz", 'w') as f:
            f.write("%i \n" % len(atoms))
            f.write("#jmolscript: select *;"
                    "color bonds lightgrey;"
                    "background white;"
                    "set antialiasdisplay true;"
                    "set perspectiveDepth false;"
                    "set cameraDepth 10;"
                    "set diffusePercent 85;"
                    "boundboxColor = \"[xa9a9a9]\";"
                    "set ambientPercent 60;"
                    "set diffusePercent 85;"
                    "set specular true;"
                    "set specularPercent 22;"
                    "set specularPower 40;"
                    "set specularExponent 6;"
                    "set zShadePower 3;"
                    "\n")
            for atom in atoms:
                f.write(atom + '\n')
            
    except IOError:
        raise IOError("***error (inp2xyz): Could not write file '%s'" 
                      % filename+'.xyz' )
    
if __name__ == "__main__":
    inp2xyz(r"C:\Users\Liam\Dropbox\Windows Tweaks\CLEED_tools_win32\cleed\src\CLEED\examples\models\nio\Ni111_2x2O.inp")