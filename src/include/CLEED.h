/*********************************************************************
 *                      CLEED.h
 *
 *  Copyright 2013-2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *********************************************************************/

/**
 * \file
 * \brief Master include file for the CLEED project.
 * \author Liam Deacon <liam.m.deacon@gmail.com>
 *
 * Contains the main documentation for the CLEED project.
 *
 * \mainpage
 *
 *
 * \section intro Introduction
 *
 * CLEED is a computational package for Low Energy Electron Diffraction (LEED)
 * IV analysis. It fits experimental IV data curves with those simulated by
 * the  \c cleed program and a 'goodness' of fit is made with the R-Factor
 * program \c crfac. Optimisation of the input parameters is performed by
 * \c csearch.
 *
 * A (still rather incomplete) documentation of the program package can be
 * found in the :download:`manual <CLEED_Manual.pdf>`, but it is probably best
 * to send an email, with all input files, to <g.held@reading.ac.uk> when you
 * encounter any problems (it is very likely you will ...)
 *
 * \section install Installation
 *
 * In order to target as many different systems in the simplest way the
 * CLEED package has been  developed using `CMake <http://www.cmake.org/>`_ as
 * a dependency tracking and makefile generation program. CMake is
 * available on all major desktop platforms and a
 * `GUI <http://www.cmake.org/cmake/resources/software.html>`_ is also
 * available for Windows users.
 *
 * Use the following command in order to install CLEED for your system:
 *
 * \code
 * cd /path/to/cleed/source
 * cmake -G <generator-name> --build build/ -i
 * cd build/
 * <make> install
 * \endcode
 *
 * Where \c <generator-name> is the target platform e.g. "Unix Makefiles"
 * or "MinGW Makefiles". \c <make> is usually \c make (Unix/GNU Linux) or
 * \c mingw32-make (Windows). The \c -i option is voluntary, but is useful
 * if you wish to be lead through the setup in a step-by-step fashion.
 *
 * \note Windows users will require
 * `MinGW <http://sourceforge.net/projects/mingw/>`_ installed (other compiler
 * tool chains such as `MSVC <http://www.visualstudio.com/>`_ and
 * `Cygwin <http://cygwin.com/index.html>`_ have not been tested).
 *
 * If you are unable to compile the programs, pre-compiled packages are
 * also available at <https://bitbucket.org/cleed/cleed/downloads>.
 *
 *
 * \subsection step1 Step 1: Pre-requisites
 *
 * In order to compile and build the CLEED package from source a number of
 * libraries are required:
 *
 * - \a libcairo : (optional) needed for extra output formats for \c patt
 * - \a libtiff : needed for \c mkiv
 * - \a GNU Scientific Library : (optional) needed for open source versions of
 * \c cleed and \c search which replace the older Numerical Recipes routines
 * (and are faster too!)
 * - \a OpenCL : (optional) used for experimental GPGPU support, enabled
 * using \c -DUSE_OPENCL when compiling.
 * - \a OpenMP : (optional) used for parallel calculations using multiple CPU
 * cores, enabled using \c -D_USE_OPENMP when compiling.
 *
 *
 * \section getting_started Getting Started
 *
 * On other operating systems check the compatibility of the trigonometric
 * functions and data type used for real in real.h in the \c /include directory.
 *
 * We have had a few problems on OSs different from DEC alpha with the
 * symmetrised version of CLEED (\c cleed_sym ), therefore this is currently
 * not supported in this distribution, but
 * the non-symmetrised version should be running okay.
 *
 * A set of sample input files for Er/Si(111) (no expt. IV curves),
 * Benzene on Ru(0001) and O and Cu/Ni(111) can be found under \c /SIER ,
 * \c /RUBENZ ,  \c /NIO and \c /NICU in the \c /examples directory of
 * the CLEED installation.
 *
 * There are three files for each search (in addition to phase shifts
 * and experimental data) which have to be created by the user:
 *
 * 1. <bul_file> \c *.bul - Bulk geometry and non-geometric parameters
 * 2. <inp_file> \c *.inp - Specifies start geometry for the search and
 * search parameters (symmetry etc.)
 * 3. <ctr_file> \c *.ctr - Specifies correspondence between theoretical
 * and experimental IV curves.
 *
 * A detailed description of the file format can be found in the PDF manual
 * CLEED_Manual.pdf, but it is probably easiest to use one of the sample input
 * files and modify it. All other files are created by the programs.
 *
 * The search can be monitored through the \c *.log file ; at each point of the
 * search the current best fit geometries and IV curves can be found in
 * \c *.rmin and  \c *.pmin .
 *
 * These files are all described in the CLEED manual.
 *
 * The phase shift input must have the same format as for the
 * VanHove/Tong programs
 * <http://www.icts.hkbu.edu.hk/surfstructinfo/SurfStrucInfo_files/leed/
 * leedpack.html> with a first line added that contains the number of
 * energies \f$ n_{\textbox{eng}} \f$ and the maximum angular momentum
 * quantum number \f$ l_{\textbox{max}} \f$ . For each type of atoms there
 * must be a separate file in the directory \c /phase/ called \c <atom>.phs .
 * The directory is passed to the LEED program through the environment
 * variable \a CLEED_PHASE .
 *
 * The Python phaseshifts <https://pypi.python.org/pypi/phaseshifts>
 * package provides a simple script (\c phsh.py ) to generate the necessary
 * phase shifts from the CLEED  \c *.inp and \c *.bul files
 * (see the phaseshifts documentation for further information):
 *
 * \code
 * phsh.py -i *.inp -b *.bul
 * \endcode
 *
 * Before starting any program, three environment variables have to be set:
 *
 * - \a CLEED_PHASE : see above.
 * - \a CSEARCH_LEED : path to executable LEED-IV program, e.g. \c cleed_nsym
 * - \a CSEARCH_RFAC : path to executable R factor program, \c crfac )
 *
 * A shell script doing this can be found in \c /bin/set_env/
 * (again, the directory path of the parent directory must be changed
 * in this file). For users with a Python interpreter installed,
 * the script 'set_env.py' allows much more sophisticated setups,
 * for more details see:
 *
 * \code
 * set_env.py --help
 * \endcode
 *
 * The search is started by:
 *
 * \code
 * csearch -i <inp_file>
 * \endcode
 *
 * \c csearch is the master program that calls the LEED-IV program
 * (\c cleed_nsym or \c cleed_sym ) and the R-factor program (\c crfac ) to
 * calculate IV curves and R-factors for a given trial structure and
 * optimises the geometry parameters of the surface geometry in order to
 * minimise the R factor using the simplex method (other search methods are
 * currently tested).
 *
 * The LEED-IV program can be called outside the search using:
 *
 * \code
 * cleed_nsym -i <par_file> -b <bul_file> -o <res_file> > <out_file>
 * \endcode
 *
 * \c <par_file> is usually created by the search program. It contains only the
 * positions of the overlayer atoms that are optimised during the search.
 * An example is given in \c /examples/NIO/ . The LEED-IV program produces a
 * lot of control output (written to stdout) when compiled with #DEBUG or
 * #CONTROL defined. Therefore it is best to redirect stdout to a file using
 * the \c 1> command.
 *
 * It is best to check the LEED-IV program separately first, after you have
 * made any alterations to the input file.
 *
 * In order to test the R-factor program separately, call it by:
 *
 * \code
 * crfac -c <ctr_file> -t <output_file_from_cleed>
 * \endcode
 *
 * On Linux machines, for the search it is recommended to use the
 * \c nohup command together with \c & which runs the search in the
 * background:
 *
 * \code
 * nohup csearch -i <inp_file> > <out_file> 2> <err_file> &
 * \endcode
 *
 * In the NiO111_2x2O example each iteration takes about 90-100s
 * (for a 2.4 GHz Linux PC), the final R-factor after convergence should be
 * around 0.1322 and it takes around 183 iteration to get there from the start
 * geometry specified in \c Ni111_2x2O.inp (see the attached \c *.log file).
 *
 * The example in NICU leads to an R-factor of 0.0633 after 63 iterations
 * (\a ca 15s cpu time per iteration).
 *
 * All information necessary to restart the search at the current position
 * is stored in \c *.ver . If the search stops for any reason, e.g. because
 * it has reached the limit of iterations (currently 2000), it can be
 * restarted by:
 *
 * \code
 * nohup csearch -i <inp_file> -v <ver_file> 1> <out_file> 2> <err_file> &
 * \endcode
 *
 * GOOD LUCK
 * -- Georg Held --
 *
 *
 * \section license License
 *
 * \code
 *
 * CLEED - the Computation Low Energy Electron Diffraction package
 * Copyright (C) 1994-2014  Georg Held
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * \endcode
 *
 * This license applies to all the programs included with the distribution,
 * unless stated otherwise. Individual program copyright notices can be found
 * using the \c --version command line argument for each program.
 *
 * The philosophy behind releasing the CLEED package is to have
 * an open source set of programs for making Low Energy Electron Diffraction
 * I(V) analysis both available to everyone and easy to use in the hope
 * that we can build a thriving collaborative community, however the authors
 * ask that if you use CLEED to help publish work, then please acknowledge
 * the hard work that has gone into this by citing the reference(s) in
 * the \ref acknowledgements section - Thanks!
 *
 *
 * \section acknowledgements Acknowledgements
 *
 * The authors have released CLEED as an open source project, free to use and
 * modify by anyone. All the authors request in return is that if you publish
 * work in which CLEED has contributed, then please cite the following:
 * \todo "CLEED paper"
 *
 * Additionally, the \c mkiv program, if used to extract IV data, should be
 * cited using: "G. Held, S. Uremovic, C. Stellwagand & D. Menzel,
 * \a Rev. \a Sci. \a Instr. \b 67 (1996) 378."
 *
 *
 * \subsection authors Authors
 *
 * The primary author of the CLEED project is Georg Held
 * <g.held@reading.ac.uk>, however contributions & development has also been
 * undertaken by:
 *
 * - Liam Deacon <liam.m.deacon@gmail.com> : Windows port with Explorer
 * integration, CMake packaging, phaseshifts scripting, man pages,
 * Doxygen/Sphinx documentation)
 * - Zhasmina Zheleeva : angle of incidence code used in \c caoi_leed and \c
 * caoi_rfac
 * - Wolfgang Braun : symmetry code & CLEED manual
 * - Christian Stellwag <leed@iron.E20.physik.tu-muenchen.de> : \c mkiv code
 * development.
 *
 * If you wish to contribute to this project then please contact either
 * Georg Held <g.held@reading.ac.uk>, Liam Deacon <liam.m.deacon@gmail.com>
 * or Jacopo Ardini <j.ardini@pgr.reading.ac.uk>.
 *
 **/
 
#ifndef __CLEED_H__
#define __CLEED_H__

#include "leed.h"
#include "search.h"
#include "rfac.h"

#include "latt.h"
#include "patt.h"
#include "ftsmooth.h"

#endif /* __CLEED_H__ */
