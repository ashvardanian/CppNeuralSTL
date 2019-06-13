//
//  activations.hpp
//  AILibsComparator
//
//  Created by Ashot Vardanian on 6/8/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#pragma once
#include "Matrix.hpp"
#include "Ranges.hpp"

namespace av::neural {
    
    template <typename derived_t>
    struct ActivationBatched {
        static Weights values(Weights v) {
            range::transform(v, v, &derived_t::value);
            return v;
        }
        static Weights derivs(Weights v) {
            range::transform(v, v, &derived_t::deriv);
            return v;
        }
        static Matrix values(Matrix v) {
            range::transform(v.flat(), v.flat(), &derived_t::value);
            return v;
        }
        static Matrix derivs(Matrix v) {
            range::transform(v.flat(), v.flat(), &derived_t::deriv);
            return v;
        }
    };
    
    struct Sigmoid :
    public ActivationBatched<Sigmoid> {
        static Weight value(Weight);
        static Weight deriv(Weight);
    };
    
    struct Tanh :
    public ActivationBatched<Tanh> {
        static Weight value(Weight);
        static Weight deriv(Weight);
    };
    
    struct ReLU :
    public ActivationBatched<ReLU> {
        static Weight value(Weight);
        static Weight deriv(Weight);
    };
    
    struct ReLULeaky :
    public ActivationBatched<ReLULeaky> {
        static constexpr Weight alpha = 0.01;
        static Weight value(Weight);
        static Weight deriv(Weight);
    };
    
    struct ELU :
    public ActivationBatched<ELU> {
        static constexpr Weight alpha = 0.01;
        static Weight value(Weight);
        static Weight deriv(Weight);
    };

}
