//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other QIR-EE developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//---------------------------------------------------------------------------//
//! \file qirxacc/XaccQuantum.cc
//---------------------------------------------------------------------------//
#include "XaccQuantum.hh"

#include <algorithm>
#include <iostream>
#include <utility>
#include <xacc/xacc.hpp>

#include "qiree/Assert.hh"

using xacc::constants::pi;

namespace qiree
{
//---------------------------------------------------------------------------//
/*!
 * Call initialize explicitly with args.
 */
void XaccQuantum::initialize(std::vector<std::string> args)
{
    xacc::Initialize(std::move(args));
}

//---------------------------------------------------------------------------//
/*!
 * Construct with accelerator name and number of shots.
 */
XaccQuantum::XaccQuantum(std::ostream& os,
                         std::string const& accel_name,
                         size_type shots)
    : output_{os}
{
    QIREE_VALIDATE(shots > 0, << "invalid number of shots " << shots);

    if (!xacc::isInitialized())
    {
        // Perhaps XaccQuantum::initialize was already called?
        xacc::Initialize();
        // TODO: uninstall xacc signal handlers
    }
    QIREE_ASSERT(xacc::isInitialized());

    // Tell XACC to throw exceptions rather than calling std::exit
    xacc::setIsPyApi();

    // Create accelerator
    accelerator_ = xacc::getAccelerator(accel_name);
    QIREE_VALIDATE(accelerator_, << "failed to create accelerator");
    accelerator_->updateConfiguration({{"shots", static_cast<int>(shots)}});
    // TODO: bit order is accelerator-dependent?
    endian_ = Endianness::little;

    // Create providers
    provider_ = xacc::getIRProvider("quantum");
}

//---------------------------------------------------------------------------//
/*!
 * Construct with simulator.
 */
XaccQuantum::XaccQuantum(std::ostream& os) : XaccQuantum{os, "qsim", 1} {}

//---------------------------------------------------------------------------//
/*!
 * Call finalize when xacc is destroyed.
 */
XaccQuantum::~XaccQuantum()
{
    xacc::Finalize();
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
    cur_circuit_ = provider_->createComposite("quantum_circuit");
    result_to_qubit_.resize(attrs.required_num_results);
    num_qubits_ = attrs.required_num_qubits;
}

//---------------------------------------------------------------------------//
/*!
 * Complete an execution.
 */
void XaccQuantum::tear_down()
{
    cur_circuit_.reset();
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
 * Initialize the execution environment, resetting qubits.
 */
void XaccQuantum::initialize(OptionalCString env)
{
    if (env)
    {
        output_
            << "What's env for? No one knows! But here is some value: " << env
            << std::endl;
    }
}

//---------------------------------------------------------------------------//
/*!
 * Execute the circuit and read outputs.
 */
void XaccQuantum::array_record_output(size_type, OptionalCString)
{
    accelerator_->execute(buffer_, cur_circuit_);
    buffer_->print(output_);
}

//---------------------------------------------------------------------------//
/*!
 * Mark the start of an array and its size.
 */
void XaccQuantum::result_record_output(Result r, OptionalCString tag)
{
    QIREE_EXPECT(r.value < this->num_results());
    using BitOrder = xacc::AcceleratorBuffer::BitOrder;

    auto q = result_to_qubit_[r.value];

    // Get a map of string ("0" and "1" ???) -> int
    auto counts = buffer_->getMarginalCounts(
        {static_cast<int>(q.value)},
        endian_ == Endianness::little ? BitOrder::LSB : BitOrder::MSB);

    // Print the result
    output_ << "qubit " << q.value << " experiment " << (tag ? tag : "<null>")
            << ": {0: " << counts["0"] << ", 1: " << counts["1"] << '}'
            << std::endl;
}

//---------------------------------------------------------------------------//
/*!
 * No one uses tuples!
 */
void XaccQuantum::tuple_record_output(size_type, OptionalCString)
{
    QIREE_NOT_IMPLEMENTED("XaccQuantum::tuple_record_output");
}

//---------------------------------------------------------------------------//
// QUANTUM INSTRUCTION MAPPING
//---------------------------------------------------------------------------//
void XaccQuantum::ccx(Qubit q1, Qubit q2)
{
    this->add_instruction("CCX", {q1, q2});
}
void XaccQuantum::ccnot(Qubit q1, Qubit q2, Qubit q3)
{
    this->add_instruction("CCNOT", {q1, q2, q3});
}
void XaccQuantum::cnot(Qubit q1, Qubit q2)
{
    this->add_instruction("CNOT", {q1, q2});
}
void XaccQuantum::cx(Qubit q1, Qubit q2)
{
    this->add_instruction("CX", {q1, q2});
}
void XaccQuantum::cy(Qubit q1, Qubit q2)
{
    this->add_instruction("CY", {q1, q2});
}
void XaccQuantum::cz(Qubit q1, Qubit q2)
{
    this->add_instruction("CZ", {q1, q2});
}
void XaccQuantum::h(Qubit q)
{
    this->add_instruction("H", q);
}
void XaccQuantum::reset(Qubit q)
{
    this->add_instruction("Reset", q);
}
void XaccQuantum::rx(double angle, Qubit q)
{
    this->add_instruction("Rx", q, angle);
}
void XaccQuantum::ry(double angle, Qubit q)
{
    this->add_instruction("Ry", q, angle);
}
void XaccQuantum::rz(double angle, Qubit q)
{
    this->add_instruction("Rz", q, angle);
}
void XaccQuantum::rzz(double angle, Qubit q1, Qubit q2)
{
    this->add_instruction("RZZ", {q1, q2}, angle);
}
void XaccQuantum::s(Qubit q)
{
    return this->rz(pi / 2, q);
}
void XaccQuantum::s_adj(Qubit q)
{
    return this->rz(-pi / 2, q);
}
void XaccQuantum::swap(Qubit q1, Qubit q2)
{
    // compile swap operation into cnots
    // Dan: we should check if backend can directly implement SWAP first
    this->cnot(q1, q2);
    this->cnot(q2, q1);
    this->cnot(q1, q2);
}
void XaccQuantum::t(Qubit q)
{
    return this->rz(pi / 4, q);
}
void XaccQuantum::t_adj(Qubit q)
{
    return this->rz(-pi / 4, q);
}
void XaccQuantum::x(Qubit q)
{
    this->add_instruction("X", q);
}
void XaccQuantum::y(Qubit q)
{
    this->add_instruction("Y", q);
}
void XaccQuantum::z(Qubit q)
{
    this->add_instruction("Z", q);
}

//---------------------------------------------------------------------------//
// PRIVATE FUNCTIONS
//---------------------------------------------------------------------------//
/*!
 * Add an instruction with a single qubit.
 */
template<class... Ts>
void XaccQuantum::add_instruction(std::string s, Qubit q, Ts... args)
{
    return this->add_instruction(std::move(s), {q}, std::forward<Ts>(args)...);
}

//---------------------------------------------------------------------------//
/*!
 * Add an instruction with multiple qubits.
 */
template<class... Ts>
void XaccQuantum::add_instruction(std::string s,
                                  std::initializer_list<Qubit> qs,
                                  Ts... args)
{
    // Transform opaque qubit types into raw integer indices
    std::vector<std::size_t> q_indices(qs.size());
    std::transform(qs.begin(), qs.end(), q_indices.begin(), [this](Qubit q) {
        QIREE_EXPECT(q.value < this->num_qubits());
        return q.value;
    });

    // Create the instruction
    using VecInstr = std::vector<xacc::InstructionParameter>;
    auto instr = provider_->createInstruction(
        std::move(s), q_indices, VecInstr{std::forward<Ts>(args)...});

    // Add to current quantum circuit
    cur_circuit_->addInstruction(std::move(instr));
}

//---------------------------------------------------------------------------//
}  // namespace qiree