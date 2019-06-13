//
//  MLP.hpp
//  AILibsComparator
//
//  Created by Ashot Vardanian on 6/8/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#pragma once
#include "Matrix.hpp"
#include <iostream>

namespace av::neural {

    struct Perceptron {
        float learning_rate = 0.01;
        float error_min_needed = 0.001;
        float error_last = 0;

        static float estimate_error(Matrix const & Yhat, Matrix const & Y);
    };
    
    struct SLP :
    public Perceptron {
        Matrix const & final_activations() const;
        void redefine_shape(std::array<Dim, 3>);
        void propagate_forward(Matrix const & X);
        void propagate_backward(Matrix const & X, Matrix const & Y);
    private:
        Matrix Wh, Wo;
        Matrix Zh, Zo;
        Matrix Ah, Ao;
    };

    struct MLP :
    public Perceptron {
        Matrix const & final_activations() const;
        void redefine_shape(std::vector<Dim>);
        void propagate_forward(Matrix const & X);
        void propagate_backward(Matrix const & X, Matrix const & Y);
    private:
        std::vector<Matrix> Ws, Zs, As;
    };
    
    template <typename model_t>
    struct TrainableModel {
        
        TrainableModel(model_t & m) : model(m) { }
        
        Matrix predict(Matrix const & X) {
            model.propagate_forward(X);
            return model.final_activations();
        }
        
        float test_error(Matrix const & X, Matrix const & Y) {
            model.propagate_forward(X);
            return model.estimate_error(model.final_activations(), Y);
        }
        
        void train(Matrix const & xs, Matrix const & ys, Count epochs) {
            for (Count epoch = 0; epoch < epochs; epoch ++) {
                model.propagate_forward(xs);
                model.propagate_backward(xs, ys);
                
                if (epoch % 100 == 0)
                    std::cout << "Epoch =" << epoch
                    << ", Error =" << model.error_last << std::endl;
                if (model.error_last < model.error_min_needed)
                    return;
            }
        }
        
        model_t & model;
    };
        
}

namespace av::opensource {
    using Perceptron1 = neural::SLP;
    using PerceptronN = neural::MLP;
    template <typename TModel>
    using Network = neural::TrainableModel<TModel>;
}
