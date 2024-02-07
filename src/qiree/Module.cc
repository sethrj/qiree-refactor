//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other QIR-EE developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//---------------------------------------------------------------------------//
//! \file qiree/Module.cc
//---------------------------------------------------------------------------//
#include "Module.hh"

#include <llvm/IR/Attributes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>

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
    static llvm::LLVMContext ctx;
    return ctx;
}

//---------------------------------------------------------------------------//
/*!
 * Load an LLVM module from a file.
 */
std::unique_ptr<llvm::Module> load_llvm_module(std::string const& filename)
{
    llvm::SMDiagnostic err;
    auto module = llvm::parseIRFile(filename, err, context());
    if (!module)
    {
        err.print("qiree", llvm::errs());
        QIREE_VALIDATE(module,
                       << "failed to read QIR input at '" << filename << "'");
    }
    return module;
}

//---------------------------------------------------------------------------//
/*!
 * Find a function tagged with the QIR `entry_point`.
 */
llvm::Function* find_entry_point(llvm::Module& m)
{
    for (llvm::Function& f : m)
    {
        for (auto const& attr_set : f.getAttributes())
        {
            for (auto const& attr : attr_set)
            {
                if (attr.isStringAttribute())
                {
                    auto s = attr.getKindAsString();
                    if (s == "entry_point")
                    {
                        return &f;
                    }
                }
            }
        }
    }
    return nullptr;
}

//---------------------------------------------------------------------------//
}  // namespace

//---------------------------------------------------------------------------//
/*!
 * Construct with an LLVM IR file (bitcode or disassembled).
 */
Module::Module(std::string const& filename)
    : module_{load_llvm_module(filename)}
{
    QIREE_EXPECT(module_);

    // Search for entry point
    entrypoint_ = find_entry_point(*module_);
    QIREE_VALIDATE(entrypoint_,
                   << "no function with QIR 'entry_point' attribute "
                      "exists in '"
                   << filename << "'");
}

//---------------------------------------------------------------------------//
/*!
 * Construct with an LLVM IR file (bitcode or disassembled) and entry
 * point.
 */
Module::Module(std::string const& filename, std::string const& entrypoint)
    : module_{load_llvm_module(filename)}
{
    QIREE_EXPECT(module_);

    // Search for explicitly named entry point
    entrypoint_ = module_->getFunction(entrypoint);
    QIREE_VALIDATE(entrypoint_,
                   << "no entrypoint function '" << entrypoint << "' exists");
}

//---------------------------------------------------------------------------//
Module::~Module() = default;
Module::Module(Module&&) = default;
Module& Module::operator=(Module&&) = default;

//---------------------------------------------------------------------------//
/*!
 * Process entry point attributes.
 */
EntryPointAttrs Module::load_entry_point_attrs() const
{
    QIREE_NOT_IMPLEMENTED("entry point attributes");
}

//---------------------------------------------------------------------------//
/*!
 * Translate module attributes into flags.
 */
ModuleFlags Module::load_module_flags() const
{
    QIREE_NOT_IMPLEMENTED("module flags");
}

//---------------------------------------------------------------------------//
}  // namespace qiree
