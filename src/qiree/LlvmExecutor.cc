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
#include "detail/GlobalMapper.hh"

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
//! Generate a function name without a specialization suffix
#define QIREE_RT_FUNCTION(FUNC) quantum__rt__##FUNC

//! Generate a function name with a specialization suffix
#define QIREE_QIS_FUNCTION(FUNC, SUFFIX) quantum__qis__##FUNC##__##SUFFIX

//---------------------------------------------------------------------------//
//!@{
/*!
 * QIR function wrappers.
 *
 * NOTE: QIR "opaque pointers" are represented in LLVM IR as pointer types
 * (i.e. they're \c sizeof(void*) ) but our interfaces represent them as opaque
 * wrappers.
 */
void QIREE_QIS_FUNCTION(mz, body)(std::uintptr_t q, std::uintptr_t r)
{
    return q_interface_->mz(Qubit{q}, Result{r});
}

void QIREE_QIS_FUNCTION(h, body)(std::uintptr_t q)
{
    return q_interface_->h(Qubit{q});
}

void QIREE_QIS_FUNCTION(cnot, body)(std::uintptr_t q1, std::uintptr_t q2)
{
    return q_interface_->cnot(Qubit{q1}, Qubit{q2});
}

bool QIREE_QIS_FUNCTION(read_result, body)(std::uintptr_t r)
{
    return static_cast<bool>(q_interface_->read_result(Result{r}));
}

void QIREE_RT_FUNCTION(array_record_output)(size_type s)
{
    return r_interface_->record_output(s);
}

void QIREE_RT_FUNCTION(result_record_output)(std::uintptr_t r,
                                             OptionalCString tag)
{
    return r_interface_->record_output(Result{r}, tag);
}

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
    LLVMInitializeNativeAsmParser();
    LLVMLinkInMCJIT();

    // Load LLVM IR file
    llvm::SMDiagnostic err;
    auto temp_mod = llvm::parseIRFile(filename, err, context());
    mod_ = temp_mod.get();

    if (!mod_)
    {
        err.print("qiree", llvm::errs());
        QIREE_VALIDATE(mod_,
                       << "failed to read QIR input at '" << filename << "'");
    }

    entrypoint_ = mod_->getFunction(entrypoint);
    QIREE_VALIDATE(entrypoint_,
                   << "no entrypoint function '" << entrypoint << "' exists");

    // Create execution engine by capturing the module
    ee_ = [&temp_mod] {
        llvm::EngineBuilder builder{std::move(temp_mod)};

        // Pass a reference to a string for diagnosing errors
        std::string err_str;
        builder.setErrorStr(&err_str);

        // Create the builder, or throw an exception with the failure
        std::unique_ptr<llvm::ExecutionEngine> ee{builder.create()};
        QIREE_VALIDATE(ee, << "failed to create execution engine: " << err_str);
        return ee;
    }();

    // Suppress symbol lookup in system dynamic libraries
    ee_->DisableSymbolSearching(true);

    // Add "lazy function creator" that just gives a more informative message
    ee_->InstallLazyFunctionCreator([](std::string const& s) -> void* {
        QIREE_VALIDATE(false, << "cannot call unknown function '" << s << "'");
    });

    // Bind functions if available
    detail::GlobalMapper bind_function(*mod_, ee_.get());
#define QIREE_BIND_RT_FUNCTION(FUNC) \
    bind_function("__quantum__rt__" #FUNC, QIREE_RT_FUNCTION(FUNC))
#define QIREE_BIND_QIS_FUNCTION(FUNC, SUFFIX)            \
    bind_function("__quantum__qis__" #FUNC "__" #SUFFIX, \
                  QIREE_QIS_FUNCTION(FUNC, SUFFIX))
    QIREE_BIND_QIS_FUNCTION(mz, body);
    QIREE_BIND_QIS_FUNCTION(h, body);
    QIREE_BIND_QIS_FUNCTION(cnot, body);
    QIREE_BIND_QIS_FUNCTION(read_result, body);
    QIREE_BIND_RT_FUNCTION(array_record_output);
    QIREE_BIND_RT_FUNCTION(result_record_output);
#undef QIREE_BIND_RT_FUNCTION
#undef QIREE_BIND_QIS_FUNCTION
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
    QIREE_EXPECT(ee_);

    QIREE_VALIDATE(!q_interface_ && !r_interface_,
                   << "cannot call LLVM executor recursively or in MT "
                      "environment (for now)");
    detail::EndGuard on_end_scope_([] {
        q_interface_ = nullptr;
        r_interface_ = nullptr;
    });
    q_interface_ = &qi;
    r_interface_ = &ri;

    // Execute the main function
    QIREE_ASSERT(entrypoint_);
    auto result = ee_->runFunction(entrypoint_, {});
    QIREE_DISCARD(result);
}

//---------------------------------------------------------------------------//
}  // namespace qiree
