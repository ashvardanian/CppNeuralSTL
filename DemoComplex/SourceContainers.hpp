//
//  SourceContainers.hpp
//  SandboxAI
//
//  Created by Ashot Vardanian on 6/11/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#pragma once
#include <clang-c/Index.h>
#include "STLAliasis.hpp"

namespace av::opensource {
    
    using Stats = MapOrdered<Text, Count>;    
    
    enum class TokenCase {
        Unknown,
        single,
        CAP_CASE,
        camelCase,
        PascalCase,
        snake_case
    };
    
    struct SourceTextPart {
        enum class Purpose {
            Unknown,
            Code,
            Space,
            Comment,
            StringLiteral
        } purpose;
        TextView text;
    };
    
    struct SourceTokenized {
        Text text;
        Array<SourceTextPart> parts;
        Array<TextView> code_tokens;
        Array<TextView> lines;
    };
    
    struct FileMetadata {
        Text commit;
        FilePath path;
        TimePoint date;
    };
    
    struct FileParsed {
        Stats stats;
        SourceTokenized content;
        Set<FilePath> dependencies_direct;
    };
    
    struct DirectoryIndex {
        MapOrdered<FilePath, FileParsed> files;
        Stats stats;
    };
}
