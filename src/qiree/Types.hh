//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other QIR-EE developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//---------------------------------------------------------------------------//
//! \file qiree/Types.hh
//---------------------------------------------------------------------------//
#pragma once

#include <cstdint>
#include <type_traits>

namespace qiree
{
//---------------------------------------------------------------------------//
/*!
 * A type-safe wrapper class for managing "pointers".
 *
 * The opaque pointers in QIR are *indices* (or other opaque values) rather
 * than *addresses*:
 *  > Qubits are represented as pointers to an opaque LLVM structure type,
 *  > `%Qubit`. This is done so that qubit values may be distinguished from
 *  > other value types. It is not expected that qubit values actually be valid
 *  > memory addresses, and neither user code nor runtime code should ever
 *  > attempt to dereference a qubit value.
 */
template<class T>
struct OpaqueId
{
    std::uint64_t value{0};
};

//---------------------------------------------------------------------------//
// ENUMERATIONS
//---------------------------------------------------------------------------//
/*!
 * Measured quantum state.
 */
enum class QState : bool
{
    zero, //!< State is |0>
    one //!< State is |1>
};

//---------------------------------------------------------------------------//
/*!
 * Specalization of a "callable" function.
 *
 * This is the suffix for QIR LLVM function declarations.
 */
enum class CallableSpecialization
{
    body, //!< Base
    adj, //!< Adjoint
    ctl, //!< Controlled
    ctladj, //!< Controlled adjoint
};

//---------------------------------------------------------------------------//
// TYPE ALIASES
//---------------------------------------------------------------------------//

//! Integer size used for QIR
using size_type = std::uint64_t;

//! Opaque QIR array identifier
using Array = OpaqueId<struct Array_>;

//! Opaque QIR result identifier
using Result = OpaqueId<struct Result_>;

//! Opaque QIR tuple identifier
using Tuple = OpaqueId<struct Tuple_>;

//! Opaque QIR qubit identifier
using Qubit = OpaqueId<struct Qubit_>;

//! Pointer to a C string that may be null
using OptionalCString = char const*;

//! Tag dispatch class for a callable
template <CallableSpecialization CS>
using CallableTag = std::integral_constant<CallableSpecialization, CS>;

//!@{
//! \name Callable specialization tags
using BodyTag = CallableTag<CallableSpecialization::body>;
using AdjTag = CallableTag<CallableSpecialization::adj>;
using CtlTag = CallableTag<CallableSpecialization::ctl>;
using CtlAdjTag = CallableTag<CallableSpecialization::ctladj>;
//!@}

//---------------------------------------------------------------------------//
}  // namespace qiree
