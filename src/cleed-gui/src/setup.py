#!/usr/bin/env python
# encoding: utf-8
import os
import sys

#from setuptools import setup, find_packages
#import fix_setuptools_chmod
try:
    from setuptools import find_packages
except ImportError:
    from distutils.core import find_packages

from distutils.core import Extension, setup

if len(sys.argv) == 1:
    sys.argv.append('install')
    
dist = setup(
        name = 'cleed-gui',
        packages = find_packages(),
        version='0.1.0-dev',
        author='Liam Deacon',
        author_email='liam.deacon@diamond.ac.uk',
        license='GNU General Public License v3.0',
        url='https://pypi.python.org/pypi/cleed-gui',
        description='PyQt GUI front end to CLEED',
        long_description=open(os.path.join('phaseshifts', 'README.rst')
            ).read() if os.path.isfile('README.rst') else None,
        classifiers=[
            'Development Status :: 2 - Pre-Alpha',
            'Environment :: X11 Applications :: Qt',
            'Intended Audience :: Science/Research',
            'License :: OSI Approved :: GNU General Public License v3 or later (GPLv3+)',
            'Operating System :: OS Independent',
            'Programming Language :: Python',
            'Topic :: Scientific/Engineering :: Chemistry',
            'Topic :: Scientific/Engineering :: Physics',
            'Topic :: Scientific/Engineering :: Visualization',
            ],
        keywords='cleed-gui LEED-IV diffraction',
        include_package_data = True,
        package_data = {
            # If any package contains *.txt or *.rst files, include them:
            '': ['*.txt', '*.rst', '*.pyw'],
            },
        scripts = ["cleed-gui/cleed-gui.py"],
        install_requires = ['PyQy'],
        ext_modules=[],
        window=[os.path.join("cleed-gui", "cleed-gui.pyw")],
        # options={
            # 'py2exe': { 
                    # 'skip_archive':1,
                    # 'compressed':0,  
                    # 'bundle_files': 2, 
                    # 'dist_dir': os.path.join("dist", "py2exe"),
                    # 'excludes':['tcl', 'bz2'],
                    # 'dll_excludes':['w9xpopen.exe', 'tk85.dll', 'tcl85.dll']
                   # }
               # },
        #zipfile = None
               
)
