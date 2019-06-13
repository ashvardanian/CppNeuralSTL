//
//  losses.hpp
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
    struct CostBatched {
        static Weights values(Weights a, Weights const & b) {
            range::transform(a, b, a, &derived_t::value);
            return a;
        }
        static Weights derivs(Weights a, Weights const & b) {
            range::transform(a, b, a, &derived_t::deriv);
            return a;
        }
        static Matrix values(Matrix a, Matrix const & b) {
            range::transform(a.flat(), b.flat(), a.flat(), &derived_t::value);
            return a;
        }
        static Matrix derivs(Matrix a, Matrix const & b) {
            range::transform(a.flat(), b.flat(), a.flat(), &derived_t::deriv);
            return a;
        }
    };

    // Simple overview.
    // https://rdipietro.github.io/friendly-intro-to-cross-entropy-loss/
    struct CrossEntropy { };
    
    // The mean square error is a consequence of performing maximum log-likelihood
    // estimation over the conditional probability distribution of the output.
    // Maximizing the log likelohood is same as minimizing binary cross entropy
    // between the true data generating distribution and the distribution learned
    // by the model.
    struct MeanSquareError :
    public CostBatched<MeanSquareError> {
        static Weight value(Weight, Weight);
        static Weight deriv(Weight, Weight);
    };
    
    using L2 = MeanSquareError;

}
