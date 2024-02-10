//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other QIR-EE developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//---------------------------------------------------------------------------//
//! \file qirxacc/XaccQuantum.hh
//---------------------------------------------------------------------------//
#pragma once

#include <vector>

#include "qiree/Macros.hh"
#include "qiree/QuantumNotImpl.hh"
#include "qiree/RuntimeInterface.hh"
#include "qiree/Types.hh"

namespace xacc
{
class AcceleratorBuffer;
class Accelerator;
class IRProvider;
class CompositeInstruction;
}  // namespace xacc

namespace qiree
{
//---------------------------------------------------------------------------//
/*!
 * Translate instructions from QIR to XACC and execute them on read.
 */
class XaccQuantum final : virtual public QuantumNotImpl,
                          virtual public RuntimeInterface
{
  public:
    // Construct with accelerator name and number of shots
    XaccQuantum(std::string const& accel_name, size_type shots);

    // Call finalize when xacc is destroyed.
    ~XaccQuantum();

    QIREE_DELETE_COPY_MOVE(XaccQuantum);

    //!@{
    //! \name Accessors
    size_type num_results() const { return result_to_qubit_.size(); }
    size_type num_qubits() const { return num_qubits_; }
    //!@}

    //!@{
    //! \name Quantum interface
    // Prepare to build a quantum circuit for an entry point
    void set_up(EntryPointAttrs const&) override;

    // Complete an execution
    void tear_down() override;

    // Map a qubit to a result index
    void mz(Qubit, Result) final;

    // Read the value of a result.
    QState read_result(Result) final;
    //!@}

    //!@{
    //! \name Runtime interface
    // Initialize the execution environment, resetting qubits
    void initialize(OptionalCString env) override;

    // Execute the circuit and read outputs
    void array_record_output(size_type, OptionalCString tag) final;

    // Save one result
    void result_record_output(Result result, OptionalCString tag) final;

    // No one uses tuples??
    void tuple_record_output(size_type, OptionalCString) final;
    //!@}

  private:
    enum class Endianness
    {
        little,
        big
    };

    size_type num_qubits_{};
    std::vector<Qubit> result_to_qubit_;
    Endianness endian_;

    std::shared_ptr<xacc::AcceleratorBuffer> buffer_;
    std::shared_ptr<xacc::Accelerator> accelerator_;
    std::shared_ptr<xacc::IRProvider> provider_;
    std::shared_ptr<xacc::CompositeInstruction> ansatz_;
};

//---------------------------------------------------------------------------//
}  // namespace qiree
