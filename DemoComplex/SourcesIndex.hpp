//
//  SourcesIndex.hpp
//  AILibsComparator
//
//  Created by Ashot Vardanian on 6/10/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#pragma once
#include "STLAliasis.hpp"
#include "SourceContainers.hpp"

namespace av::opensource {
    
    struct SourcesIndex {
        static SourcesIndex & AllLibraries();

        DirectoryIndex & find_repo(Text);
        Count add_file_to(DirectoryIndex &, FilePath const &, Bool recursive);
        Count add_library(FilePath const &);
        
        void pull_list_of_files(FilePath const &, Bool build = true);
        void pull_github_stats(FilePath const &);

        Bool read_stats_from_file(FilePath const &);
        Bool export_stats_to_file(FilePath const &) const;

    private:
        void build_index();
        void accumulate_file_stats_into_lib_stats();

        MapOrdered<Text, DirectoryIndex> libs;
    };
    
}
