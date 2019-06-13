//
//  MLP.cpp
//  AILibsComparator
//
//  Created by Ashot Vardanian on 6/8/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#include "MLP.hpp"
#include "activations.hpp"
#include "losses.hpp"
#include "range.hpp"
#include <array>
#include <iostream>

using namespace av::neural;
using func_activation = Sigmoid;
using func_loss = MeanSquareError;

#pragma mark - Shared

float Perceptron::estimate_error(Matrix const & Yhat, Matrix const & Y) {
    Matrix const error = func_loss::values(Yhat, Y);
    auto const total = ::range::accumulate(error.flat(), 0.f, std::plus<Weight> { });
    return total / Y.rows();
}

#pragma mark - Single Layer Perceptron

Matrix const & SLP::final_activations() const {
    return Ao;
}

void SLP::redefine_shape(std::array<Dim, 3> sizes) {
    Wh.resize({ sizes[0], sizes[1] }).randomize_uniform(0, 1);
    Wo.resize({ sizes[1], sizes[2] }).randomize_uniform(0, 1);
}

void SLP::propagate_forward(Matrix const & X) {
    // Hidden layer.
    Zh = X * Wh;
    Ah = func_activation::values(Zh);

    // Output layer.
    Zo = Ah * Wo;
    Ao = func_activation::values(Zo);
}

void SLP::propagate_backward(Matrix const & X, Matrix const & Y) {
    Matrix const & Yhat = final_activations();
    error_last = estimate_error(Yhat, Y);
    Matrix const error_deriv = func_loss::derivs(Yhat, Y);

    // Layer Error.
    Matrix const Eo = error_deriv.hadamar(func_activation::derivs(Zo));
    Matrix const Eh = (Eo * Wo.transposed()).hadamar(func_activation::derivs(Zh));

    // Cost derivative for weights.
    Matrix const dWo = Ah.transposed() * Eo;
    Matrix const dWh = X.transposed() * Eh;
    
    // Update weights.
    Wh = Wh - dWh * learning_rate;
    Wo = Wo - dWo * learning_rate;
}

#pragma mark - Multi Layer Perceptron

Matrix const & MLP::final_activations() const {
    return As.back();
}

void MLP::redefine_shape(std::vector<Dim> sizes) {
    Ws.resize(sizes.size() - 1);
    Zs.resize(sizes.size() - 1);
    As.resize(sizes.size() - 1);
    for (Count Idx = 1; Idx < sizes.size(); Idx ++) {
        Ws[Idx-1].resize({ sizes[Idx-1], sizes[Idx] }).randomize_uniform(0, 1);
    }
}

void MLP::propagate_forward(Matrix const & X) {
    for (int Idx = 0; Idx < Ws.size(); Idx ++) {
        auto & W = Ws[Idx];
        auto & Z = Zs[Idx];
        auto & A = As[Idx];
        if (Idx == 0) {
            Z = X * W;
            A = func_activation::values(Z);
        } else {
            Z = As[Idx - 1] * W;
            A = func_activation::values(Z);
        }
    }
}

void MLP::propagate_backward(Matrix const & X, Matrix const & Y) {
    Matrix const & Yhat = final_activations();
    error_last = estimate_error(Yhat, Y);
    Matrix const error_deriv = func_loss::derivs(Yhat, Y);
    Count const layers_count = Ws.size();
    std::vector<Matrix> Es { layers_count };
    
    for (int Idx = int(layers_count) - 1; Idx >= 0; Idx --) {
        Matrix const & Z = Zs[Idx];
        Matrix const dZ = func_activation::derivs(Z);
        // Layer Errors.
        Matrix & E = Es[Idx];
        if (Idx + 1 == Ws.size()) {
            E = error_deriv.hadamar(dZ);
        } else {
            E = (Es[Idx + 1] * Ws[Idx + 1].transposed()).hadamar(dZ);
        }
    }

    ::range::for_each(Idxs { layers_count }, [&](int Idx) {
        Matrix const & E = Es[Idx];
        // Cost derivative for weights.
        Matrix dW;
        if (Idx != 0) {
            dW = As[Idx-1].transposed() * E;
        } else {
            dW = X.transposed() * E;
        }
        // Update weights.
        Matrix & W = Ws[Idx];
        W = W - dW * learning_rate;
    });
}
