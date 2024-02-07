//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other QIR-EE developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//---------------------------------------------------------------------------//
//! \file qiree/LlvmExecutor.hh
//---------------------------------------------------------------------------//
#pragma once

#include <memory>
#include <string>

#include "Macros.hh"

namespace llvm
{
class Module;
class ExecutionEngine;
class Function;
}  // namespace llvm

namespace qiree
{
//---------------------------------------------------------------------------//
class QuantumInterface;
class ResultInterface;

//---------------------------------------------------------------------------//
/*!
 * Load a QIR file into an LLVM execution engine and run it.
 */
class LlvmExecutor
{
  public:
    // Construct with a QIR input filename and function name
    LlvmExecutor(std::string const& filename, std::string const& entrypoint);

    //! Construct with a QIR input filename to run "main" from
    explicit LlvmExecutor(std::string const& filename)
        : LlvmExecutor{filename, "main"}
    {
    }

    // Default destructor
    ~LlvmExecutor();

    QIREE_DELETE_COPY_MOVE(LlvmExecutor);

    // Execute with the given interface functions
    void operator()(QuantumInterface& qi, ResultInterface& ri) const;

  private:
    llvm::Function* entrypoint_{nullptr};
    llvm::Module* mod_{nullptr};
    std::unique_ptr<llvm::ExecutionEngine> ee_;
    std::unique_ptr<llvm::Function> ifstmt_;
};

//---------------------------------------------------------------------------//
}  // namespace qiree
