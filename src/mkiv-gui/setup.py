#!/usr/bin/env python
from distutils.core import setup
from glob import glob
import platform, sys, ntpath
import py2exe

excludes = []
includes = []
data_files = []

# Include MSVC DLLs
if platform.system() == 'Windows':
	sys.path.append("C:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\redist\\x86\\Microsoft.VC90.CRT")
	data_files = [("Microsoft.VC90.CRT", glob(r'C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT\*.*'))]

# SciPy fix
for include in  []: #["scipy.special.__cvs_version__", "scipy.linalg.__cvs_version__", "scipy.special.cephes"]:
	includes.append(include) 
	
# PyQt fix
for include in ["sip"]:
	includes.append(include)
if sys.version_info.major < 3:
	excludes.append('PyQt4.uic.port_v3')

# skimage fix
#data_files.append(("",glob(ntpath.join(sys.exec_prefix, "Lib\site-packages\skimage\*"))))
includes.append('skimage')
	
opts = {
    "py2exe": {
        "optimize":2,
        "includes":includes,
        "excludes":excludes
    }
} 


# Setup
setup(
	name="Extract-PyV",
	author="Liam Deacon",
	author_email="liam.m.deacon@gmail.com",
    windows = [
        {
            "script": "main.py",
            "icon_resources":[(1, "PyV.ico")],
        }
    ],
	data_files=data_files,
	zipfile=None,
	options=opts
)
