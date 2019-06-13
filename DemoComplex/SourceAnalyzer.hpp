//
//  main.hpp
//  SandboxAI
//
//  Created by Ashot Vardanian on 5/31/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#pragma once
#include "SourcesIndex.hpp"
#include "Matrix.hpp"

namespace av::opensource {
    
    struct SourceAnalyzer {
        Array<Text> features_names() const;
        Array<Text> properties_names() const;
        Matrix features_values() const;
        Matrix properties_values() const;
        
        void multi();
        void single();
        void pytorch();
    private:
        Matrix train, train_expectations;
        Matrix test, test_expectations;
    };
}
