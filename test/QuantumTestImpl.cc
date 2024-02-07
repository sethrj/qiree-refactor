//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other QIR-EE developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//---------------------------------------------------------------------------//
//! \file QuantumTestImpl.cc
//---------------------------------------------------------------------------//
#include "QuantumTestImpl.hh"

#include <gtest/gtest.h>

#include "qiree/Assert.hh"

namespace qiree
{
namespace test
{
//---------------------------------------------------------------------------//
/*!
 * Construct with pointer to modifiable test result.
 */
QuantumTestImpl::QuantumTestImpl(TestResult* result) : tr_{result}
{
    QIREE_EXPECT(tr_);
}

//---------------------------------------------------------------------------//
/*!
 * Measure the qubit and store in the result.
 */
void QuantumTestImpl::mz(Qubit q, Result r)
{
    tr_->commands.push_back("mz");
}

//---------------------------------------------------------------------------//
/*!
 * Apply the H gate to the given qubit.
 */
void QuantumTestImpl::h(Qubit)
{
    tr_->commands.push_back("h");
}

//---------------------------------------------------------------------------//
/*!
 * Apply the CNOT gate to the given qubits.
 */
void QuantumTestImpl::cnot(Qubit, Qubit)
{
    tr_->commands.push_back("cnot");
}

//---------------------------------------------------------------------------//
/*!
 * Read the value of a result.
 */
QState QuantumTestImpl::read_result(Result)
{
    return QState::zero;
}

//---------------------------------------------------------------------------//
/*!
 * Construct with pointer to modifiable test result.
 */
ResultTestImpl::ResultTestImpl(TestResult* tr) : tr_{tr}
{
    QIREE_EXPECT(tr_);
}

//---------------------------------------------------------------------------//
/*!
 * Prepare to store N results.
 */
void ResultTestImpl::record_output(size_type s)
{
    ASSERT_GT(s, 0);
}

//---------------------------------------------------------------------------//
/*!
 * Mark the start of an array and its size.
 */
void ResultTestImpl::record_output(Result result, OptionalCString tag) {}

//---------------------------------------------------------------------------//
}  // namespace test
}  // namespace qiree
