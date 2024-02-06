//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other QIR-EE developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//---------------------------------------------------------------------------//
//! \file LlvmExecutor.test.cc
//---------------------------------------------------------------------------//
#include "qiree/LlvmExecutor.hh"

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
};

TEST_F(LlvmExecutorTest, bell)
{
    LlvmExecutor execute(test_data_path("bell.ll"));
}

//---------------------------------------------------------------------------//
}  // namespace test
}  // namespace qiree
