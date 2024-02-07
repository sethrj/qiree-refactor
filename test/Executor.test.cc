//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other QIR-EE developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//---------------------------------------------------------------------------//
//! \file Executor.test.cc
//---------------------------------------------------------------------------//
#include "qiree/Executor.hh"

#include "QuantumTestImpl.hh"
#include "qiree/Module.hh"
#include "qiree_test.hh"

namespace qiree
{
namespace test
{
//---------------------------------------------------------------------------//

class ExecutorTest : public ::qiree::test::Test
{
  protected:
    void SetUp() override {}

    TestResult run(std::string const& filename);
};

//---------------------------------------------------------------------------//
TestResult ExecutorTest::run(std::string const& filename)
{
    Executor execute(Module(this->test_data_path(filename)));

    // Run with the test interface
    TestResult tr;
    QuantumTestImpl quantum_impl(&tr);
    ResultTestImpl result_impl(&tr);

    execute(quantum_impl, result_impl);
    return tr;
}

//---------------------------------------------------------------------------//
TEST_F(ExecutorTest, minimal)
{
    this->run("minimal.ll");
}

//---------------------------------------------------------------------------//
TEST_F(ExecutorTest, bell)
{
    this->run("bell.ll");
}

//---------------------------------------------------------------------------//
TEST_F(ExecutorTest, several_gates)
{
    this->run("pyqir_several_gates.ll");
}

//---------------------------------------------------------------------------//
}  // namespace test
}  // namespace qiree
