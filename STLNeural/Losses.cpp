//
//  losses.cpp
//  AILibsComparator
//
//  Created by Ashot Vardanian on 6/8/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#include "losses.hpp"

using namespace av::neural;

Weight MeanSquareError::value(Weight predicted, Weight wanted) {
    auto const d = predicted - wanted;
    return d * d / 2.f;
}
Weight MeanSquareError::deriv(Weight predicted, Weight wanted) {
    return predicted - wanted;
}
