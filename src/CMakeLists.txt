#---------------------------------*-CMake-*----------------------------------#
# Copyright 2023 UT-Battelle, LLC, and other QIR-EE developers.
# See the top-level COPYRIGHT file for details.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#----------------------------------------------------------------------------#

configure_file("qqir_version.h.in" "qqir_version.h" @ONLY)
configure_file("qqir_config.h.in" "qqir_config.h" @ONLY)

#----------------------------------------------------------------------------#
# LIBRARIES
#----------------------------------------------------------------------------#

add_library(qqir
  stub.cc
)
add_library(QIR-EE::qqir ALIAS qqir)
install(TARGETS qqir
  EXPORT qqir-targets
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
