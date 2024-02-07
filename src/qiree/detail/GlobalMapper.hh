//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other QIR-EE developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//---------------------------------------------------------------------------//
//! \file qiree/detail/GlobalMapper.hh
//---------------------------------------------------------------------------//
#pragma once

#include <type_traits>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>

#include "qiree/Assert.hh"

namespace qiree
{
namespace detail
{
//---------------------------------------------------------------------------//
/*!
 * Map IR functions to compiled functions.
 */
class GlobalMapper
{
  public:
    // Construct with module and engine
    inline GlobalMapper(llvm::Module const& mod, llvm::ExecutionEngine* ee);

    //! Map a symbol name to a compiled function pointer
    template<class F>
    void operator()(char const* name, F* func) const
    {
        static_assert(std::is_function_v<F>, "not a function");

        llvm::Function* irfunc = mod_.getFunction(name);
        if (!irfunc)
        {
            // Function isn't available in the module
            return;
        }
        this->check_func(*irfunc);
        // TODO: also add traits-based checking for function signature

        return ee_->addGlobalMapping(irfunc, reinterpret_cast<void*>(func));
    }

  private:
    llvm::Module const& mod_;
    llvm::ExecutionEngine* ee_;

    // Non-templated function checks
    inline void check_func(llvm::Function const& irfunc) const;
};

//---------------------------------------------------------------------------//
// INLINE DEFINITIONS
//---------------------------------------------------------------------------//
/*!
 * Construct with module and engine.
 */
GlobalMapper::GlobalMapper(llvm::Module const& mod, llvm::ExecutionEngine* ee)
    : mod_{mod}, ee_{ee}
{
    QIREE_EXPECT(ee_);
}

//---------------------------------------------------------------------------//
/*!
 * Perform non-templated safety checks on a function.
 */
void GlobalMapper::check_func(llvm::Function const& irfunc) const
{
    QIREE_VALIDATE(irfunc.empty(),
                   << "could not bind to already-defined function '"
                   << irfunc.getName().str() << "'");
}

//---------------------------------------------------------------------------//
}  // namespace detail
}  // namespace qiree
