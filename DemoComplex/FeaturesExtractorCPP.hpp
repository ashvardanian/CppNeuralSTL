//
//  FeaturesExtractorCPP.hpp
//  AILibsComparator
//
//  Created by Ashot Vardanian on 6/12/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#pragma once
#include "SourcesIndex.hpp"

namespace av::opensource {
    
    struct FeaturesExtractorCPP {
        FeaturesExtractorCPP();
        void extract_all_in(DirectoryIndex &);
        
        static void file_tokenize(SourceTokenized &);
        static Array<TextView> get_preprocessor_macros(SourceTokenized const &);
        static Array<TextView> get_included_filenames(SourceTokenized const &);

    private:
        Vocabulary keywords_supported;
        Vocabulary words_in_language;
    };
    
}
