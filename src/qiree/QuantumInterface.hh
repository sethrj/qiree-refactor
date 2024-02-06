//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other QIR-EE developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//---------------------------------------------------------------------------//
//! \file qiree/QuantumInterface.hh
//---------------------------------------------------------------------------//
#pragma once

#include "Types.hh"

namespace qiree
{
//---------------------------------------------------------------------------//
/*!
 * Interface class for the \c qis (quantum) namespace.
 *
 * \code
    void @__quantum__qis__mz__body(%Qubit*, %Result*)
    void @__quantum__qis__h__body(%Qubit*)
    void @__quantum__qis__cnot__body(%Qubit*, %Qubit*)
    i1 @__quantum__qis__read_result__body(%Result*)
 * \endcode
 */
class QuantumInterface
{
  public:
    //! Measure the qubit and store in the result.
    virtual void mz(Qubit, Result) = 0;

    //! Apply the H gate to the given qubit.
    virtual void h(Qubit) = 0;

    //! Apply the CNOT gate to the given qubits.
    virtual void cnot(Qubit, Qubit) = 0;

    //! Read the value of a result.
    virtual QState read_result(Result) = 0;

#if 0
    //! Apply the S gate to the given qubit.
    virtual void s(Qubit) = 0;

    //! Apply the adjoint S gate to the given qubit.
    virtual void s(AdjTag, Qubit, Qubit) = 0;
#endif

  protected:
    virtual ~QuantumInterface() = default;
};

//---------------------------------------------------------------------------//
}  // namespace qiree
