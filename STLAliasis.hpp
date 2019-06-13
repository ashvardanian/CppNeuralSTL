//
//  STLAliasis.hpp
//  SandboxAI
//
//  Created by Ashot Vardanian on 6/10/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#pragma once
#include <chrono>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <string_view>
#include <nlohmann/json.hpp>
#include "AhoCorasick.hpp"

namespace av::opensource {
    using FilePath = std::string;
    using TextView = std::string_view;
    using Text = std::string;
    using Clock = std::chrono::system_clock;
    using TimePoint = Clock::time_point;
    template <typename T>
    using Array = std::vector<T>;
    template <typename T1, typename T2>
    using Pair = std::pair<T1, T2>;
    template <typename T>
    using Stack = std::stack<T>;
    template <typename T>
    using Set = std::set<T>;
    template <typename Key, typename Value, typename Comp = std::less<>>
    using MapOrdered = std::map<Key, Value, Comp>;
    using Bool = bool;
    using Count = size_t;
    using JSObj = nlohmann::json;
    using Vocabulary = aho_corasick::trie;
}
