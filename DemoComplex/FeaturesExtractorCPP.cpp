//
//  FeaturesExtractorCPP.cpp
//  AILibsComparator
//
//  Created by Ashot Vardanian on 6/12/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#include "FeaturesExtractorCPP.hpp"
#include "Helpers.hpp"
#include "Ranges.hpp"

using namespace av::opensource;

#pragma mark - Constants

static Set<TextView> keywords_math {
    "&", "|", "*", ";", ":",
    "!", "+", "*", "/", "-", "~"
    // These can only be single digit.
    // "==", "!=", "+=", "*=", "/=", "-=", "&=", "|=", "&&",  "||",
};
static Set<TextView> keywords_cpp {
    "and", "and_eq", "asm", "auto", "bitand", "bitor", "bool", "break",
    "case", "catch", "char", "class", "compl", "const", "const_cast", "continue",
    "default", "delete", "do", "double", "dynamic_cast",
    "else", "enum", "explicit", "export", "extern",
    "false", "float", "for", "friend", "goto", "if", "inline", "int",
    "long", "mutable", "namespace", "new", "not", "not_eq",
    "operator", "or", "or_eq", "private", "protected", "public",
    "register", "reinterpret_cast", "return", "short",
    "signed", "sizeof", "static", "static_cast", "struct", "switch", "synchronized",
    "template", "this", "throw", "true", "try", "typedef", "typeid", "typename",
    "union", "unsigned", "using", "virtual", "void", "volatile",
    "wchar_t", "while", "xor", "xor_eq"
};
static Set<TextView> keywords_cpp11 {
    "alignas", "alignof", "noexcept", "nullptr", "thread_local",
    "static_assert", "constexpr", "decltype",
    "char16_t", "char32_t", "override", "final"
};
static Set<TextView> keywords_cpp_20 {
    "co_await", "co_return", "co_yield",
    "concept", "consteval", "requires",
    "audit", "axiom",
    "import", "module",
    "char8_t"
};
static Set<TextView> keywords_cpp23 {
    "atomic_cancel", "atomic_commit", "atomic_noexcept", "synchronized", "reflexpr",
    "transaction_safe", "transaction_safe_dynamic"
};
static Set<TextView> keywords_c {
    "auto", "break", "case", "char", "const", "continue", "default", "do", "double",
    "else", "enum", "extern", "float", "for", "goto", "if", "int", "long",
    "register", "return", "short", "signed", "sizeof", "static", "struct", "switch",
    "typedef", "union", "unsigned", "void", "volatile", "while" };
static Set<TextView> keywords_c99 {
    "inline", "restrict", "_Imaginary", "_Bool", "_Complex",
    // Convenience macros:
    "bool", "complex", "imaginary"
};
static Set<TextView> keywords_c11 {
    "_Alignas", "_Alignof", "_Atomic", "_Generic", "_Noreturn", "_Static_assert", "_Thread_local",
    // Convenience macros:
    "alignas", "alignof", "noreturn", "static_assert", "thread_local",
    "atomic_bool", "atomic_char", "atomic_schar", "atomic_uchar", "atomic_short", "atomic_ushort",
    "atomic_int", "atomic_uint", "atomic_long", "atomic_ulong", "atomic_llong", "atomic_ullong",
    "atomic_char16_t", "atomic_char32_t", "atomic_wchar_t"
};
static Set<TextView> keywords_preprocessor {
    "#if", "#elif", "#else", "#endif",
    "#defined", "#ifdef", "#ifndef", "#define", "#undef",
    "#include", "#line", "#error", "#pragma"
};
static Set<TextView> keywords_other {
    "exit", "assert",
    "unique_ptr", "auto_ptr", "shared_ptr", "weak_ptr", "scoped_ptr", "intrusive_ptr",
    "std::", "torch::", "tensorflow::", "tf::", "boost::", "folly::",
};
static Array<Pair<TextView, Set<TextView>>> keywords_and_purposes {
    { "complex_execution_flow", { "break", "continue", "throw", "goto", "catch", "try", "exit", "assert" } },
    { "reflections", { "sizeof", "auto", "decltype", "constexpr", "alignas", "alignof", "template" } },
    { "unsafe_casting", { "const_cast", "dynamic_cast", "reinterpret_cast" } },
    { "mutability", { "mutable", "const" } },
    { "memory_alignmnent", { "alignas", "alignof", "union", "&", "|", "~" } },
    { "memory_allocation", { "new", "delete" } },
    { "optimizations", { "inline", "noexcept", "asm" } },
    { "logic_literal", { "bit_and", "bit_or", "and", "and_eq", "or", "or_eq", "compl" } }
};

#pragma mark - Analysis

Bool contains(Vocabulary & vocab, Text const & word) {
    auto const results = vocab.parse_text(word);
    // Some of the substrings that we will find withing that string can be separate words.
    // We need to filter them out.
    return std::any_of(results.begin(), results.end(), [&](auto const & result) {
        return result.get_keyword().size() == word.size();
    });
}

void match_cpp_keywords() {
    Vocabulary trie;
    for (auto const & keywords : {
        keywords_c, keywords_c99, keywords_c11,
        keywords_cpp, keywords_cpp11, keywords_cpp23,
        keywords_math, keywords_other, keywords_preprocessor
    }) {
        for (TextView const & key : keywords) {
            trie.insert(Text { key });
        }
    }
}

TokenCase match_token_case(TextView name) {
    auto const count_underscore = std::count_if(name.begin(), name.end(), [](char c) {
        return c == '_';
    });
    auto const count_uppercase = std::count_if(name.begin(), name.end(), &isupper);
    auto const count_lowercase = std::count_if(name.begin(), name.end(), &islower);
    TokenCase t_case = TokenCase::Unknown;
    if (!count_underscore && !count_uppercase && count_lowercase) {
        t_case = TokenCase::single;
    } else if (count_underscore && !count_uppercase && count_lowercase) {
        t_case = TokenCase::snake_case;
    } else if (!count_underscore && count_uppercase && count_lowercase) {
        if (std::isupper(name.front())) {
            t_case = TokenCase::camelCase;
        } else {
            t_case = TokenCase::PascalCase;
        }
    } else if (count_uppercase && !count_lowercase) {
        t_case = TokenCase::CAP_CASE;
    }
    return t_case;
}

Array<TextView> split_token_into_lemmas(TextView name) {
    Array<TextView> lemmas;
    lemmas.push_back({ });
    auto const widen_last_to_fit = [&](char const * c_ptr) {
        TextView & lemma_last = lemmas.back();
        if (lemma_last.empty())
            lemma_last = TextView { c_ptr, 1 };
        else
            lemma_last = TextView {
                lemma_last.data(),
                static_cast<Count>(c_ptr - lemma_last.data())
            };
    };
    
    TextView name_remaining = name;
    while (!name_remaining.empty()) {
        auto const c = name_remaining.front();
        if (c == '_' || std::isdigit(c)) {
            // Do nothing.
        } else if (std::isupper(c)) {
            lemmas.push_back({ });
            widen_last_to_fit(name_remaining.data());
        } else {
            widen_last_to_fit(name_remaining.data());
        }
        name_remaining.remove_prefix(1);
    }
    return lemmas;
}

void analyze_token(TextView name,
                   Vocabulary & dict,
                   Stats & stat) {
    auto const t_parts = split_token_into_lemmas(name);
    range::for_each(t_parts, [&](TextView lemma) {
        Text lemma_str { lemma };
        range::transform(lemma_str, lemma_str, &tolower);
        if (contains(dict, lemma_str)) {
            increment(stat["token_lemma.defined"]);
        } else {
            increment(stat["token_lemma.unknown"]);
        }
    });
    
    auto const t_case = match_token_case(name);
    switch (t_case) {
        case TokenCase::single: increment(stat["token_case.single"]); break;
        case TokenCase::CAP_CASE: increment(stat["token_case.CAP_CASE"]); break;
        case TokenCase::camelCase: increment(stat["token_case.camelCase"]); break;
        case TokenCase::PascalCase: increment(stat["token_case.PascalCase"]); break;
        case TokenCase::snake_case: increment(stat["token_case.snake_case"]); break;
        default: increment(stat["token_case.unknown"]); break;
    }
}

void estimate_directory_consistency() { }

void estimate_depencies_distribution() { }

#pragma mark - API

FeaturesExtractorCPP::FeaturesExtractorCPP() {
    words_in_language;
    keywords_supported;
}

void FeaturesExtractorCPP::extract_all_in(DirectoryIndex & dir) {
    for (auto & [path, f] : dir.files) {
        FileParsed & file = f;
        file_tokenize(file.content);
        range::for_each(file.content.code_tokens, [&](TextView token) {
            analyze_token(token, words_in_language, file.stats);
        });
    }
}


#pragma mark - Public Tokenization Methods

void FeaturesExtractorCPP::file_tokenize(SourceTokenized & info) {
    info.lines = str::split(info.text, '\n');
    
    // Separate code comments and string literals.
    TextView text_remaining = info.text;
    while (!text_remaining.empty()) {
        Count const idx_string_literal = text_remaining.find("\"");
        Count const idx_comment_limited = text_remaining.find("/*");
        Count const idx_comment_line = text_remaining.find("//");
        Bool const has_anything_special = (is_unknown(idx_string_literal) &&
                                           is_unknown(idx_comment_limited) &&
                                           is_unknown(idx_comment_line));
        if (!has_anything_special) {
            info.parts.push_back({ SourceTextPart::Purpose::Code, text_remaining });
            return;
        }
        
        Count const idx_closest = std::min(idx_string_literal, std::min(idx_comment_limited, idx_comment_line));
        Bool const has_code_before_special = (idx_string_literal && idx_comment_limited && idx_comment_line);
        
        if (has_code_before_special) {
            info.parts.push_back({
                SourceTextPart::Purpose::Code,
                text_remaining.substr(0, idx_closest)
            });
            text_remaining = text_remaining.substr(idx_closest);
        }
        
        SourceTextPart::Purpose type_closest = SourceTextPart::Purpose::Unknown;
        Count length_of_closest = 0;
        if (idx_string_literal == idx_closest) {
            type_closest = SourceTextPart::Purpose::StringLiteral;
            length_of_closest = text_remaining.substr(1).find("\"");
            expect(!is_unknown(length_of_closest));
            length_of_closest += 2;
        } else if (idx_comment_limited == idx_closest) {
            type_closest = SourceTextPart::Purpose::Comment;
            length_of_closest = text_remaining.find("*/");
            expect(!is_unknown(length_of_closest));
            length_of_closest += 2;
        } else if (idx_comment_line == idx_closest) {
            type_closest = SourceTextPart::Purpose::Comment;
            length_of_closest = text_remaining.find("\n");
            expect(!is_unknown(length_of_closest));
            length_of_closest += 1;
        }
        
        expect(length_of_closest > 1);
        info.parts.push_back({ type_closest, text_remaining.substr(0, length_of_closest) });
        text_remaining = text_remaining.substr(length_of_closest);
    }
    
    // Separate code tokens.
    range::for_each(info.parts, [&](SourceTextPart const & part) {
        if (part.purpose != SourceTextPart::Purpose::Code)
            return;
        static Set<char> const special_delims {
            '[',']','{','}','<','>','(',')',
            '|','/','\\','-','*','&','+','^','?','!','~',
            '\'',';',':','.',',','\"'
        };
        auto const tokens = str::split(part.text, special_delims);
        range::for_each(tokens, [&](TextView token) {
            if (token.empty())
                return;
            info.code_tokens.push_back(token);
        });
    });
}

Array<TextView> FeaturesExtractorCPP::get_included_filenames(SourceTokenized const & info) {
    Array<TextView> result;
    range::for_each(get_preprocessor_macros(info), [&](TextView line) {
        TextView const needed_prefix { "include" };
        if (!starts_with(line, needed_prefix))
            return;
        line.remove_prefix(needed_prefix.size());
        auto const idx_path_start = line.find("\"");
        if (is_unknown(idx_path_start))
            return;
        line.remove_prefix(idx_path_start + 1);
        auto const idx_path_end = line.find("\"");
        if (is_unknown(idx_path_end))
            return;
        line = line.substr(0, idx_path_end);
        result.push_back(line);
    });
    return result;
}

Array<TextView> FeaturesExtractorCPP::get_preprocessor_macros(SourceTokenized const & info) {
    Array<TextView> result;
    range::for_each(info.lines, [&](TextView line) {
        auto line_active = str::trim(line, { '\n', ' ' });
        if (line_active.empty())
            return;
        if (line_active.front() == '#') {
            line_active.remove_prefix(1);
            line_active = str::trim(line, { '\n', ' ' });
            result.push_back(line_active);
        }
    });
    return result;
}
