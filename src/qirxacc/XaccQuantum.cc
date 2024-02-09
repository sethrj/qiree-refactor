//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other QIR-EE developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//---------------------------------------------------------------------------//
//! \file qirxacc/XaccQuantum.cc
//---------------------------------------------------------------------------//
#include "XaccQuantum.hh"

#include <iostream>

#include "qiree/Assert.hh"

namespace qiree
{
//---------------------------------------------------------------------------//
/*!
 * Construct with accelerator name and number of shots.
 */
XaccQuantum::XaccQuantum(std::string const& accel_name, size_type shots)
{
    QIREE_VALIDATE(shots > 0, << "invalid number of shots " << shots);

    xacc::Initialize();

    // Create accelerator
    accelerator_ = xacc::getAccelerator(accel_name);
    QIREE_VALIDATE(accelerator_, << "failed to create accelerator");
    accelerator_->updateConfiguration({{"shots", static_cast<int>(shots)}});
    // TODO: bit order is accelerator-dependent?
    bit_order_ = xacc::AcceleratorBuffer::BitOrder::LSB;

    // Create providers
    provider_ = xacc::getIRProvider("quantum");
}

//---------------------------------------------------------------------------//
/*!
 * Prepare to build a quantum circuit for an entry point.
 */
void XaccQuantum::set_up(EntryPointAttrs const& attrs)
{
    QIREE_EXPECT(!buffer_);
    QIREE_VALIDATE(attrs.required_num_qubits > 0,
                   << "input is not a quantum program");

    buffer_ = xacc::qalloc(attrs.required_num_qubits);
    ansatz_ = provider_->createComposite("quantum_circuit");
    result_to_qubit_.resize(attrs.required_num_results);
    num_qubits_ = attrs.required_num_qubits;
}

//---------------------------------------------------------------------------//
/*!
 * Complete an execution.
 */
void XaccQuantum::tear_down()
{
    ansatz_.reset();
    buffer_.reset();
}

//---------------------------------------------------------------------------//
/*!
 * Map a qubit to a result index.
 */
void XaccQuantum::mz(Qubit q, Result r)
{
    QIREE_EXPECT(q.value < this->num_qubits());
    QIREE_EXPECT(r.value < this->num_results());

    result_to_qubit_[r.value] = q;
}

//---------------------------------------------------------------------------//
/*!
 * Read the value of a result.
 */
QState XaccQuantum::read_result(Result)
{
    // ?????
    return QState::zero;
}

//---------------------------------------------------------------------------//
/*!
 * Execute the circuit and read outputs.
 */
void XaccQuantum::array_record_output(size_type, OptionalCString)
{
    accelerator_->execute(buffer_, ansatz_);
    buffer_->print();
}

//---------------------------------------------------------------------------//
/*!
 * Mark the start of an array and its size.
 */
void XaccQuantum::result_record_output(Result r, OptionalCString tag)
{
    QIREE_EXPECT(r.value < this->num_results());
    auto q = result_to_qubit_[r.value];

    // Get a map of string ("0" and "1" ???) -> int
    auto counts
        = buffer_->getMarginalCounts({static_cast<int>(q.value)}, bit_order_);
    std::cout << "qubit " << q.value << " experiment "
              << (tag ? tag : "<null>")
              << "\n"
                 "0 "
              << counts["0"]
              << "\n"
                 "1 "
              << counts["1"] << std::endl;
}

//---------------------------------------------------------------------------//
}  // namespace qiree
