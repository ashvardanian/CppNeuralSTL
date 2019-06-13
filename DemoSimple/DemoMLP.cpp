//
//  mlp_tests.cpp
//  AILibsComparator
//
//  Created by Ashot Vardanian on 6/10/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#include "DemoMLP.hpp"
#include "STLAliasis.hpp"
#include "MLP.hpp"
#include "Network.hpp"
#include "Ranges.hpp"

#include <nlohmann/json.hpp>
#include <fstream>

using namespace av::neural;
using namespace av::opensource;

void func_xor(Weight const * const ins,
              Weight * const outs) {
    Weight a = ins[0], b = ins[1];
    Weight & r = outs[0];
    r = Bool(a) ^ Bool(b);
}

void func_trig(Weight const * const ins,
               Weight * const outs) {
    Weight x = ins[0];
    Weight & y = outs[0];
    y = (0.2 +
         0.4 * x * x +
         0.3 * x * sin(15 * x) +
         0.05 * cos(50 * x));
}

void func_spheric_to_cart(Weight const * const ins,
                          Weight * const outs) {
    Weight ro = ins[0], theta = ins[1], phi = ins[2];
    Weight & x = outs[0], & y = outs[1], & z = outs[2];
    x = ro * sin(theta) * cos(phi);
    y = ro * sin(theta) * sin(phi);
    z = ro * cos(theta);
}

template <typename func_t>
void fill(Matrix & in,
          Matrix & out,
          Dim count_samples,
          func_t func,
          Dim inputs_per_sample,
          Dim outputs_per_sample,
          Weight input_var_min = 0,
          Weight input_var_max = 1) {
    in
    .resize({ count_samples, inputs_per_sample })
    .randomize_uniform(input_var_min, input_var_max);
    
    out
    .resize({ count_samples, outputs_per_sample });
    
    for (Dim idx_sample = 0; idx_sample < count_samples; idx_sample ++) {
        auto start_ins = &in(idx_sample, 0);
        auto start_outs = &out(idx_sample, 0);
        func(start_ins, start_outs);
    }
}

void run_example_xor() {
    MLP model;
    Matrix inputs, outputs;
    model.redefine_shape({ 2, 2, 1 });
    fill(inputs, outputs, 20,
         func_xor, 2, 1);
    
    TrainableModel network { model };
    network.train(inputs, outputs, 1000);
}

void run_example_trig(Dim between_layer_size = 10,
                      Count epochs = 1000) {
    MLP model;
    model.redefine_shape({ 1, between_layer_size, 1 });
    Matrix inputs, outputs;
    auto const matrices_refill = [&] {
        fill(inputs, outputs, 200,
             func_trig, 1, 1);
    };
    
    matrices_refill();
    TrainableModel network { model };
    network.train(inputs, outputs, epochs);
    
    matrices_refill();
    auto const derr = network.test_error(inputs, outputs);
    std::cout << "final error is " << derr << std::endl;
}

void run_example_transform(std::initializer_list<Dim> layer_sizes,
                           Count epochs = 1000) {
    MLP model;
    Matrix inputs, outputs;
    model.redefine_shape(layer_sizes);
    auto const matrices_refill = [&] {
        fill(inputs, outputs, 2000,
             func_spheric_to_cart, 3, 3,
             -4, 4);
    };
    
    matrices_refill();
    TrainableModel network { model };
    network.train(inputs, outputs, epochs);
    
    matrices_refill();
    auto const derr = network.test_error(inputs, outputs);
    std::cout << "final error is " << derr << std::endl;
}

void TestMLP::run() {
    run_example_xor();
    
    run_example_trig(5, 500);
    run_example_trig(20, 2000);
    run_example_trig(50, 5000);
    
    run_example_transform({ 3, 3, 3, 3, 3, 3 }, 1000);
    run_example_transform({ 3, 4, 5, 4, 3 }, 2000);
}
