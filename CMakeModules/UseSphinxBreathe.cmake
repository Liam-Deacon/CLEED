macro(add_sphinx_target TARGET_NAME BUILDER COMMENT_STR)
    add_custom_target(${TARGET_NAME}
    COMMAND sphinx-build -b ${BUILDER} . sphinx/build/${BUILDER}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/doc
        DEPENDS doxygen
        COMMENT ${COMMENT_STR}
    )

endmacro(add_sphinx_target)

find_package(Doxygen)
if(DOXYGEN_FOUND)

add_custom_target(doxygen
    COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_SOURCE_DIR}/doc/doxygen.conf
    COMMENT "Build doxygen xml files used by sphinx/breathe."
)


endif(DOXYGEN_FOUND)

add_sphinx_target(docs-html
    html
    "Build html documentation"
)

add_sphinx_target(docs-pdf
    pdflatex
    "Build pdf documentation"
)