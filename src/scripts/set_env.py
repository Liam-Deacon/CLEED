#!/usr/bin/env python
# encoding: utf-8

##############################################################################
# Author: Liam Deacon                                                        #
#                                                                            #
# Contact: liam.deacon@diamond.ac.uk                                         #
#                                                                            #
# Created on 9 May 2014                                                      #
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
set_env -- setup CLEED environment

set_env sets the environment in which CLEED is run. Program paths are checked 
in the order: command line METAVAR > SYSTEM PATH > CLEED_HOME.  

'''
from __future__ import print_function, unicode_literals
from __future__ import absolute_import, division, with_statement

import sys
import os

import ConfigParser

from os import environ, path
from platform import system
from argparse import ArgumentParser
from argparse import RawDescriptionHelpFormatter

__all__ = []
__version__ = 0.1
__date__ = '2014-05-09'
__updated__ = '2014-05-09'
__contact__ = 'liam.deacon@diamond.ac.uk'

VERBOSE = 0
DEBUG = 0
TESTRUN = 0
PROFILE = 0


class ConfigParserWithComments(ConfigParser.ConfigParser):
    def __init__(self):
        ConfigParser.ConfigParser.__init__(self)
        self.optionxform = str
    
    def add_comment(self, section, comment):
        self.set(section, '; %s' % (comment,), None)

    def write(self, fp):
        """Write an .ini-format representation of the configuration state."""
        if self._defaults:
            fp.write("[%s]\n" % ConfigParser.DEFAULTSECT)
            for (key, value) in self._defaults.items():
                self._write_item(fp, key, value)
            fp.write("\n")
        for section in self._sections:
            fp.write("[%s]\n" % section)
            for (key, value) in self._sections[section].items():
                self._write_item(fp, key, value)
            fp.write("\n")

    def _write_item(self, fp, key, value):
        if key.startswith(';') and value is None:
            fp.write("%s\n" % (key,))
        else:
            fp.write("%s=%s\n" % (key, str(value).replace('\n', '\n\t')))


class CLIError(Exception):
    '''Generic exception to raise and log different fatal errors.'''
    def __init__(self, msg):
        super(CLIError).__init__(type(self))
        self.msg = "E: %s" % msg
        
    def __str__(self):
        return self.msg
    
    def __unicode__(self):
        return self.msg


def norm_path(p):
    return os.path.normpath(
                os.path.expanduser(os.path.expandvars(p)))


def which(program):
    import os
    
    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(norm_path(program))
    if fpath:
        if is_exe(program):
            return program
    else:
        # look in path
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file
        
        # look in CLEED_HOME environment
        exe_file = os.path.join(os.environ["CLEED_HOME"], 'bin', program) 
        if is_exe(exe_file):
            return exe_file

    return None


def read_ini(filename):
    ''' Read ini file variables '''
    config = ConfigParserWithComments()
    config.read(filename)
    
    if VERBOSE:
        print("Reading environment setup from '%s'" % filename)
        print(config)
        print(config.defaults())
    
    return config


def write_ini(filename, config):
    ''' Write ini file variables '''
    config.write(filename)


def main(argv=None): 
    '''Command line options.'''

    if argv is None:
        argv = sys.argv
    else:
        sys.argv.extend(argv)

    program_name = os.path.basename(sys.argv[0])
    program_version = "- version %s" % __version__
    program_build_date = str(__updated__)
    program_version_message = '%%(prog)s %s (%s)' % (program_version, 
                                                     program_build_date)
    program_shortdesc = __import__('__main__').__doc__.split("\n")[1]
    program_longdesc = '\n'.join([
                  line for line in __import__('__main__').__doc__.split("\n") 
                  if not line.startswith('@')])
    program_license = '''%s

  Created by Liam Deacon on %s.
  Copyright 2014 Diamond Light Source Ltd. All rights reserved.

  Licensed under the Apache License 2.0
  http://www.apache.org/licenses/LICENSE-2.0

  Distributed on an "AS IS" basis without warranties
  or conditions of any kind, either express or implied.

  Please send your feedback, including bug notifications 
  and fixes to: %s

usage:-
''' % (program_longdesc, str(__date__), __contact__)

    if system() == "Windows":
        exe = '.exe'
    else:
        exe = ''
        
    try:
        # Setup argument parser
        parser = ArgumentParser(description=program_license, 
                                formatter_class=RawDescriptionHelpFormatter)
        parser.add_argument('-c', '--check', dest='check', action='store_true',
                            default=False, help='perform post-processing '
                            'checks to ensure all paths and executables are '
                            'set correctly')
        parser.add_argument('-x', '--ext', nargs='+', dest='ext', 
                            metavar="<ext>", action='append', 
                            help="set enabled extensions;"
                            " can be any of 'aoi', 'sym' or 'phsh'. Note: "
                            "Multiple extensions can be enabled "
                            "simultaneously.")
        parser.add_argument('-e', '--env', nargs='+', dest='env', 
                            metavar="<env>", action='append', 
                            help="set custom environment "
                            "values of the form: \"<var>:<value>\"")
        parser.add_argument('-i', '--input', dest='input', 
                            metavar="<ini_file>", default=None,
                            help="load custom environment from %(metavar)s.")
        parser.add_argument('-o', '--output', dest='output', 
                            metavar="<ini_file>", default="cleed.ini",
                            help='save custom environment to %(metavar)s'
                            ' [default: "%(default)s"]. Note %(metavar)s'
                            ' is always written')
        parser.add_argument('--aoi-leed', dest='aoi_leed', 
                            default='caoi_leed' + exe,
                            metavar='<AOI_LEED>', help='path to angle of '
                            'incidence LEED program wrapper'
                            ' [default: "%(default)s"]')
        parser.add_argument('--aoi-rfac', dest='aoi_rfac', 
                            metavar='<AOI_RFAC>', default='caoi_rfac' + exe,
                            help='path to angle of incidence R-Factor wrapper'
                            ' program [default: "%(default)s"]')
        parser.add_argument('--leed', dest='leed', default='cleed_nsym' + exe,
                            metavar='<LEED>', help='path to LEED program'
                            ' [default: "%(default)s"]')
        parser.add_argument('--search', dest='search', default='csearch' + exe,
                            metavar="<SEARCH>", help='path to search program'
                            ' [default: "%(default)s"]')
        parser.add_argument('--sym', dest='sym', default='cleed_sym' + exe,
                            metavar='<LEED_SYM>', 
                            help='path to symmetrised LEED program'
                            ' [default: "%(default)s"]')
        parser.add_argument('--rfac', dest='rfac', default='crfac' + exe, 
                            metavar="<RFAC>", help='path to R-Factor program'
                            ' [default: "%(default)s"]')
        parser.add_argument('--phsh', dest='phsh', default='phsh.py',
                            metavar='<PHSH>', help='path to phase shifts program'
                            ' [default: "%(default)s"]')
        parser.add_argument("-v", "--verbose", dest="verbose", 
                            action="count", 
                            help="set verbosity level [default: %(default)s]")
        parser.add_argument('-V', '--version', action='version', 
                            version=program_version_message)

        # Process arguments
        args = parser.parse_args()

        VERBOSE = args.verbose
        env = 'default'
        prog_cycle = {}

        if args.ext:
            extensions = [] 
            for ext in args.ext:
                for extension in ext:
                    extensions.append(extension)
            args.ext = extensions
        
        ini_config = ConfigParserWithComments()
        variables = {}
        
        if args.input:
            if os.path.isfile(norm_path(str(args.input))):
                ini_config = read_ini(norm_path(args.input))
                if ini_config.has_section("ENVIRONMENT"):
                    variables = ini_config.items("ENVIRONMENT")
                else:
                    variables = ini_config.defaults()
                    
                print(variables)
        
        if args.env: 
            for env in args.env:
                for variable in env:
                    var = variable.split(':')[0]
                    value = ":".join([val for i,val in enumerate(
                                    variable.split(':')) if i > 0])
                    variables[var] = value 

        if VERBOSE:
            print('Running %s script:' % program_name)

        # default executable filenames
        caoi_leed_exe = norm_path(args.aoi_leed)
        caoi_rfac_exe = norm_path(args.aoi_rfac)
        cleed_nsym_exe = norm_path(args.leed) 
        cleed_sym_exe = norm_path(args.sym)
        crfac_exe = norm_path(args.rfac)
        csearch_exe = norm_path(args.search)
        phsh_exe = norm_path(args.phsh)
        
        # set default environment variables
        if system() == "Windows":
            # CLEED root dir
            if "CLEED_HOME" not in environ:
                environ["CLEED_HOME"] = "C:\Program Files (x86)\CLEED"
            elif path.isdir(norm_path(environ["CLEED_HOME"])):
                sys.stderr.write("warning (CLEED_HOME): '%s' is not a valid "
                                 "directory (using default)\n" 
                                 % norm_path(environ['CLEED_HOME']))
                sys.stderr.flush()
                environ["CLEED_HOME"] = "C:\Program Files (x86)\CLEED"
                  
        elif system() == "Linux" or system() == "Unix":
            environ["CLEED_HOME"] = "/usr/local/CLEED"  # CLEED root dir
        
        if "CLEED_PHASE" not in environ:  # path to phase shifts
            environ["CLEED_PHASE"] = path.join(environ["CLEED_HOME"], "phase")  
        elif path.isdir(norm_path(environ["CLEED_PHASE"])):
            sys.stderr.write("warning (CLEED_PHASE): '%s' "
                             "is not a valid directory (using default)\n" 
                             % norm_path(environ['CLEED_PHASE']))
            sys.stderr.flush()
            environ["CLEED_PHASE"] = path.join(environ["CLEED_HOME"], "phase")
              
        environ["CSEARCH_LEED"] = path.join(environ["CLEED_HOME"], "bin", 
                                            "cleed_nsym%s" % exe)  # path to cleed_nsym
        
        environ["CSEARCH_RFAC"] = path.join(environ["CLEED_HOME"], "bin", 
                                            "crfac%s" % exe)     # path to crfac

        # assign default values
        cleed_home = environ["CLEED_HOME"]
        cleed_phase = environ["CLEED_PHASE"]
        phaseshifts_leed = ''

        # set following environment variables ahead of time
        if "CLEED_HOME" in variables:
            cleed_home = path.abspath(path.expanduser(path.expandvars(
                            variables["CLEED_HOME"])))
            if path.isdir(cleed_home):
                environ["CLEED_HOME"] = cleed_home
        else:
            variables["CLEED_HOME"] = environ["CLEED_HOME"]
        
        if "CLEED_PHASE" in variables:
            cleed_phase = path.abspath(path.expanduser(path.expandvars(
                            variables["CLEED_PHASE"])))
            if path.isdir(cleed_phase):
                environ["CLEED_PHASE"] = cleed_phase
        else:
            variables["CLEED_PHASE"] = environ["CLEED_PHASE"]
        
        try:
            csearch_leed = environ["CSEARCH_LEED"]
        except KeyError:
            environ["CSEARCH_LEED"] = path.join(environ["CLEED_HOME"], 'bin', 
                                                cleed_nsym_exe)
            csearch_leed = environ["CSEARCH_LEED"] 
        
        try:
            csearch_rfac = environ["CSEARCH_RFAC"]
        except KeyError:
            environ["CSEARCH_RFAC"] = path.join(environ["CLEED_HOME"], 'bin', 
                                                crfac_exe)
            csearch_rfac = environ["CSEARCH_RFAC"]
        
        # add variables to environment
        if variables:
            print()
            print('Setting custom environment...')
            for var in variables:
                print('%s -> %s' % (var, variables[var]))
        
        if "CSEARCH_LEED" not in variables:
            variables["CSEARCH_LEED"] = csearch_leed
            csearch_leed = cleed_nsym_exe
            
        if "CSEARCH_RFAC" not in variables:
            variables["CSEARCH_RFAC"] = csearch_rfac
            csearch_rfac = crfac_exe
            
            prog_cycle["CSEARCH_LEED"] = 'CSEARCH_RFAC'
            
        # determine whether to enable extensions
        if args.ext:
            env = ''
            
            print()
            print("=" * 80)
            print('Testing extensions:')
            print("=" * 80)
            
            if 'phsh' in args.ext:
                phsh_exe = which(phsh_exe)
                if phsh_exe == None:
                    sys.stderr.write("Unable to find 'phsh.py' - "
                                     "phaseshifts extension disabled\n")
                    sys.stderr.flush()
                else:
                    env += 'phsh '
                    print('phsh -> %s' % phsh_exe)

            if 'sym' in args.ext:
                cleed_sym_exe = which(cleed_sym_exe)
                if cleed_sym_exe == None:
                    sys.stderr.write("Unable to find 'cleed_sym' - "
                                     "symmetry extension disabled\n")
                    sys.stderr.flush()
                else:
                    env += 'sym '
                    print('sym -> %s' % cleed_sym_exe)
                    
            if 'aoi' in args.ext:
                caoi_leed_exe = which(caoi_leed_exe)
                caoi_rfac_exe = which(caoi_rfac_exe)
                
                if caoi_leed_exe == None:
                    
                    sys.stderr.write("Unable to find 'caoi_leed' - "
                                     "angle of incidence extension disabled\n")
                    sys.stderr.flush()
                
                elif caoi_rfac_exe == None:
                    sys.stderr.write("Unable to find 'caoi_rfac' - "
                                     "angle of incidence extension disabled\n")
                    sys.stderr.flush()
                
                if caoi_rfac_exe != None and caoi_leed_exe != None:
                    env += 'aoi'
                    print('aoi_leed -> %s' % caoi_leed_exe)
                    print('aoi_rfac -> %s' % caoi_rfac_exe)
                    
            if 'aoi' in args.ext and 'sym' in args.ext and 'phsh' in args.ext:
                csearch_leed = phsh_exe
                caoi_leed = cleed_sym_exe
                caoi_rfac = crfac_exe
                csearch_rfac = caoi_rfac_exe
                phaseshifts_leed = caoi_leed_exe
                
                variables["CSEARCH_LEED"] = csearch_leed
                variables["CSEARCH_RFAC"] = csearch_rfac
                variables["CAOI_LEED"] = caoi_leed
                variables["CAOI_RFAC"] = caoi_rfac
                variables["PHASESHIFTS_LEED"] = phaseshifts_leed
                
                prog_cycle["CSEARCH_LEED"] = 'PHASESHIFTS_LEED'
                prog_cycle["PHASESHIFTS_LEED"] = 'CAOI_LEED'
                prog_cycle["CAOI_LEED"] = 'CAOI_RFAC'
                prog_cycle["CAOI_RFAC"] = 'CSEARCH_RFAC'
                
                
            elif 'aoi' in args.ext and 'sym' in args.ext:
                csearch_leed = caoi_leed_exe
                caoi_leed = cleed_sym_exe
                caoi_rfac = crfac_exe
                csearch_rfac = caoi_rfac_exe

                variables["CSEARCH_LEED"] = csearch_leed
                variables["CSEARCH_RFAC"] = csearch_rfac
                variables["CAOI_LEED"] = caoi_leed
                variables["CAOI_RFAC"] = caoi_rfac
                
                prog_cycle["CSEARCH_LEED"] = 'CAOI_LEED'
                prog_cycle["CAOI_LEED"] = 'CAOI_RFAC'
                prog_cycle["CAOI_RFAC"] = 'CSEARCH_RFAC'
                
            elif 'aoi' in args.ext and 'phsh' in args.ext:
                csearch_leed = caoi_leed_exe
                caoi_leed = cleed_sym_exe
                caoi_rfac = crfac_exe
                csearch_rfac = caoi_rfac_exe
                
                variables["CSEARCH_LEED"] = csearch_leed
                variables["CSEARCH_RFAC"] = csearch_rfac
                variables["CAOI_LEED"] = caoi_leed
                variables["CAOI_RFAC"] = caoi_rfac
                variables["PHASESHIFTS_LEED"] = phaseshifts_leed
                
                prog_cycle["CSEARCH_LEED"] = 'PHASESHIFTS_LEED'
                prog_cycle["PHASESHIFTS_LEED"] = 'CAOI_LEED'
                prog_cycle["CAOI_LEED"] = 'CAOI_RFAC'
                prog_cycle["CAOI_RFAC"] = 'CSEARCH_RFAC'
                
            elif 'sym' in args.ext and 'phsh' in args.ext:
                csearch_leed = phsh_exe
                csearch_rfac = caoi_rfac_exe                
                phaseshifts_leed = caoi_leed_exe
                
                variables["CSEARCH_LEED"] = csearch_leed
                variables["CSEARCH_RFAC"] = csearch_rfac
                variables["PHASESHIFTS_LEED"] = phaseshifts_leed
                
                prog_cycle["CSEARCH_LEED"] = 'PHASESHIFTS_LEED'
                prog_cycle["PHASESHIFTS_LEED"] = 'CSEARCH_RFAC'
                
            elif 'aoi' in args.ext:
                csearch_leed = caoi_leed_exe
                caoi_leed = cleed_nsym_exe
                caoi_rfac = crfac_exe
                csearch_rfac = caoi_rfac_exe
                
                variables["CSEARCH_LEED"] = csearch_leed
                variables["CSEARCH_RFAC"] = csearch_rfac
                variables["CAOI_LEED"] = caoi_leed
                variables["CAOI_RFAC"] = caoi_rfac
                
                prog_cycle["CSEARCH_LEED"] = 'CAOI_LEED'
                prog_cycle["CAOI_LEED"] = 'CAOI_RFAC'
                prog_cycle["CAOI_RFAC"] = 'CSEARCH_RFAC'
                
            elif 'sym' in args.ext:
                csearch_leed = cleed_sym_exe
                csearch_rfac = crfac_exe
                
                variables["CSEARCH_LEED"] = csearch_leed
                variables["CSEARCH_RFAC"] = csearch_rfac
                
                prog_cycle["CSEARCH_LEED"] = 'CSEARCH_RFAC'
                
            elif 'phsh' in args.ext:
                csearch_leed = phsh_exe
                csearch_rfac = crfac_exe
                phaseshifts_leed = cleed_nsym_exe
                
                variables["CSEARCH_LEED"] = csearch_leed
                variables["CSEARCH_RFAC"] = csearch_rfac
                variables["PHASESHIFTS_LEED"] = phaseshifts_leed
                
                prog_cycle["CSEARCH_LEED"] = 'PHASESHIFTS_LEED'
                prog_cycle["PHASESHIFTS_LEED"] = 'CSEARCH_RFAC'
                
            else:  # default variables
                csearch_leed = cleed_nsym_exe
                csearch_rfac = crfac_exe
                
                variables["CSEARCH_LEED"] = csearch_leed
                variables["CSEARCH_RFAC"] = csearch_rfac
                
                prog_cycle["CSEARCH_LEED"] = 'CSEARCH_RFAC'
        
        print()
        print("=" * 80)        
        print("CLEED Environment: %s" % env)
        print("=" * 80)
        for var in variables:
            print("%s = %s" % (var, variables[var]))
            environ[var] = variables[var]
        print()
        print("Note: For older versions of CLEED paths should be encapsulated"
              " in double quotation marks or else spaces, etc. need to be "
              "escaped")

        if args.check:  # check everything is valid
            
            print()
            print("=" * 80)
            print('Checking setup:')
            print("=" * 80)
            pointer = 'CSEARCH_LEED'
            i = 1
            print('Program iteration loop is as follows:')
            while(pointer != None ):
                try:
                    if which(environ[pointer]):
                        print('   %i. %s ("%s")' % (i, 
                                        path.split(environ[pointer])[1], 
                                        which(environ[pointer])))
                    else:
                        sys.stderr.write('Error: "%s" not found' % environ[pointer])
                        sys.stderr.flush()
                
                except KeyError:
                    sys.stderr.write("Error: %s not in environment!\n" % pointer)
                    sys.stderr.flush()
                    break
                
                try:                    
                    if pointer == "CSEARCH_RFAC":
                        break
                    
                    pointer = prog_cycle[pointer]
                    i += 1
                    
                except KeyError:
                    sys.stderr.write("Error: Unknown step '%s'\n" % pointer)
                    sys.stderr.flush()
                    break                        

        if args.output:  # always true -> store INI file of CLEED environment
            if not ini_config.has_section("ENVIRONMENT"):
                # create new section and add variables 
                ini_config.add_section("ENVIRONMENT")
                ini_config.add_comment("ENVIRONMENT", 
                                       "CLEED environment values set by: %s" 
                                       % program_name)
            # set variables in ENVIRONMENT section
            ini_config.set("ENVIRONMENT", "CLEED_HOME", 
                           environ["CLEED_HOME"])
            ini_config.set("ENVIRONMENT", "CLEED_PHASE", 
                           environ["CLEED_PHASE"])
            for var in variables:
                ini_config.set("ENVIRONMENT", var, variables[var])
                
            # save to file
            with open(norm_path(args.output), 'w') as configfile:
                ini_config.write(configfile)
                
            if VERBOSE:
                print("Wrote environment to INI file '%s'" % 
                      norm_path(args.output))
            
        return 0
    
    except KeyboardInterrupt:
        ### handle keyboard interrupt ###
        return 0
    
    except Exception, e:
        if DEBUG or TESTRUN:
            raise(e)
        indent = len(program_name) * " "
        sys.stderr.write(program_name + ": " + repr(e) + "\n")
        sys.stderr.write(indent + "  for help use --help")
        return 2

if __name__ == "__main__":
    if DEBUG:
        sys.argv.append("-h")
        sys.argv.append("-v")
        sys.argv.append("-r")
    if TESTRUN:
        import doctest
        doctest.testmod()
    if PROFILE:
        import cProfile
        import pstats
        profile_filename = 'set_env_profile.txt'
        cProfile.run('main()', profile_filename)
        statsfile = open("profile_stats.txt", "wb")
        p = pstats.Stats(profile_filename, stream=statsfile)
        stats = p.strip_dirs().sort_stats('cumulative')
        stats.print_stats()
        statsfile.close()
        sys.exit(0)
    sys.exit(main())
