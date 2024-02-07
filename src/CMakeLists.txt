#---------------------------------*-CMake-*----------------------------------#
# Copyright 2023 UT-Battelle, LLC, and other QIR-EE developers.
# See the top-level COPYRIGHT file for details.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#----------------------------------------------------------------------------#

configure_file(
  "qiree_version.h.in"
  "${QIREE_HEADER_CONFIG_DIRECTORY}/qiree_version.h"
  @ONLY)
configure_file("qiree_config.h.in"
  "${QIREE_HEADER_CONFIG_DIRECTORY}/qiree_config.h"
  @ONLY)

#----------------------------------------------------------------------------#

llvm_map_components_to_libnames(_llvm_libs
  MCJIT
  native
)

#----------------------------------------------------------------------------#
# LIBRARIES
#----------------------------------------------------------------------------#

add_library(qiree
  qiree/Assert.cc
  qiree/Module.cc
  qiree/Executor.cc
  qiree/QuantumNotImpl.cc
)
target_link_libraries(qiree
  PRIVATE
    ${_llvm_libs}
)
target_include_directories(qiree
  PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
    "$<BUILD_INTERFACE:${QIREE_HEADER_CONFIG_DIRECTORY}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
  PRIVATE
    "${LLVM_INCLUDE_DIRS}"
)

add_library(QIREE::qiree ALIAS qiree)
install(TARGETS qiree
  EXPORT qiree-targets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    COMPONENT runtime
)

#----------------------------------------------------------------------------#
# HEADERS
#----------------------------------------------------------------------------#

# C++ source headers
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  COMPONENT development
  FILES_MATCHING REGEX ".*\\.hh?$"
)

#---------------------------------------------------------------------------##
