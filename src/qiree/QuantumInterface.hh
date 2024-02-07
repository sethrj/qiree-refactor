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
 * Interface class for quantum instruction set in the \c qis (quantum)
 namespace.
 *
 * This class should subsume all known Quantum Instruction Set (QIS)
 * definitions. The QIR specification does not explicitly list these operations
 *
 * An instance of this class should be stateful (i.e. set up, store, and
 * measure the quantum circuit).
 *
 * \code
    ; Measurements
    void @__quantum__qis__mz__body(%Qubit*, %Result*)
    bool @__quantum__qis__read_result__body(%Result*)

    ; Gates
    void @__quantum__qis__cnot__body(%Qubit*, %Qubit*)
    void @__quantum__qis__h__body(%Qubit*)

 * \endcode
 */
class QuantumInterface
{
  public:
    //@{
    //! \name Measurements

    //! Read the value of a result.
    virtual QState read_result(Result) = 0;

    //! Measure the qubit and store in the result.
    virtual void mz(Qubit, Result) = 0;
    //@}
    //@{
    //! \name Gates

    //! Apply the H gate to the given qubit.
    virtual void h(Qubit) = 0;

    //! Apply the CNOT gate to the given qubits.
    virtual void cnot(Qubit, Qubit) = 0;
    //@}

  protected:
    virtual ~QuantumInterface() = default;
};

//---------------------------------------------------------------------------//
}  // namespace qiree
