#---------------------------------*-CMake-*----------------------------------#
# Copyright 2023 UT-Battelle, LLC, and other QIR-EE developers.
# See the top-level COPYRIGHT file for details.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#----------------------------------------------------------------------------#

file(TO_CMAKE_PATH "${PROJECT_SOURCE_DIR}" QIREE_SOURCE_DIR)
configure_file(qiree_test_config.h.in qiree_test_config.h @ONLY)

#---------------------------------------------------------------------------##
# LIBRARY
#---------------------------------------------------------------------------##
add_library(qiree_test_lib
  QuantumTestImpl.cc
  Test.cc
)
target_link_libraries(qiree_test_lib
  PUBLIC QIREE::qiree GTest::GTest
)
target_include_directories(qiree_test_lib
  PUBLIC "${PROJECT_BINARY_DIR}/test"
)

#---------------------------------------------------------------------------##
# MACROS
#---------------------------------------------------------------------------##
function(qiree_add_test name)
  add_executable(${name}_test
    ${name}.test.cc
  )
  target_link_libraries(${name}_test
    QIREE::qiree
    qiree_test_lib
    GTest::gtest_main
  )
  add_test(NAME test/${name} COMMAND "$<TARGET_FILE:${name}_test>")
endfunction()

#---------------------------------------------------------------------------##
# TESTS
#---------------------------------------------------------------------------##

qiree_add_test(Executor)
qiree_add_test(Module)

#---------------------------------------------------------------------------##
