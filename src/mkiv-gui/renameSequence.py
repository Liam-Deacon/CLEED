#!/usr/local/bin/python2.7
# encoding: utf-8
'''
renameSequence -- renames sequence of files

renameSequence is a module for renaming multiple files

It defines classes_and_methods

@author:     Liam Deacon
        
@copyright:  2013 Liam Deacon. All rights reserved.
        
@license:    MIT Licence

@contact:    liam.m.deacon@gmail.com
@deffield    updated: 2013-10-10
'''

import sys, os, ntpath, re, glob

from shutil import copy, move

from argparse import ArgumentParser
from argparse import RawDescriptionHelpFormatter

__all__ = []
__version__ = 0.1
__date__ = '2013-10-10'
__updated__ = '2013-10-10'

DEBUG = 0
TESTRUN = 0
PROFILE = 0

class renameSequence(object):
    files = []
    initial = 0
    final = 0
    duplicate_policy = 'i'
    output_path = None
    output_prefix = None
    output_suffix = None
    append_ext = True
    delete_original = False
    precision = 0
    percent = 0
    
    def __init__(self, files=[], initial=0, final=0, precision=0, duplicate_policy='ignore', output_path=None,
                     output_prefix=None, output_suffix=None, append_ext=True, delete_original=False):
        # Setup variables 
        if isinstance(files, list):
            self.files = list(files)
        elif isinstance(files, str):
            self.files = [files]
        self.initial = float(initial)
        self.final = float(final)
        self.duplicate_policy = duplicate_policy
        self.setOutput(output_path)
        self.setPrefix(output_prefix)
        self.setSuffix(output_suffix)
        self.append_ext = append_ext
        self.delete_original = delete_original 
        
        # Check variables
        if precision >= 0:
            self.precision = precision 
    
    #########################################
    # Setter methods
    #########################################
    def addFile(self, file):
        if os.path.isfile(file) and file not in self.files:
            self.files.append(file)
    
    def setAppendExtension(self, append=True):
        self.append_ext = bool(append)
       
        
    def setDeleteOriginal(self, delete=False):
        self.delete_original = bool(delete)
    
    
    def setDuplicatePolicy(self, policy='ignore'):
        if str(policy) in ['i', 'ui', 'ignore']:
            self.duplicate_policy = str(policy) 


    def setFiles(self, files):
        for file in files:
            if not ntpath.exists(str(file)):
                sys.stderr.write('Error: "{0}" does not exist - aborting\n'.format(str(file)))
                sys.stderr.flush()
                return
        self.files = files

    def setFinal(self, value):
        try:
            self.final = float(value)
        except:
            pass

            
    def setInitial(self, value=1):
        try:
            self.initial = float(value)
        except:
            pass

        
    def setOutput(self, path=None):
        validatedPath = ''.join(ch for ch in str(path) if ch.isalnum() or ch in ['_', ' ', '.', '-', ':', '/', '\\'])
        self.output_path = str(validatedPath)

    
    def setPrecision(self, precision=0):
        try:
            if int(precision) >= 0:
                self.precision = int(precision)
        except:
            pass
        
    def setPrefix(self, prefix=None):
        self.output_prefix = str(prefix)

    
    def setSuffix(self, suffix=None):
        self.output_suffix = str(suffix)
        
    
    #########################################
    # Rename files in sequence
    #########################################
    def rename(self):
        # Do Checks
        if len(self.files) < 1:
            sys.stderr.write('No files to rename\n')
            sys.stderr.flush()
            return
        for file in self.files:
            if not ntpath.exists(str(file)):
                sys.stderr.write('Error: {0} does not exist - aborting\n'.format(str(file)))
                sys.stderr.flush()
                return
        if self.initial == self.final:
            # Do simple renaming
            self.final = len(self.files)
            self.initial = 1
            sys.stderr.write('Warning: initial & final values are the same - using steps of 1\n')
            sys.stderr.flush()
        elif self.initial > self.final:
            temp = self.final
            self.final = self.initial        
            self.initial = temp
            sys.stderr.write('Warning: initial value > final value => swapped\n')
        if self.precision < 0:
            self.precision = 0
            
        total = float(len(self.files)-1)
        try:
            step = (self.final-self.initial)/total
        except ZeroDivisionError:
            step = 0
        format = '{:0.'+str(self.precision)+'f}'
        if not ntpath.exists(self.output_path):
            try:
                os.makedirs(self.output_path)
            except IOError:
                sys.stderr.write('Error: unable to create output directory\n')
                sys.stderr.flush()
                sys.exit(-1)
        if not os.path.isdir(self.output_path):
            os.makedirs(self.output_path)
    
        
        previous = ''    
        for i,file in enumerate(self.files):
            value = format.format(self.initial + step*i, self.precision)
            output = ntpath.splitext(ntpath.basename(file))[0]
            if not self.output_prefix:
                output = self.output_prefix
            output += '.' + str(value)
            if not self.output_suffix:
                output += self.output_suffix
            if self.append_ext:
                ext = ntpath.splitext(file)[1]
                output += ext
            if self.duplicate_policy != 'ignore' and output == previous:
                if self.duplicate_policy == 'i':
                    while os.path.isfile(output):
                        output+='i'
                elif self.duplicate_policy == 'ui':
                    delta = 1./pow(10, self.precision)
                    
            try:
                if self.delete_original:
                    move(file, os.path.join(self.output_path, output))
                else:
                    copy(file, os.path.join(self.output_path, output))
            except any as e:
                sys.stderr.write(e)
                sys.stderr.flush()
            self.percent = (i+1)/len(self.files)
        self.percent = 100
        
        
class CLIError(Exception):
    '''Generic exception to raise and log different fatal errors.'''
    def __init__(self, msg):
        super(CLIError).__init__(type(self))
        self.msg = "Error: %s" % msg
    def __str__(self):
        return self.msg
    def __unicode__(self):
        return self.msg

def main(argv=None): # IGNORE:C0111
    '''Command line options.'''
    
    if argv is None:
        argv = sys.argv
    else:
        sys.argv.extend(argv)

    program_name = os.path.basename(sys.argv[0])
    program_version = "v%s" % __version__
    program_build_date = str(__updated__)
    program_version_message = '%%(prog)s %s (%s)' % (program_version, program_build_date)
    program_shortdesc = __import__('__main__').__doc__.split("\n")[1]
    program_license = '''%s

  Created by Liam Deacon on %s.
  Copyright 2013 Liam Deacon. All rights reserved.
  
  Licensed under the Apache License 2.0
  http://www.apache.org/licenses/LICENSE-2.0
  
  Distributed on an "AS IS" basis without warranties
  or conditions of any kind, either express or implied.

USAGE
''' % (program_shortdesc, str(__date__))

    try:
        # Setup argument parser
        parser = ArgumentParser(description=program_license, formatter_class=RawDescriptionHelpFormatter)
        parser.add_argument("-i", "--input", dest="input", metavar='input', help="csv list or directory for input files [default: %(default)s]")
        parser.add_argument("-o", "--output", dest="output", metavar='output', help="output directory for files [default: %(default)s]")
        parser.add_argument('-d','--duplicates', dest='duplicates', metavar='option', help='Set the policy for duplicate renamed files. Options are:\n'
                            '\t\'i\'\t- append an i to the end of duplicate values;\n'
                            '\t\'ui\'\t- append either an i or a u if over or under the expected value, respectively;\n'
                            '\t\'ignore\'\t- do nothing (duplicate values will not be written to disk)')
        parser.add_argument('-D','--delete', dest='delete', action='store_true', help='delete original files' )
        parser.add_argument('-e','--extension', dest='extension', action='store_true', help='append file extension to renamed file paths' )
        parser.add_argument("-E", "--exclude", dest="exclude", help="exclude paths matching this regex pattern. [default: %(default)s]", metavar="RE" )
        parser.add_argument('-f','--final', dest='final', metavar='value', help='final value for renamed sequence')
        parser.add_argument("-I", "--include", dest="include", metavar="RE", help="only include paths matching this regex pattern."
                            " Note: exclude is given preference over include. [default: %(default)s]")
        parser.add_argument('-s','--start', dest='start', metavar='value', help='start value for renamed sequence')
        parser.add_argument('-S','--suffix', dest='suffix', metavar='suffix', help='append suffix after renamed values')
        parser.add_argument('-P','--prefix', dest='prefix', metavar='prefix', help='Add prefix before renamed values rather than original filename')
        parser.add_argument('-p','--precision', dest='precision', metavar='precision', type=int, default=0, 
                            help='number of decimal places for renamed values')
        parser.add_argument("-v", "--verbose", dest="verbose", action="count", help="set verbosity level [default: %(default)s]")
        parser.add_argument('-V', '--version', action='version', version=program_version_message)
        
        # Process arguments
        args = parser.parse_args()
        
        verbose = args.verbose
        inpat = args.include
        expat = args.exclude
        output = args.output
        input = args.input
        startValue = float(args.start)
        endValue = float(args.final)
        delete = bool(args.delete)
        duplicates = str(args.duplicates)
        extension = bool(args.extension)
        if args.suffix is not None:
            suffix = str(args.suffix)
        else:
            suffix = None
        if args.prefix is not None:
            prefix = str(args.prefix)
        else:
            prefix = None
        precision = int(args.precision)
        
        if precision < 0:
            precision = 0 
        
        if inpat and expat and inpat == expat:
            raise CLIError("include and exclude pattern are equal! Nothing will be processed.")
        
        filenames = []
        if isinstance(input, list):
            for i in input:
                if os.path.isfile(i):
                    filenames.append(i)
                elif os.path.isdir(i):
                    excludes = glob.glob(ntpath.join(i,expat))
                    for f in glob.glob(ntpath.join(i,inpat)):
                        if f not in excludes:
                            filenames.append(f)
        else:
            if os.path.isfile(input):
                filenames.append(input)
            elif os.path.isdir(input):
                excludes = glob.glob(ntpath.join(input,expat))
                for f in glob.glob(ntpath.join(input,inpat)):
                    if f not in excludes:
                        filenames.append(f)
        
        rn = renameSequence(files=filenames, precision=precision, initial=startValue, final=endValue, 
                            duplicate_policy=duplicates, output_path=output, output_prefix=prefix, 
                            output_suffix=suffix, append_ext=extension, delete_original=delete )
        rn.rename()
        
    
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
    if TESTRUN:
        import doctest
        doctest.testmod()
    if PROFILE:
        import cProfile
        import pstats
        profile_filename = 'renameSequence_profile.txt'
        cProfile.run('main()', profile_filename)
        statsfile = open("profile_stats.txt", "wb")
        p = pstats.Stats(profile_filename, stream=statsfile)
        stats = p.strip_dirs().sort_stats('cumulative')
        stats.print_stats()
        statsfile.close()
        sys.exit(0)
    sys.exit(main())