//
//  FeaturesExtractorAST.hpp
//  AILibsComparator
//
//  Created by Ashot Vardanian on 6/12/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#pragma once
#include "SourcesIndex.hpp"

namespace av::opensource {
    
    // Build the Abstract Syntax Tree of the source file to
    // extract deeper structural properties of the code.
    // -    Cyclomatic complexity: number of nested loops.
    // -    Branching factors of functions.
    // -    Inlinability.
    // Materials for starting up:
    //      Compiling parts of LLVM and Clang on Mac.
    //      https://clang.llvm.org/docs/LibASTMatchersTutorial.html
    //      After you have compiled Clang, there is no need to pull all of LLVM.
    //      https://shaharmike.com/cpp/libclang/
    //      Don't use the raw AST parser, instead use this:
    //      https://github.com/foonathan/cppast
    struct FeaturesExtractorAST {
        void extract_all_in(DirectoryIndex &);
    };
    
}
