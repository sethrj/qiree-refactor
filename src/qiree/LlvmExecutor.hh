//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other QIR-EE developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//---------------------------------------------------------------------------//
//! \file qiree/LlvmExecutor.hh
//---------------------------------------------------------------------------//
#pragma once

#include <memory>

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
    // Construct with a QIR input filename
    explicit LlvmExecutor(std::string const& filename);

    // Default destructor
    ~LlvmExecutor();

    QIREE_DELETE_COPY_MOVE(LlvmExecutor);

    // Execute with the given interface functions
    void operator()(QuantumInterface& qi, ResultInterface& ri) const;

  private:
    std::unique_ptr<llvm::Module> mod_;
    std::unique_ptr<llvm::ExecutionEngine> ee_;
    std::unique_ptr<llvm::Function> ifstmt_;
};

//---------------------------------------------------------------------------//
}  // namespace qiree
