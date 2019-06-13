//
//  SourcesIndex.cpp
//  AILibsComparator
//
//  Created by Ashot Vardanian on 6/10/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#include "SourcesIndex.hpp"
#include "Helpers.hpp"
#include "Ranges.hpp"
#include "Matrix.hpp" // `Idxs`
#include "FeaturesExtractorAST.hpp"
#include "FeaturesExtractorGIT.hpp"
#include "FeaturesExtractorCPP.hpp"

#include <iostream>
#include <fmt/format.h>

using namespace av::opensource;

void SourcesIndex::build_index() {
    FeaturesExtractorCPP f_cpp;
    FeaturesExtractorAST f_ast;
    FeaturesExtractorGIT f_git;
    for (auto & [name, lib] : libs) {
        if (!lib.stats.empty())
            continue;
        try {
            f_cpp.extract_all_in(lib);
            // f_ast.extract_all_in(lib);
            // f_git.extract_all_in(lib);
        } catch (std::exception & ex) {
            
        }
    }
    accumulate_file_stats_into_lib_stats();
}

DirectoryIndex & SourcesIndex::find_repo(Text name) {
    range::transform(name, name, &tolower);
    return libs[name];
}


void SourcesIndex::pull_github_stats(FilePath const & path) {
    auto const libs_csv = parse_file(path);
    auto const libs_lines = str::split(libs_csv, '\n');
    Count const libs_count = libs_lines.size();
    expect(libs_count > 0, "Didn't read files!");
    
    // Here iare the names of the first columns in the file:
    // id,full_name,score,size,forks,forks_count,open_issues,open_issues_count,watchers,watchers_count,stargazers_count
    Array<Pair<Text, Count>> feature2column {
        { "github.score", 1 },
        { "github.size", 2 },
        { "github.forks", 3 },
        { "github.open_issues", 4 },
        { "github.watchers", 5 },
        { "github.stars", 6 }
    };

    Count total_params = 0;
    for (Count lib_idx = 0; lib_idx < libs_count; lib_idx ++) {
        if (lib_idx == 0)
            // We don't need the header.
            continue;
        
        auto const & lib_line = libs_lines[lib_idx];
        auto const lib_params = str::split(lib_line, ',');
        Text const repo_name { lib_params[0] };
        DirectoryIndex & repo_index = find_repo(repo_name);
        for (auto const & param : feature2column) {
            Text const num_str { lib_params[param.second] };
            repo_index.stats[param.first] = std::stoull(num_str);
            total_params ++;
        }
    }
}

void SourcesIndex::pull_list_of_files(FilePath const & path, Bool build) {
    auto const files_list = parse_file(path);
    auto const files_paths = str::split(files_list, '\n');
    Set<Text> unqie_libs_added;
    for (auto const & file_path : files_paths) {
        auto const path_components = str::split(file_path, '/');
        auto const lib_name = fmt::format("{}/{}", path_components[4], path_components[5]);
        find_repo(lib_name).files.insert_or_assign(Text { file_path }, FileParsed { });
        unqie_libs_added.insert(lib_name);
        
        if (unqie_libs_added.size() >= 10)
            return;
    }
    if (build)
        build_index();
}

Count SourcesIndex::add_file_to(DirectoryIndex & index,
                                FilePath const & path,
                                Bool recursive) {
    if (index.files.find(path) != index.files.end())
        return 0;
    index.files[path].content.text = parse_file(path);
    if (!recursive)
        return 1;
    
    Count paths_added = 0;
    while (true) {
        Set<TextView> files_wanted;
        for (auto & [p, info] : index.files) {
            FeaturesExtractorCPP::file_tokenize(info.content);
            auto const files_new = FeaturesExtractorCPP::get_included_filenames(info.content);
            files_wanted.insert(files_new.begin(), files_new.end());
            info.dependencies_direct.insert(files_new.begin(), files_new.end());
        }
        for (auto const & path_str : files_wanted)
            paths_added += add_file_to(index, FilePath { Text { path_str } }, recursive);
        if (!paths_added)
            break;
    }
    return paths_added;
}

void SourcesIndex::accumulate_file_stats_into_lib_stats() {
    for (auto & [name, lib] : libs) {
        for (auto const & [_, file] : lib.files) {
            for (auto const & [k, v] : file.stats) {
                if (lib.stats.find(k) == lib.stats.end()) {
                    lib.stats[k] = v;
                } else {
                    lib.stats[k] += v;
                }
            }
        }
    }
}

Bool SourcesIndex::read_stats_from_file(FilePath const &) {
    return false;
}

Bool SourcesIndex::export_stats_to_file(FilePath const &) const {
    return false;
}
