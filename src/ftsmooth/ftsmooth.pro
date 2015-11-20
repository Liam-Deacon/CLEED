# ---------------------------------------------
#         ftsmooth qmake project file
# 
# author: Liam Deacon <liam.m.deacon@gmail.com>
#
# run 'qmake -project && make' to build 
# ---------------------------------------------

INCLUDEPATH += ../include

FTSMOOTH_SRCS = *.c
TEST_FILES = test_fts_offset_data.c \
             test_fts_print_data.c \
             test_fts_rm_neg_data.c \
             test_fts_trim_data.c
FTSMOOTH_SRCS -= $$TEST_FILES

SOURCES = $$FTSMOOTH_SRCS 
