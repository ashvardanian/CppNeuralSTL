//
//  mlp_pytorch.hpp
//  AILibsComparator
//
//  Created by Ashot Vardanian on 6/11/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#pragma once
#include "STLAliasis.hpp"
#include <torch/torch.h>

namespace av::opensource {
        
    struct PyTorchMLP :
    public torch::nn::Module {
        
        PyTorchMLP(unsigned int num_inputs,
                   bool recurrent = false,
                   unsigned int hidden_size = 64);
        
        std::vector<torch::Tensor> forward(torch::Tensor inputs,
                                           torch::Tensor hxs,
                                           torch::Tensor masks);
        
    private:
        torch::nn::Sequential actor { nullptr };
        torch::nn::Sequential critic { nullptr };
        torch::nn::Linear critic_linear { nullptr };
        // torch::nn::Rec
    };


    struct TestPyTorchMLP {
        void run_autograd();
        void run_procedural();
        void run();
    };
    
}
