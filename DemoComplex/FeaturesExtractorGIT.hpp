//
//  FeaturesExtractorGIT.hpp
//  AILibsComparator
//
//  Created by Ashot Vardanian on 6/12/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#pragma once
#include "SourcesIndex.hpp"

namespace av::opensource {
    
    // Extract the differences between the nearby commits.
    // -    The average number of added/changed/removed lines per file.
    // -    Total number of commits touching source files.
    struct FeaturesExtractorGIT {
        void extract_all_in(DirectoryIndex &);
    };
    
}
