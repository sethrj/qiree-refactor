//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other QIR-EE developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//---------------------------------------------------------------------------//
//! \file QuantumTestImpl.hh
//---------------------------------------------------------------------------//
#pragma once

#include <string>
#include <vector>

#include "qiree/QuantumInterface.hh"
#include "qiree/ResultInterface.hh"

namespace qiree
{
namespace test
{
//---------------------------------------------------------------------------//
struct TestResult
{
    std::vector<std::string> commands;
};

//---------------------------------------------------------------------------//
/*!
 * Implementation of the quantum interface.
 */
class QuantumTestImpl final : public QuantumInterface
{
  public:
    // Construct with pointer to modifiable test result
    explicit QuantumTestImpl(TestResult* result);

    // Measure the qubit and store in the result.
    void mz(Qubit, Result) final;

    // Apply the H gate to the given qubit.
    void h(Qubit) final;

    // Apply the CNOT gate to the given qubits.
    void cnot(Qubit, Qubit) final;

    // Read the value of a result.
    QState read_result(Result) final;

  private:
    TestResult* tr_;
};

class ResultTestImpl final : public ResultInterface
{
  public:
    // Construct with pointer to modifiable test result
    explicit ResultTestImpl(TestResult* result);

    // Prepare to store N results.
    void record_output(size_type) final;

    // Mark the start of an array and its size.
    void record_output(Result result, OptionalCString tag) final;

  private:
    TestResult* tr_;
};

//---------------------------------------------------------------------------//
}  // namespace test
}  // namespace qiree
