#---------------------------------*-CMake-*----------------------------------#
# Copyright 2023 UT-Battelle, LLC, and other QQIR developers.
# See the top-level COPYRIGHT file for details.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#----------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.12)

# Set QQIR_VERSION using git tags using the following format
set(CGV_TAG_REGEX "v([0-9.]+)(-dev|-rc.[0-9]+)?")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/CgvFindVersion.cmake")
cgv_find_version(QQIR)

project(QQIR VERSION "${QQIR_VERSION}" LANGUAGES CXX)
cmake_policy(VERSION 3.12...3.22)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

#----------------------------------------------------------------------------#
# MAIN OPTIONS
#----------------------------------------------------------------------------#

# Components
option(QQIR_BUILD_TESTS "Build Queue-QIR unit tests" ON)

# Assertion handling
option(QQIR_DEBUG "Enable runtime assertions" ON)

#----------------------------------------------------------------------------#
# CMAKE INTRINSIC OPTIONS
#
# These are generally used to initialize properties on targets, and it's
# possible Queue-QIR is being built inside another project. Instead of saving
# these as cache variables (which change the defaults project-wide, including
# changing behavior of other code that was loaded before this project) set them
# as # local variables to be inherited underneath this directory.
#----------------------------------------------------------------------------#

macro(qqir_set_default name value)
  if(NOT DEFINED ${name})
    message(VERBOSE "QQIR set default ${name}=${value}")
    set(${name} "${value}")
  endif()
  list(APPEND QQIR_DEFAULT_VARIABLES ${name})
endmacro()

### Configuration ###
qqir_set_default(CMAKE_EXPORT_NO_PACKAGE_REGISTRY ON)
qqir_set_default(CMAKE_FIND_USE_PACKAGE_REGISTRY FALSE)
qqir_set_default(CMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY FALSE)

### Build flags ###
# Default to debug or released based on value of QQIR_DEBUG
if(DEFINED CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE)
  if(QQIR_DEBUG)
    set(_default_build_type "Debug")
  else()
    set(_default_build_type "Release")
  endif()
  set(CMAKE_BUILD_TYPE "${_default_build_type}" CACHE STRING "Build type" FORCE)
  message(STATUS "Set default CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
  unset(_default_build_type)
endif()
# Default to using C++17 everywhere
qqir_set_default(CMAKE_CXX_STANDARD 17)
qqir_set_default(CMAKE_CXX_EXTENSIONS OFF)

### Linking flags ###
# Default to building shared libraries (*not* a cache variable)
qqir_set_default(BUILD_SHARED_LIBS ON)
# Inform installed binaries of external library rpaths
qqir_set_default(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)
if(BUILD_SHARED_LIBS)
  # Inform installed binaries of internal library rpaths
  qqir_set_default(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}")
  # Do not relink libs/binaries when dependent shared libs change
  qqir_set_default(CMAKE_LINK_DEPENDS_NO_SHARED ON)
endif()

### Installation flags ###
# When developing add checking for proper usage of `install(`
if(QQIR_DEBUG)
  qqir_set_default(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION ON)
endif()
# Avoid printing details about already installed files
qqir_set_default(CMAKE_INSTALL_MESSAGE LAZY)

#----------------------------------------------------------------------------#
# UNUSED: Output locations for QQIR products
set(QQIR_CMAKE_CONFIG_DIRECTORY
  "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/cmake")
set(QQIR_HEADER_CONFIG_DIRECTORY
  "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_INCLUDEDIR}")
set(QQIR_LIBRARY_OUTPUT_DIRECTORY
  "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(QQIR_RUNTIME_OUTPUT_DIRECTORY
  "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")

#----------------------------------------------------------------------------#
# DEPENDENCIES
#----------------------------------------------------------------------------#

if(QQIR_BUILD_TESTS AND NOT GTest_FOUND)
  # TODO: download and build GTest as a subproject if not available
  find_package(GTest)
  if(NOT GTest_FOUND)
    message(SEND_ERROR
      "Googletest (GTest) is required for testing but was not found"
    )
  endif()
endif()

#----------------------------------------------------------------------------#
# LIBRARY
#----------------------------------------------------------------------------#

# Add the main libraries
add_subdirectory(src)

#----------------------------------------------------------------------------#
# UNIT TESTS
#----------------------------------------------------------------------------#

if(QQIR_BUILD_TESTS)
  include(CTest)
  add_subdirectory(test)
endif()

#----------------------------------------------------------------------------#
# APPLICATIONS AND BINARIES
#----------------------------------------------------------------------------#

# add_subdirectory(app)

#----------------------------------------------------------------------------#
# DOCUMENTATION
#----------------------------------------------------------------------------#

# if(QQIR_BUILD_DOCS)
#   add_subdirectory(doc)
# endif()

#----------------------------------------------------------------------------#
# CONFIG FILE INSTALLATION
#----------------------------------------------------------------------------#

# Where to install configured cmake files
set(QQIR_INSTALL_CMAKECONFIGDIR "${CMAKE_INSTALL_LIBDIR}/cmake/QQIR")

#TODO

install(FILES ${_cmake_files}
  DESTINATION "${QQIR_INSTALL_CMAKECONFIGDIR}"
  COMPONENT development
)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/cmake/backport"
  DESTINATION "${QQIR_INSTALL_CMAKECONFIGDIR}"
  COMPONENT development
)

# Export all cache variables that start with QQIR_
set(QQIR_EXPORT_VARIABLES)
macro(qqir_export_var varname)
  list(APPEND QQIR_EXPORT_VARIABLES "set(${varname} \"${${varname}}\")")
endmacro()
qqir_export_var(QQIR_VERSION_STRING)
list(APPEND QQIR_EXPORT_VARIABLES "\n# Configuration options")
get_directory_property(_cachevar_keys CACHE_VARIABLES)
foreach(_key IN LISTS _cachevar_keys)
  if(_key MATCHES "^QQIR_")
    qqir_export_var(${_key})
  endif()
endforeach()

# Export defaulted variables
list(APPEND QQIR_EXPORT_VARIABLES "\n# Defaulted variables")
foreach(_key IN LISTS QQIR_DEFAULT_VARIABLES)
  list(APPEND QQIR_EXPORT_VARIABLES "set(QQIR_${_key} \"${${_key}}\")")
endforeach()

# Add hints for direct dependencies and indirect geant dependencies
list(APPEND QQIR_EXPORT_VARIABLES "\n# Hints for upstream dependencies")
foreach(_key LLVM_DIR GTest_DIR)
  set(_val "${${_key}}")
  if(_val)
    list(APPEND QQIR_EXPORT_VARIABLES
      "if(NOT DEFINED ${_key})"
      "  set(${_key} \"${_val}\")"
      "endif()"
    )
  endif()
endforeach()
list(JOIN QQIR_EXPORT_VARIABLES "\n" QQIR_EXPORT_VARIABLES)

# Generate the file needed by downstream "find_package(QQIR)"
configure_file(
  "${PROJECT_SOURCE_DIR}/cmake/QQIRConfig.cmake.in"
  "${QQIR_CMAKE_CONFIG_DIRECTORY}/QQIRConfig.cmake"
  @ONLY
)

# Export version info
# TODO for version 1.0.0: change COMPATIBILITY to MajorVersion
write_basic_package_version_file(
  "${QQIR_CMAKE_CONFIG_DIRECTORY}/QQIRConfigVersion.cmake"
  COMPATIBILITY AnyNewerVersion
)

# Install generated config files
install(DIRECTORY "${QQIR_HEADER_CONFIG_DIRECTORY}/"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  COMPONENT development
  FILES_MATCHING REGEX ".*\\.hh?$"
)

# Install the config and version files
install(FILES
  "${QQIR_CMAKE_CONFIG_DIRECTORY}/QQIRConfig.cmake"
  "${QQIR_CMAKE_CONFIG_DIRECTORY}/QQIRConfigVersion.cmake"
  DESTINATION ${QQIR_INSTALL_CMAKECONFIGDIR}
  COMPONENT development
)

# Install 'QQIRTargets.cmake', included by QQIRConfig.cmake, which
# references the targets we install.
install(EXPORT qqir-targets
  FILE QQIRTargets.cmake
  NAMESPACE QQIR::
  DESTINATION "${QQIR_INSTALL_CMAKECONFIGDIR}"
  COMPONENT development
)

if(QQIR_VERSION VERSION_EQUAL "0.0.0")
  install(CODE "
message(WARNING \"The Queue-QIR version was not detected during configuration.
  (Check the beginning of your initial configure output for more details.)
  This will result in the installation having incorrect version metadata and
  will interfere with downstream CMake version requirements and may obscure
  provenance data in output results.\")
")
endif()


#----------------------------------------------------------------------------#
