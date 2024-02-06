//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other QIR-EE developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//---------------------------------------------------------------------------//
//! \file qiree/LlvmExecutor.cc
//---------------------------------------------------------------------------//
#include "LlvmExecutor.hh"

#include <iostream>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/TargetSelect.h>

#include "Assert.hh"
#include "QuantumInterface.hh"
#include "ResultInterface.hh"
#include "detail/EndGuard.hh"

namespace qiree
{
namespace
{
//---------------------------------------------------------------------------//
/*!
 * Shared, thread-local LLVM context.
 *
 * This must exxceed the lifetime of any IR, modules, etc.
 */
llvm::LLVMContext& context()
{
    static llvm::LLVMContext ctx;
    return ctx;
}
//---------------------------------------------------------------------------//
/*!
 * Pointer to active interfaces.
 *
 * LLVM's addGlobalMapping requires a global function symbol rather than a
 * std::function.
 */
static QuantumInterface* q_interface_{nullptr};
static ResultInterface* r_interface_{nullptr};

//---------------------------------------------------------------------------//
}  // namespace

//---------------------------------------------------------------------------//
/*!
 * Construct with a QIR input filename.
 */
LlvmExecutor::LlvmExecutor(std::string const& filename,
                           std::string const& entrypoint)
{
    // Initialize LLVM
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    // Load LLVM IR file
    llvm::SMDiagnostic err;
    mod_ = llvm::parseIRFile(filename, err, context());

    if (!mod_)
    {
        err.print("qiree", llvm::errs());
        QIREE_VALIDATE(mod_,
                       << "failed to read QIR input at '" << filename << "'");
    }

    entrypoint_ = mod_->getFunction(entrypoint);
    QIREE_VALIDATE(entrypoint_,
                   << "no entrypoint function '" << entrypoint << "' exists");

    for (llvm::Function const& func : *mod_)
    {
        if (func.empty())
        {
            // No definition provided
            std::cout << "Empty function: " << func.getName().str()
                      << std::endl;
        }
    }
}

//---------------------------------------------------------------------------//
//! Default destructor
LlvmExecutor::~LlvmExecutor() = default;

//---------------------------------------------------------------------------//
/*!
 * Execute with the given interface functions.
 */
void LlvmExecutor::operator()(QuantumInterface& qi, ResultInterface& ri) const
{
    QIREE_VALIDATE(!q_interface_ && !r_interface_,
                   << "cannot call LLVM executor recursively or in MT "
                      "environment (for now)");
    detail::EndGuard on_end_scope_([] {
        q_interface_ = nullptr;
        r_interface_ = nullptr;
    });

    // Execute the main function
    QIREE_ASSERT(entrypoint_);
    auto result = ee_->runFunction(entrypoint_, {});
    QIREE_DISCARD(result);
}

//---------------------------------------------------------------------------//
}  // namespace qiree
