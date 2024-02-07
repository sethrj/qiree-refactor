//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other QIR-EE developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//---------------------------------------------------------------------------//
//! \file Module.test.cc
//---------------------------------------------------------------------------//
#include "qiree/Module.hh"

#include "qiree_test.hh"
// #include "Module.test.hh"

namespace qiree
{
namespace test
{
//---------------------------------------------------------------------------//

class ModuleTest : public ::qiree::test::Test
{
  protected:
    void SetUp() override {}
};

//---------------------------------------------------------------------------//
TEST_F(ModuleTest, minimal)
{
    Module m(this->test_data_path("minimal.ll"), "main");
    EXPECT_TRUE(m);

    // Test move semantics
    Module other(std::move(m));
    EXPECT_FALSE(m);
    EXPECT_TRUE(other);
}

//---------------------------------------------------------------------------//
TEST_F(ModuleTest, bell)
{
    Module m(this->test_data_path("bell.ll"));
    EXPECT_TRUE(m);
}

//---------------------------------------------------------------------------//
}  // namespace test
}  // namespace qiree
