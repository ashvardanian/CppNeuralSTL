//
//  FeaturesExtractorAST.cpp
//  AILibsComparator
//
//  Created by Ashot Vardanian on 6/12/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#include "FeaturesExtractorAST.hpp"
#include "Helpers.hpp"

#include <clang-c/Index.h>
#include <iostream>
#include <fmt/format.h>

using namespace av::opensource;

TextView name_of_enclosing_class(CXCursor c) {
    while (!clang_Cursor_isNull(c)) {
        auto const kind = clang_getCursorKind(c);
        switch (kind) {
            case CXCursor_StructDecl:
            case CXCursor_UnionDecl:
            case CXCursor_ClassDecl:
            case CXCursor_EnumDecl:
            case CXCursor_ClassTemplate:
                return clang_getCString(clang_getCursorKindSpelling(kind));
                break;
            default:
                c = clang_getCursorSemanticParent(c);
                break;
        }
    }
    return { };
}

char const * ptr_for_location(CXSourceLocation loc, TextView text) {
    unsigned offset = 0;
    clang_getExpansionLocation(loc, nullptr, nullptr, nullptr, &offset);
    if (offset < text.size())
        return &text.at(offset);
    return nullptr;
}

TextView enclosing_range(CXCursor c, SourceTokenized const & info) {
    auto const source_range = clang_getCursorExtent(c);
    auto const ptr_start = ptr_for_location(clang_getRangeStart(source_range), info.text);
    auto const ptr_finish = ptr_for_location(clang_getRangeEnd(source_range), info.text);
    if (!ptr_start || !ptr_finish)
        return { };
    return { ptr_start, static_cast<Count>(ptr_finish - ptr_start) };
}

void extract_from_cursor_to_func(CXCursor c, FileParsed & info) {
    clang_CXXMethod_isVirtual(c);
    clang_CXXRecord_isAbstract(c);
    
    struct ContextInFunc {
        struct Level {
            TextView content;
            enum class Type {
                ForLoop,
                IfConditional
            } type;
        };
        Stack<Level> nested_level;
        struct MaxValueSearch {
            void increment() { current ++; maximum = std::max(current, maximum); }
            void decrement() { current --; }
            int get() const { return maximum; }
        private:
            int current = 0;
            int maximum = 0;
        };
        MaxValueSearch depth_conditional;
        MaxValueSearch depth_loops;
        int exit_points = 0;
        FileParsed * link_info = nullptr;
        
        void received(TextView part, Level::Type type) {
            if (part.empty())
                return;
            // Check if we are still in the old context.
            while (!nested_level.empty()) {
                auto const & last = nested_level.top();
                if (is_unknown(last.content.find(part))) {
                    if (last.type == Level::Type::ForLoop)
                        depth_loops.decrement();
                    else depth_conditional.decrement();
                    nested_level.pop();
                    
                }
            }
            // Add the new level.
            {
                nested_level.push({ part, type });
                if (type == Level::Type::ForLoop)
                    depth_loops.increment();
                else depth_conditional.increment();
            }
        }
    };

    ContextInFunc ctx;
    ctx.link_info = &info;
    clang_visitChildren(c, [](CXCursor c, CXCursor, CXClientData client_data) {
        ContextInFunc & ctx = *(ContextInFunc *)client_data;
        FileParsed & info = *ctx.link_info;
        auto const kind = clang_getCursorKind(c);
        if (kind == CXCursor_LambdaExpr) {
            extract_from_cursor_to_func(c, info);
            return CXChildVisit_Recurse;
        }
        
        auto const source_range = enclosing_range(c, info.content);
        // Once we are outside of that
        switch (kind) {
            case CXCursor_IfStmt:
            case CXCursor_SwitchStmt:
                ctx.received(source_range, ContextInFunc::Level::Type::IfConditional);
                break;
                
            case CXCursor_DoStmt:
            case CXCursor_WhileStmt:
                ctx.received(source_range, ContextInFunc::Level::Type::ForLoop);
                break;
                
            case CXCursor_BreakStmt:
            case CXCursor_ReturnStmt:
            case CXCursor_CXXThrowExpr:
            case CXCursor_GotoStmt:
                ctx.exit_points ++;
                break;
                
            default:
                break;
        }
        return CXChildVisit_Recurse;
    }, &ctx);
}

void extract_from_cursor(CXCursor c, FileParsed & info) {
    auto & stat = info.stats;
    auto const parent = clang_getCursorSemanticParent(c);
    auto const content_str = clang_getCString(clang_getCursorSpelling(c));
    auto const kind_str = clang_getCString(clang_getCursorKindSpelling(c.kind));
    auto const kind_parent_str = clang_getCString(clang_getCursorKindSpelling(parent.kind));
    auto const make_key = [=](char const * prefix) {
        return fmt::format("{}.{}", prefix, kind_str);
    };
    
    switch (c.kind) {
        case CXCursor_CXXMethod:
        case CXCursor_FunctionDecl:
        case CXCursor_LambdaExpr:
        case CXCursor_Constructor:
        case CXCursor_Destructor:
            increment(stat[make_key("functions.count")]);
            break;
            
        case CXCursor_StructDecl:
        case CXCursor_UnionDecl:
        case CXCursor_ClassDecl:
        case CXCursor_EnumDecl:
            increment(stat[make_key("types.count")]);
            break;
            
        case CXCursor_ClassTemplate:
        case CXCursor_FunctionTemplate:
            increment(stat[make_key("templates.count")]);
            break;
            
        case CXCursor_TemplateTypeParameter:
        case CXCursor_NonTypeTemplateParameter:
        case CXCursor_TemplateTemplateParameter:
            increment(stat[make_key("templates.parameters.count")]);
            break;
            
        case CXCursor_CXXStaticCastExpr:
        case CXCursor_CXXDynamicCastExpr:
        case CXCursor_CXXReinterpretCastExpr:
        case CXCursor_CXXConstCastExpr:
        case CXCursor_CXXFunctionalCastExpr:
            increment(stat[make_key("casts.count")]);
            break;
            
        case CXCursor_IndirectGotoStmt:
        case CXCursor_GotoStmt:
        case CXCursor_CXXThrowExpr:
        case CXCursor_CXXCatchStmt:
            increment(stat[make_key("flow.count")]);
            break;
            
        case CXCursor_CXXNewExpr:
        case CXCursor_CXXDeleteExpr:
            increment(stat[make_key("memory.count")]);
            break;
            
            // How to count static and global variables?
            
        default:
            break;
    }
    
    switch (c.kind) {
        case CXCursor_FunctionDecl:
        case CXCursor_FunctionTemplate:
        case CXCursor_LambdaExpr:
        case CXCursor_CXXMethod:
            extract_from_cursor_to_func(c, info);
            break;
        default:
            break;
    }
    
    switch (c.kind) {
        case CXCursor_FunctionDecl:
        case CXCursor_FunctionTemplate:
        case CXCursor_LambdaExpr:
        case CXCursor_CXXMethod:
        case CXCursor_VarDecl:
        case CXCursor_VariableRef:
        case CXCursor_MemberRef:
        case CXCursor_MemberRefExpr:
        case CXCursor_FieldDecl:
            // AnalyzeNameOf(c, stats);
            break;
        default:
            break;
    }
    
    auto const source_range = enclosing_range(c, info.content);
    std::cout << fmt::format("-- Cursor {} (in {}):{}\n{}\n",
                             kind_str, kind_parent_str, content_str, source_range);
}

#pragma mark - Primary API

void FeaturesExtractorAST::extract_all_in(DirectoryIndex & dir) {
    CXIndex index = clang_createIndex(false, true);
    MapOrdered<FilePath, CXTranslationUnit> units;
    
    for (auto & [p, info] : dir.files) {
        auto unit = clang_parseTranslationUnit(index, p.c_str(),
                                               nullptr, 0, nullptr, 0,
                                               CXTranslationUnit_None);
        if (!unit)
            std::cerr << fmt::format("Failed to parse {}.", p) << std::endl;
        units[p] = unit;
    }
    
    for (auto & [p, unit] : units) {
        auto & info = dir.files[p];
        CXCursor cursor = clang_getTranslationUnitCursor(unit);
        clang_visitChildren(cursor, [](CXCursor c, CXCursor parent, CXClientData client_data) {
            auto & info = *(FileParsed *)client_data;
            extract_from_cursor(c, info);
            return CXChildVisit_Recurse;
        }, &info);
    }
    
    for (auto & [_, unit] : units)
        clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);
}
