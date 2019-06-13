//
//  activations.cpp
//  AILibsComparator
//
//  Created by Ashot Vardanian on 6/8/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#include "activations.hpp"
#include <cmath> // `exp`
#include <functional>

using namespace av::neural;

Weight Sigmoid::value(Weight v) {
    return 1.f / (1.f + std::exp(- v));
}
Weight Sigmoid::deriv(Weight v) {
    auto const sig = value(v);
    return sig * (1.f - sig);
}

Weight Tanh::value(Weight v) {
    auto const ev = std::exp(v);
    auto const emv = std::exp(- v);
    return (ev - emv) / (ev + emv);
}
Weight Tanh::deriv(Weight v) {
    auto const t = value(v);
    return 1.f - t * t;
}

Weight ReLU::value(Weight v) {
    return std::max(v, 0.f);
}
Weight ReLU::deriv(Weight v) {
    return (v <= 0 ? 0 : 1);
}

Weight ELU::value(Weight v) {
    return (v > 0 ? v : alpha * (std::exp(v) - 1.f));
}
Weight ELU::deriv(Weight v) {
    return (v > 0 ? 1 : alpha * (std::exp(v)));
}

Weight ReLULeaky::value(Weight v) {
    return (v > 0 ? v : alpha * v);
}
Weight ReLULeaky::deriv(Weight v) {
    return (v > 0 ? 1 : alpha);
}
