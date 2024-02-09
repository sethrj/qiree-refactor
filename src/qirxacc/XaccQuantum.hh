//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other QIR-EE developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//---------------------------------------------------------------------------//
//! \file qirxacc/XaccQuantum.hh
//---------------------------------------------------------------------------//
#pragma once

#include <vector>
#include <xacc/xacc.hpp>

#include "qiree/Macros.hh"
#include "qiree/QuantumInterface.hh"
#include "qiree/ResultInterface.hh"
#include "qiree/Types.hh"

namespace qiree
{
//---------------------------------------------------------------------------//
/*!
 * Translate instructions from QIR to XACC and execute them on read.
 */
class XaccQuantum : public QuantumInterface, public ResultInterface
{
  public:
    // Construct with accelerator name and number of shots
    XaccQuantum(std::string const& accel_name, size_type shots);

    // Allow move, prevent copy
    XaccQuantum(XaccQuantum&&) = default;
    XaccQuantum& operator=(XaccQuantum&&) = default;
    XaccQuantum(XaccQuantum const&) = delete;
    XaccQuantum& operator=(XaccQuantum const&) = delete;

    //!@{
    //! \name Accessors
    size_type num_results() const { return result_to_qubit_.size(); }
    size_type num_qubits() const { return num_qubits_; }
    //!@}

    //!@{
    //! \name Quantum interface
    // Prepare to build a quantum circuit for an entry point
    void set_up(EntryPointAttrs const&) final;

    // Complete an execution
    void tear_down() final;

    // Map a qubit to a result index
    void mz(Qubit, Result) final;

    // Read the value of a result.
    QState read_result(Result) final;
    //!@}

    //!@{
    //! \name Result interface
    // Execute the circuit and read outputs
    void array_record_output(size_type, OptionalCString tag) final;

    // Save one result
    void result_record_output(Result result, OptionalCString tag) final;
    //!@}

  private:
    xacc::AcceleratorBuffer::BitOrder bit_order_{};
    size_type num_qubits_{};
    std::vector<Qubit> result_to_qubit_;

    std::shared_ptr<xacc::AcceleratorBuffer> buffer_;
    std::shared_ptr<xacc::Accelerator> accelerator_;
    std::shared_ptr<xacc::IRProvider> provider_;
    std::shared_ptr<xacc::CompositeInstruction> ansatz_;
};

//---------------------------------------------------------------------------//
}  // namespace qiree
