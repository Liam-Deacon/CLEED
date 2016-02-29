# CLEED - Computational Low Energy Electron Diffraction
# Copyright (C) 1994-2016 Georg Held
# Copyright (C) 2013-2016 Liam Deacon
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

# Program information
SET(CLEED_VERSION_MAJOR "${CPACK_PACKAGE_VERSION_MAJOR}")
SET(CLEED_VERSION_MINOR "${CPACK_PACKAGE_VERSION_MINOR}")
SET(CLEED_VERSION_PATCH "${CPACK_PACKAGE_VERSION_PATCH}")
SET(CLEED_MAINTAINER "${CPACK_PACKAGE_VENDOR} <${CPACK_PACKAGE_CONTACT}>")
CONFIGURE_FILE(  "${PROJECT_SOURCE_DIR}/src/include/leed_ver.h.in"
    "${PROJECT_SOURCE_DIR}/src/include/leed_ver.h" IMMEDIATE @ONLY )

###############################################################################
# Sources
###############################################################################
SET (cleed_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/leed_nsym CACHE STRING "" FORCE)
SET (cleed_sym_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/leed_sym CACHE STRING "" FORCE)

# basic qm functions
SET (QMOBJ 
    ${cleed_SOURCE_DIR}/qmbessm.c
    ${cleed_SOURCE_DIR}/qmcgc.c
    ${cleed_SOURCE_DIR}/qmhank.c
    ${cleed_SOURCE_DIR}/qmylm.c
)

# matrix library
SET (MATOBJ 
    ${cleed_SOURCE_DIR}/matabs.c
    ${cleed_SOURCE_DIR}/matalloc.c
    ${cleed_SOURCE_DIR}/matarralloc.c
    ${cleed_SOURCE_DIR}/matarrfree.c
    #${cleed_SOURCE_DIR}/matcgau.c
    ${cleed_SOURCE_DIR}/matcheck.c
    ${cleed_SOURCE_DIR}/matclu.c
    ${cleed_SOURCE_DIR}/matcol.c
    ${cleed_SOURCE_DIR}/matconj.c
    ${cleed_SOURCE_DIR}/matcop.c
    ${cleed_SOURCE_DIR}/matdiff.c
    ${cleed_SOURCE_DIR}/matext.c
    ${cleed_SOURCE_DIR}/matfree.c
    ${cleed_SOURCE_DIR}/matins.c
    ${cleed_SOURCE_DIR}/matinv.c
    ${cleed_SOURCE_DIR}/matmul.c
    ${cleed_SOURCE_DIR}/matnattovht.c
    ${cleed_SOURCE_DIR}/matread.c
    ${cleed_SOURCE_DIR}/matrlu.c
    ${cleed_SOURCE_DIR}/matrow.c
    ${cleed_SOURCE_DIR}/matrm.c
    ${cleed_SOURCE_DIR}/matscal.c
    ${cleed_SOURCE_DIR}/matshow.c
    ${cleed_SOURCE_DIR}/matshowabs.c
    ${cleed_SOURCE_DIR}/matshowpar.c
    ${cleed_SOURCE_DIR}/matsqmod.c
    ${cleed_SOURCE_DIR}/mattrace.c
    ${cleed_SOURCE_DIR}/mattrans.c
    ${cleed_SOURCE_DIR}/matwrite.c
)

# input for LEED programs
SET (DEBOBJ ${cleed_SOURCE_DIR}/linpdebtemp.c)

SET (INPOBJ 
    ${cleed_SOURCE_DIR}/linpbullayer.c 
    ${cleed_SOURCE_DIR}/linpovllayer.c 
    ${cleed_SOURCE_DIR}/linpphasend.c  
    ${cleed_SOURCE_DIR}/linprdbulnd.c  
    ${cleed_SOURCE_DIR}/linprdpar.c    
    ${cleed_SOURCE_DIR}/linprdovlnd.c  
    ${cleed_SOURCE_DIR}/linpshowbop.c  
    ${cleed_SOURCE_DIR}/linpmatlm.c 
    # include lindebtemp.c
    ${DEBOBJ}
)

SET (INPOBJSYM
    ${cleed_sym_SOURCE_DIR}/linpbullaysym.c 
    ${cleed_sym_SOURCE_DIR}/linpovlaysym.c  
    ${cleed_sym_SOURCE_DIR}/linprdbulsym.c  
    ${cleed_sym_SOURCE_DIR}/linprdovlsym.c  
    ${cleed_sym_SOURCE_DIR}/linpphase.c     
    ${cleed_sym_SOURCE_DIR}/lwritepar.c     
    ${cleed_sym_SOURCE_DIR}/lreadpar.c
    ${cleed_sym_SOURCE_DIR}/lsymcheck.c
)

# output for LEED programs
SET (OUTOBJ 
    ${cleed_SOURCE_DIR}/loutbmlist.c 
    ${cleed_SOURCE_DIR}/louthead.c   
    ${cleed_SOURCE_DIR}/loutint.c 
)
          
SET (OUTOBJSYM
    ${cleed_sym_SOURCE_DIR}/loutintsym.c
)

# beams:
SET (BEAMOBJ 
    ${cleed_SOURCE_DIR}/lbmgen.c    
    ${cleed_SOURCE_DIR}/lbmselect.c 
    ${cleed_SOURCE_DIR}/lbmset.c
)

SET (BEAMOBJSYM 
    ${cleed_sym_SOURCE_DIR}/lbmgensym.c 
    ${cleed_sym_SOURCE_DIR}/lbmrotmat.c
)
             
# parameter control:
SET (PCOBJ 
    ${cleed_SOURCE_DIR}/lpcmktlnd.c 
    ${cleed_SOURCE_DIR}/lpctemtl.c 
    ${cleed_SOURCE_DIR}/lpcupdatend.c
)

SET (PCOBJSYM 
    ${cleed_sym_SOURCE_DIR}/lsymcheck.c
    ${cleed_sym_SOURCE_DIR}/lpcupdate.c
    ${cleed_sym_SOURCE_DIR}/lpcmktl.c
)

# layer doubling:
SET (LDOBJ 
    ${cleed_SOURCE_DIR}/lld2n.c      
    ${cleed_SOURCE_DIR}/lld2lay.c    
    ${cleed_SOURCE_DIR}/lld2layrpm.c 
    ${cleed_SOURCE_DIR}/lldpotstep.c 
    ${cleed_SOURCE_DIR}/lldpotstep0.c
)

# multiple scattering:
SET (MSOBJ 
    ${cleed_SOURCE_DIR}/lmsbravlnd.c  
    ${cleed_SOURCE_DIR}/lmscomplnd.c  
    ${cleed_SOURCE_DIR}/lmslsumii.c   
    ${cleed_SOURCE_DIR}/lmslsumij.c   
    ${cleed_SOURCE_DIR}/lmspartinv.c  
    ${cleed_SOURCE_DIR}/lmstmatii.c   
    ${cleed_SOURCE_DIR}/lmstmatndii.c 
    ${cleed_SOURCE_DIR}/lmstmatij.c   
    ${cleed_SOURCE_DIR}/lmsymat.c     
    ${cleed_SOURCE_DIR}/lmsymatset.c  
    ${cleed_SOURCE_DIR}/lmsymat.c    
    ${cleed_SOURCE_DIR}/lmsypy.c
)

SET (MSOBJSYM  
    ${cleed_sym_SOURCE_DIR}/lmscomplsym.c 
    ${cleed_sym_SOURCE_DIR}/lmscompksum.c 
    ${cleed_sym_SOURCE_DIR}/lmsbravlsym.c 
    ${cleed_sym_SOURCE_DIR}/lmsbravl.c    
    ${cleed_SOURCE_DIR}/lmscompl.c    
    ${cleed_sym_SOURCE_DIR}/lmsltok.c
)

SET (CPLOBJ ${cleed_SOURCE_DIR}/cribasfun.c)

SET (PRGOBJ 
    ${cleed_SOURCE_DIR}/prgcheck.c 
    ${cleed_SOURCE_DIR}/lhelp.c
)

IF (WIN32)
    LIST (APPEND PRGOBJ 
        ${cleed_SOURCE_DIR}/getrusage_win32.c 
    )
ENDIF (WIN32)
         
SET (PRGOBJSYM
    
)

SET (TMAOBJ 
    ${cleed_SOURCE_DIR}/lpccumtl.c 
    ${cleed_SOURCE_DIR}/lpcmkms.c
)

SET (LEEDOBJ 
    ${CPLOBJ}  
    ${QMOBJ}   
    ${MATOBJ}  
    ${INPOBJ}  
    ${PCOBJ}   
    ${PRGOBJ}  
    ${OUTOBJ}  
    ${BEAMOBJ} 
    ${LDOBJ}   
    ${MSOBJ}   
    ${TMAOBJ}
)

SET (SYMOBJ 
    ${MSOBJSYM} 
    ${OUTOBJSYM} 
    ${INPOBJSYM} 
    ${PCOBJSYM}
    ${PRGOBJSYM}
    ${BEAMOBJSYM}
)

# Specify list of sources for each target
IF (WITH_OPENCL STREQUAL "ON")
    SET (OPENCL_SRCS 
        ${cleed_SOURCE_DIR}/err_code.c
    )
ELSE()
    SET (OPENCL_SRCS "")
ENDIF (WITH_OPENCL STREQUAL "ON")

SET (leedlib_SRCS
    ${LEEDOBJ}
    ${SYMOBJ}
    ${OPENCL_SRCS}
)

SET (cleed_SRCS cleed.c)

###############################################################################
# INSTALL TARGETS
###############################################################################
ADD_LIBRARY(leed SHARED ${leedlib_SRCS})
ADD_LIBRARY(leedStatic STATIC ${leedlib_SRCS})

SET_TARGET_PROPERTIES(leedStatic PROPERTIES OUTPUT_NAME leed)

IF (WIN32)
    ENABLE_LANGUAGE(RC)
    
    # configure for windows icon
    CONFIGURE_FILE ("${cleed_SOURCE_DIR}/res/windows/cleed.rc.in"
        "${cleed_SOURCE_DIR}/res/windows/cleed.rc" IMMEDIATE @ONLY ) 
        
    SET (cleed_SRCS ${cleed_SRCS} res/windows/cleed.rc)
    
    ADD_EXECUTABLE(cleed ${cleed_SRCS})

    TARGET_LINK_LIBRARIES(leed ws2_32 psapi m)
    TARGET_LINK_LIBRARIES(leedStatic ws2_32 psapi m)
    TARGET_LINK_LIBRARIES(cleed ws2_32 psapi)
    
ELSE (WIN32)
    
    IF (UNIX)
    
        IF(APPLE)
        
            ADD_EXECUTABLE(cleed MACOSX_BUNDLE ${cleed_SRCS})
            INSTALL(TARGETS cleed BUNDLE DESTINATION ../Applications)
            
            TARGET_LINK_LIBRARIES(leed m)
            TARGET_LINK_LIBRARIES(leedStatic m)
            
        ELSE(APPLE)
        
            ADD_EXECUTABLE(cleed ${cleed_SRCS})  
            
            TARGET_LINK_LIBRARIES(leed m)
            TARGET_LINK_LIBRARIES(leedStatic m)
        ENDIF(APPLE)
        
    ENDIF(UNIX)
    
ENDIF (WIN32)

TARGET_LINK_LIBRARIES (cleed leed m)
IF (WITH_OPENCL STREQUAL "ON")
    TARGET_LINK_LIBRARIES(leed OpenCl)
    TARGET_LINK_LIBRARIES(leedStatic OpenCl)
    TARGET_LINK_LIBRARIES (cleed OpenCl)
ENDIF (WITH_OPENCL STREQUAL "ON")    


IF (WIN32)
    INSTALL (TARGETS leed cleed
        COMPONENT runtime
        RUNTIME DESTINATION bin 
        LIBRARY DESTINATION bin
    )
ELSE (WIN32)
    INSTALL (TARGETS leed cleed
        COMPONENT runtime
        RUNTIME DESTINATION bin 
        LIBRARY DESTINATION lib
    )
ENDIF (WIN32)
    
INSTALL (TARGETS leedStatic COMPONENT libraries ARCHIVE DESTINATION lib)
