//
//  Helpers.cpp
//  AILibsComparator
//
//  Created by Ashot Vardanian on 6/12/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#include "Helpers.hpp"
#include "STLAliasis.hpp"
#include <fstream>

namespace os = av::opensource;
using namespace os;

Text os::parse_file(TextView name) {
    std::ifstream t(name.data());
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    return str;
}

Bool os::is_unknown(Count Idx) {
    return Idx == std::string_view::npos;
}

void os::expect(Bool const must_be_true, char const * msg) {
    if (!must_be_true)
        throw std::logic_error { msg };
}

Bool os::starts_with(TextView haystack, TextView needle) {
    if (haystack.size() < needle.size())
        return false;
    return haystack.substr(0, needle.size()) == needle;
}

void os::increment(Count & stat, Count delta) {
    stat += delta;
}
