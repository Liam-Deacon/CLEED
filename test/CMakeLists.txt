
# CLEED - Computational Low Energy Electron Diffraction
# Copyright (C) 1994-2014 Georg Held
# Copyright (C) 2013-2014 Liam Deacon
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

# Recursive CMake Source Subdirectories
SET (TEST_TARGETS "")

ADD_SUBDIRECTORY(test_ftsmooth)
ADD_SUBDIRECTORY(test_latt)
ADD_SUBDIRECTORY(test_leed)
ADD_SUBDIRECTORY(test_mkiv)
ADD_SUBDIRECTORY(test_patt)
ADD_SUBDIRECTORY(test_rfac)
ADD_SUBDIRECTORY(test_search)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
                  DEPENDS ${TEST_TARGETS})