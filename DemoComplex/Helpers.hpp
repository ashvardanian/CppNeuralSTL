//
//  Helpers.hpp
//  AILibsComparator
//
//  Created by Ashot Vardanian on 6/12/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#pragma once
#include "STLAliasis.hpp"
#include "SourceContainers.hpp"

namespace av::opensource {
    
    Text parse_file(TextView path);
    Bool is_unknown(Count Idx);
    void expect(Bool const must_be_true, char const * msg = nullptr);
    Bool starts_with(TextView haystack, TextView needle);
    void increment(Count & stat, Count delta = 1);
    
}
