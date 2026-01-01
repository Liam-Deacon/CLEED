### automake file for cleed_nsym ###
# Process this file with automake to produce Makefile.in
lib_LIBRARIES = libleed.a

bin_PROGRAMS = cleed_nsym cleed

cleed_nsym_SOURCES = cleed_nsym.c

cleed_nsym_LTADD = libleed_la

cleed_SOURCES = cleed.c cleed_nsym.c ../leed_sym/cleed_sym.c
cleed_CPPFLAGS = -DCLEED_NO_MAIN
cleed_LTADD = libleed_la

if WIN32
bin_LTLIBRARY = libleed.la
else
lib_LTLIBRARY = libleed.la
endif

libsearch_la_SOURCES =              \
# basic quantum mechanics functions    
    qmbessm.c                       \
    qmcgc.c                         \
    qmhank.c                        \
    qmylm.c                         \
# matrix library
    matabs.c                        \
    matalloc.c                      \
    matarralloc.c                   \
    matarrfree.c                    \
    matcgau.c                       \
    matcheck.c                      \
    matclu.c                        \
    matcol.c                        \
    matconj.c                       \
    matcop.c                        \
    matdiff.c                       \
    matext.c                        \
    matfree.c                       \
    matins.c                        \
    matinv.c                        \
    matmul.c                        \
    matnattovht.c                   \
    matread.c                       \
    matrlu.c                        \
    matrow.c                        \
    matrm.c                         \
    matscal.c                       \
    matshow.c                       \
    matshowabs.c                    \
    matshowpar.c                    \
    matsqmod.c                      \
    mattrace.c                      \
    mattrans.c                      \
    matwrite.c                      \
# input for LEED programs    
    linpdebtemp.c                   \    
    linpbullayer.c                  \
    linpovllayer.c                  \
    linpphasend.c                   \
    linprdbulnd.c                   \
    linprdpar.c                     \
    linprdovlnd.c                   \
    linpshowbop.c                   \
    linpmatlm.c                     \
    ../leed_sym/linpbullaysym.c     \
    ../leed_sym/linpovlaysym.c      \
    ../leed_sym/linprdbulsym.c      \
    ../leed_sym/linprdovlsym.c      \
    ../leed_sym/linpphase.c         \     
    ../leed_sym/lwritepar.c         \     
    ../leed_sym/lreadpar.c          \
    ../leed_sym/lsymcheck.c         \
# output for LEED programs    
    loutbmlist.c                    \
    louthead.c                      \
    loutint.c                       \
    ../leed_sym/louthead2.c         \
    ../leed_sym/loutintsym.c        \
# beams    
    lbmgen.c                        \
    lbmselect.c                     \
    lbmset.c                        \
    ../leed_sym/lbmgensym.c         \
    ../leed_sym/lbmrotmat.c         \
# parameter control    
    lpcmktlnd.c                     \
    lpctemtl.c                      \
    lpcupdatend.c                   \
    ../leed_sym/lsymcheck.c         \
    ../leed_sym/lpcupdate.c         \
    ../leed_sym/lpcmktl.c           \
# layer doubling    
    lld2n.c                         \
    lld2lay.c                       \
    lld2layrpm.c                    \
    lldpotstep.c                    \
    lldpotstep0.c                   \
# multiple scattering    
    lmsbravlnd.c                    \
    lmscomplnd.c                    \
    lmslsumii.c                     \
    lmslsumij.c                     \
    lmspartinv.c                    \
    lmstmatii.c                     \
    lmstmatndii.c                   \
    lmstmatij.c                     \
    lmsymat.c                       \
    lmsymatset.c                    \
    lmsymmat.c                      \
    lmsypy.c                        \
    ../leed_sym/lmscomplsym.c       \
    ../leed_sym/lmscompksum.c       \
    ../leed_sym/lmsbravlsym.c       \
    ../leed_sym/lmsbravl.c          \    
    ../leed_sym/lmscompl.c          \    
    ../leed_sym/lmsltok.c           \
# program objects    
    cribasfun.c                     \
    prgcheck.c                      \
    lhelp.c                         \
    cleed_mode.c
    
libleed_a_SOURCES = $(libleed_la_SOURCES)
