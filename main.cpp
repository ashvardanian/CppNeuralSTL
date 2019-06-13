//
//  main.cpp
//  PyTorchTestsCPP
//
//  Created by Ashot Vardanian on 06/10/2018.
//  Copyright © 2018 Ashot Vardanian. All rights reserved.
//

#include "SourceAnalyzer.hpp"
#include "DemoMLP.hpp"

using namespace av::opensource;

int main(int argc, const char * argv[]) {
    
    TestMLP { }.run();
    
//    FilePath const stats_path = "/Users/ashvardanian/CodeMine/SandboxAI/AILibsComparator/stats_per_lib.csv";
//    SourcesIndex index;
//    if (!index.read_stats_from_file(stats_path)) {
//        try {
//            index.pull_github_stats("/Users/ashvardanian/Documents/AILibsTalk/stats_github.csv");
//            index.pull_list_of_files("/Users/ashvardanian/Documents/AILibsTalk/paths_code.txt");
//            index.export_stats_to_file(stats_path);
//        } catch (std::exception) {
//            index.export_stats_to_file(stats_path);
//        }
//    }
//
//    SourceAnalyzer analysis;
//    analysis.multi();

    return 0;
}
