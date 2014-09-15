# +-----------------------------------------------------------------------------+
# | $Id$                                                                        |
# +-----------------------------------------------------------------------------+
# |   Copyright (C) 2011                                                        |
# |   Marcel Loose (loose <at> astron.nl)                                       |
# |                                                                             |
# |   This program is free software; you can redistribute it and/or modify      |
# |   it under the terms of the GNU General Public License as published by      |
# |   the Free Software Foundation; either version 2 of the License, or         |
# |   (at your option) any later version.                                       |
# |                                                                             |
# |   This program is distributed in the hope that it will be useful,           |
# |   but WITHOUT ANY WARRANTY; without even the implied warranty of            |
# |   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             |
# |   GNU General Public License for more details.                              |
# |                                                                             |
# |   You should have received a copy of the GNU General Public License         |
# |   along with this program; if not, write to the                             |
# |   Free Software Foundation, Inc.,                                           |
# |   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                 |
# +-----------------------------------------------------------------------------+

## -----------------------------------------------------------------------------
## Macro to generate a Python interface module from one or more Fortran sources
##
## Usage: add_f2py_module(<module-name> <src1>..<srcN> DESTINATION <install-dir> COMPONENT <component>
##
macro (add_f2py_module _name)

  # Precondition check.
  if(NOT F2PY_EXECUTABLE)
    message(FATAL_ERROR "add_f2py_module: f2py executable is not available!")
  endif(NOT F2PY_EXECUTABLE)

  # Parse arguments.
  string(REGEX REPLACE ";?DESTINATION.*" "" _srcs "${ARGN}")
  string(REGEX MATCH "DESTINATION;.*" _dest_dir "${ARGN}")
  string(REGEX REPLACE "^DESTINATION;" "" _dest_dir "${_dest_dir}")

  # string(REGEX REPLACE ";?COMPONENT.*" "" _srcs "${ARGN}")
  # string(REGEX MATCH "COMPONENT;.*" _component "${ARGN}")
  # string(REGEX REPLACE "^COMPONENT;" "" _component "${_component}")
  
  # Sanity checks.
  if(_srcs MATCHES "^$")
    message(FATAL_ERROR "add_f2py_module: no source files specified")
  endif(_srcs MATCHES "^$")
  if(_dest_dir MATCHES "^$" OR _dest_dir MATCHES ";")
    message(FATAL_ERROR "add_f2py_module: destination directory invalid")
  endif(_dest_dir MATCHES "^$" OR _dest_dir MATCHES ";")
  # if(_component MATCHES "^$" OR _component MATCHES ";")
    # message(FATAL_ERROR "add_f2py_module: component is invalid")
  # endif(_component MATCHES "^$" OR _component MATCHES ";")

  # Get the compiler-id and map it to compiler vendor as used by f2py.
  # Currently, we only check for GNU, but this can easily be extended. 
  # Cache the result, so that we only need to check once.
  if(NOT F2PY_FCOMPILER)
    if(CMAKE_Fortran_COMPILER_ID MATCHES "GNU")
      if(CMAKE_Fortran_COMPILER_SUPPORTS_F90)
        set(_fcompiler "gnu95")
      else(CMAKE_Fortran_COMPILER_SUPPORTS_F90)
        set(_fcompiler "gnu")
      endif(CMAKE_Fortran_COMPILER_SUPPORTS_F90)
    else(CMAKE_Fortran_COMPILER_ID MATCHES "GNU")
      set(_fcompiler "F2PY_FCOMPILER-NOTFOUND")
    endif(CMAKE_Fortran_COMPILER_ID MATCHES "GNU")
    set(F2PY_FCOMPILER ${_fcompiler} CACHE STRING
      "F2PY: Fortran compiler type by vendor" FORCE)
    if(NOT F2PY_FCOMPILER)
      message(STATUS "[F2PY]: Could not determine Fortran compiler type. "
                     "Troubles ahead!")
    endif(NOT F2PY_FCOMPILER)
  endif(NOT F2PY_FCOMPILER)

  # Set f2py compiler options: compiler vendor and path to Fortran77/90 compiler.
  if(F2PY_FCOMPILER)
    set(_fcompiler_opts "--fcompiler=${F2PY_FCOMPILER}")
    list(APPEND _fcompiler_opts "--f77exec=${CMAKE_Fortran_COMPILER}")
    if(CMAKE_Fortran_COMPILER_SUPPORTS_F90)
      list(APPEND _fcompiler_opts "--f90exec=${CMAKE_Fortran_COMPILER}")
    endif(CMAKE_Fortran_COMPILER_SUPPORTS_F90)
  endif(F2PY_FCOMPILER)

  # Make the source filenames absolute.
  set(_abs_srcs)
  foreach(_src ${_srcs})
    get_filename_component(_abs_src ${_src} ABSOLUTE)
    list(APPEND _abs_srcs ${_abs_src})
  endforeach(_src ${_srcs})

  # Get a list of the include directories.
  # The f2py --include_paths option, used when generating a signature file,
  # needs a colon-separated list. The f2py -I option, used when compiling
  # the sources, must be repeated for every include directory.
  get_directory_property(_inc_dirs INCLUDE_DIRECTORIES)
  string(REPLACE ";" ":" _inc_paths "${_inc_dirs}")
  set(_inc_opts)
  foreach(_dir ${_inc_dirs})
    list(APPEND _inc_opts "-I${_dir}")
  endforeach(_dir)

  # generate shared objects or DLLs depending on platform
  if (WIN32)
    set (_so ".pyd")
  else(WIN32)
    set (_so ".so")
  endif(WIN32)
  
  # Define the command to generate the Fortran to Python interface module. The
  # output will be a shared library that can be imported by python.
  add_custom_command(OUTPUT "${_name}${_so}"
    # COMMAND ${F2PY_EXECUTABLE} --quiet -m "${_name}" -h "${_name}.pyf"
            # --include_paths ${_inc_paths} --overwrite-signature "${_abs_srcs}"
    COMMAND ${F2PY_EXECUTABLE} --quiet -m "${_name}" -c "${_name}.f"
            ${_fcompiler_opts} ${_inc_opts} "${_abs_srcs}"
    DEPENDS "${_srcs}"
    COMMENT "[F2PY] Building Fortran to Python interface module ${_name}")

  # Add a custom target <name> to trigger the generation of the python module.
  add_custom_target("${_name}" ALL DEPENDS "${_name}${_so}")

  # Install the python module
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${_name}${_so}"
    DESTINATION "${_dest_dir}" COMPONENT runtime)

endmacro (add_f2py_module)