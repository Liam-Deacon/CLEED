# UseSphinxBreathe.cmake - CMake module for Sphinx/Breathe/Doxygen integration
#
# This module provides targets for building documentation with Doxygen (for C API)
# and Sphinx (for user-facing documentation), integrated via Breathe.
#
# Targets provided:
#   doxygen      - Generates Doxygen XML output for Breathe consumption
#   docs-html    - Builds HTML documentation using Sphinx
#   docs-pdf     - Builds PDF documentation using Sphinx (requires LaTeX)
#
# Prerequisites:
#   - Doxygen must be installed for API documentation
#   - Graphviz (dot) is optional but recommended for diagrams
#   - Python with sphinx, breathe, and exhale packages

find_package(Doxygen OPTIONAL_COMPONENTS dot)

# Configure the Doxygen configuration file from template
if(EXISTS "${PROJECT_SOURCE_DIR}/doc/doxyfile.conf.in")
    # Set DOXYGEN_DOT_FOUND for template substitution
    if(DOXYGEN_DOT_FOUND)
        set(DOXYGEN_DOT_FOUND "YES")
    else()
        set(DOXYGEN_DOT_FOUND "NO")
        set(DOXYGEN_DOT_EXECUTABLE "")
    endif()

    configure_file(
        "${PROJECT_SOURCE_DIR}/doc/doxyfile.conf.in"
        "${PROJECT_BINARY_DIR}/doc/doxyfile.conf"
        @ONLY
    )
endif()

# Doxygen target for generating XML output
if(DOXYGEN_FOUND)
    add_custom_target(doxygen
        COMMAND ${DOXYGEN_EXECUTABLE} "${PROJECT_SOURCE_DIR}/doc/doxyfile.conf"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/doc"
        COMMENT "Generating API documentation with Doxygen (XML output for Breathe)"
        VERBATIM
    )

    # Create output directory for Doxygen
    add_custom_command(TARGET doxygen PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_SOURCE_DIR}/doc/_build/doxygen"
    )
else()
    message(STATUS "Doxygen not found - API documentation will not be generated")
endif()

# Macro for creating Sphinx documentation targets
macro(add_sphinx_target TARGET_NAME BUILDER COMMENT_STR)
    if(DOXYGEN_FOUND)
        # If Doxygen is available, make Sphinx depend on it
        add_custom_target(${TARGET_NAME}
            COMMAND sphinx-build -b ${BUILDER} . _build/${BUILDER}
            WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/doc"
            DEPENDS doxygen
            COMMENT "${COMMENT_STR}"
            VERBATIM
        )
    else()
        # Build Sphinx without Doxygen dependency (will have limited API docs)
        add_custom_target(${TARGET_NAME}
            COMMAND sphinx-build -b ${BUILDER} . _build/${BUILDER}
            WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/doc"
            COMMENT "${COMMENT_STR} (without API documentation)"
            VERBATIM
        )
    endif()
endmacro()

# Create Sphinx documentation targets
add_sphinx_target(docs-html
    html
    "Building HTML documentation with Sphinx"
)

add_sphinx_target(docs-pdf
    latex
    "Building PDF documentation with Sphinx/LaTeX"
)

# Convenience target to build all documentation
add_custom_target(docs
    DEPENDS docs-html
    COMMENT "Building all documentation"
)
