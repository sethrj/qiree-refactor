#---------------------------------*-CMake-*----------------------------------#
# Copyright 2023 UT-Battelle, LLC, and other QIR-EE developers.
# See the top-level COPYRIGHT file for details.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#----------------------------------------------------------------------------#

file(TO_CMAKE_PATH "${PROJECT_SOURCE_DIR}" QIREE_SOURCE_DIR)
configure_file(qiree_test_config.h.in qiree_test_config.h @ONLY)

#---------------------------------------------------------------------------##

add_executable(qiree_test
  LlvmExecutor.test.cc
)
target_link_libraries(qiree_test
  GTest::GTest GTest::gtest_main # For testing
  QIREE::qiree # Code to be tested
)
target_include_directories(qiree_test
  PUBLIC "${PROJECT_BINARY_DIR}/test"
)

#---------------------------------------------------------------------------##
