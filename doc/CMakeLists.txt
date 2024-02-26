#----------------------------------*-CMake-*----------------------------------#
# Copyright 2024 UT-Battelle, LLC, and other QIR-EE developers.
# See the top-level COPYRIGHT file for details.
# SPDX-License-Identifier: (Apache-2.0 OR MIT)
# Derived from QIR-EE (v0.5.0-dev-21-gddb329e28)
#-----------------------------------------------------------------------------#

#-----------------------------------------------------------------------------#
# DEVELOPER MANUAL (Doxygen)
#-----------------------------------------------------------------------------#

# Use full version string instead of short version
set(DOXYGEN_PROJECT_NUMBER "${QIREE_VERSION_STRING}")

# File setup
set(DOXYGEN_EXCLUDE_PATTERNS
  "*/generated/*"
)
if(NOT QIREE_DEBUG)
  list(APPEND DOXYGEN_EXCLUDE_PATTERNS "*/detail/*")
endif()

set(DOXYGEN_FILE_PATTERNS
  "*.h"
  "*.hh"
  "*.cc"
  "*.cu"
  "*.md"
)
set(DOXYGEN_EXTENSION_MAPPING "cu=C++")
set(_DOXYGEN_EXTRA_SOURCE
  "${PROJECT_SOURCE_DIR}/README.md"
  "${PROJECT_BINARY_DIR}/include/qiree_config.h"
  "${PROJECT_BINARY_DIR}/include/qiree_version.h"
)
if((QIREE_DEBUG AND QIREE_BUILD_TESTS)
  OR QIREE_DOXYGEN_BUILD_TESTS)
  # Add documentation for QIR-EE test harnesses when debugging or for CI
  list(APPEND _DOXYGEN_TEST_SOURCE
    "${PROJECT_SOURCE_DIR}/test"
  )
  list(APPEND DOXYGEN_EXCLUDE_PATTERNS
    "*.test.cc" "*TestBase.cc"
  )
else()
  set(_DOXYGEN_TEST_SOURCE)
endif()

set(_doxy_version_hex 1) # TODO only if needed

# Documentation usage
set(DOXYGEN_FULL_PATH_NAMES NO)
set(DOXYGEN_MACRO_EXPANSION YES)
set(DOXYGEN_JAVADOC_AUTOBRIEF YES)
set(DOXYGEN_QT_AUTOBRIEF YES)
set(DOXYGEN_DISTRIBUTE_GROUP_DOC YES)
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${PROJECT_SOURCE_DIR}/README.md")
set(DOXYGEN_FORMULA_MACROFILE "${CMAKE_CURRENT_SOURCE_DIR}/_static/macros.tex")
set(DOXYGEN_EXTRA_PACKAGES "amsmath")
set(DOXYGEN_PREDEFINED "__DOXYGEN__=${_doxy_version_hex}")
set(DOXYGEN_USE_MATHJAX YES)

# Verbosity/warning levels
qiree_set_default(DOXYGEN_QUIET YES)
qiree_set_default(DOXYGEN_WARN_IF_UNDOCUMENTED NO)

# Output options
# set(DOXYGEN_PROJECT_LOGO
#   "${CMAKE_CURRENT_SOURCE_DIR}/_static/qiree-square.svg")
set(DOXYGEN_HTML_EXTRA_STYLESHEET
  "${CMAKE_CURRENT_SOURCE_DIR}/_static/doxygen-custom.css")
set(DOXYGEN_GENERATE_HTML YES)
set(DOXYGEN_HTML_OUTPUT "doxygen-html")
set(DOXYGEN_CREATE_SUBDIRS YES)
set(DOXYGEN_REFERENCES_LINK_SOURCE YES)

qiree_set_default(DOXYGEN_VERBATIM_HEADERS NO)
qiree_set_default(DOXYGEN_MAX_DOT_GRAPH_DEPTH 2)

doxygen_add_docs(doxygen
  "${PROJECT_SOURCE_DIR}/src"
  ${_DOXYGEN_EXTRA_SOURCE}
  ${_DOXYGEN_TEST_SOURCE}
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  COMMENT "Generating Doxygen to doc/doxygen-html/index.html"
)

#-----------------------------------------------------------------------------#
