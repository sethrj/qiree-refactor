//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other QIR-EE developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//---------------------------------------------------------------------------//
//! \file LlvmExecutor.test.cc
//---------------------------------------------------------------------------//
#include "qiree/LlvmExecutor.hh"

#include "QuantumTestImpl.hh"
#include "qiree_test.hh"

namespace qiree
{
namespace test
{
//---------------------------------------------------------------------------//

class LlvmExecutorTest : public ::testing::Test
{
  protected:
    void SetUp() override {}

    static std::string test_data_path(std::string const& filename)
    {
        std::string result = qiree_source_dir;
        result += "/test/data/";
        result += filename;
        return result;
    }

    TestResult run(std::string const& filename);
};

//---------------------------------------------------------------------------//
TestResult LlvmExecutorTest::run(std::string const& filename)
{
    LlvmExecutor execute(test_data_path(filename));

    // Run with the test interface
    TestResult tr;
    QuantumTestImpl quantum_impl(&tr);
    ResultTestImpl result_impl(&tr);

    execute(quantum_impl, result_impl);
    return tr;
}

//---------------------------------------------------------------------------//
TEST_F(LlvmExecutorTest, minimal)
{
    this->run("minimal.ll");
}

//---------------------------------------------------------------------------//
TEST_F(LlvmExecutorTest, bell)
{
    this->run("bell.ll");
}

//---------------------------------------------------------------------------//
}  // namespace test
}  // namespace qiree
