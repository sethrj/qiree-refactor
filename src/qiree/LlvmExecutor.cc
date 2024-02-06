//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other QIR-EE developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//---------------------------------------------------------------------------//
//! \file qiree/LlvmExecutor.cc
//---------------------------------------------------------------------------//
#include "LlvmExecutor.hh"

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/TargetSelect.h>

#include "Assert.hh"

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
    static thread_local llvm::LLVMContext ctx;
    return ctx;
}

//---------------------------------------------------------------------------//
}  // namespace

//---------------------------------------------------------------------------//
/*!
 * Construct with a QIR input filename.
 */
LlvmExecutor::LlvmExecutor(std::string const& filename)
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
}

//---------------------------------------------------------------------------//
//! Default destructor
LlvmExecutor::~LlvmExecutor() = default;

//---------------------------------------------------------------------------//
}  // namespace qiree
