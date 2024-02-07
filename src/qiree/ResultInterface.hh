//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other QIR-EE developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//---------------------------------------------------------------------------//
//! \file qiree/ResultInterface.hh
//---------------------------------------------------------------------------//
#pragma once

#include "Types.hh"

namespace qiree
{
//---------------------------------------------------------------------------//
/*!
 * Interface class for the \c rt (runtime) namespace for storing results.
 *
 * \code
   void @__quantum__rt__array_record_output(i64, i8*)
   void @__quantum__rt__result_record_output(%Result*, i8*)
   ; bool @__quantum__rt__result_equal(%Result*, %Result*)
   ; %Result* @__quantum__rt__result_get_one()
   ; %Result* @__quantum__rt__result_get_zero()
   ; %String* @__quantum__rt__result_to_string(%Result*)
   ; void @__quantum__rt__result_update_reference_count(%Result*, i32)
 * \endcode
 *
 * Typical usage:
 * \code
array_record_output(3, nullptr);
result_record_output(%Result* null, i8* null)
result_record_output(%Result* inttoptr (i64 1 to %Result*), i8* null)
result_record_output(%Result* inttoptr (i64 2 to %Result*), i8* null)
 * \endcode
 */

class ResultInterface
{
  public:
    //! Prepare to store N results.
    virtual void record_output(size_type) = 0;

    //! Mark the start of an array and its size.
    virtual void record_output(Result result, OptionalCString tag) = 0;

  protected:
    virtual ~ResultInterface() = default;
};

//---------------------------------------------------------------------------//
}  // namespace qiree
