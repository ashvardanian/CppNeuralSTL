//
//  stl_net.hpp
//  AILibsComparator
//
//  Created by Ashot Vardanian on 6/5/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#pragma once
#include "Matrix.hpp"
#include <vector>
#include <valarray>
#include <unordered_map>
#include <map>
#include <string_view>
#include <string>
#include <set>

namespace av::neural {
    
    struct Node;
    struct Network;
    using NodePtr = std::shared_ptr<Node>;
    using NodeLink = std::weak_ptr<Node>;
    
    struct Node :
    public std::enable_shared_from_this<Node> {
        using connection = std::weak_ptr<Node>;
        static constexpr std::string_view vec_activation { "a" };
        static constexpr std::string_view vec_dactivation { "da" };

        operator NodeLink () const;
        Matrix & vals_of(std::string_view);
        Matrix const & vals_of(std::string_view) const;
        Node & put_after(NodeLink, std::string, std::string);

        virtual void pass_forward_from(Matrix const &) { }
        virtual void pass_backward_into(Matrix const &,
                                        Matrix &) { }
        virtual void update_weights() { }
    protected:
        std::set<NodeLink> followers_, predecessors_;
        std::unordered_map<std::string, Matrix> buffers_;
        std::unordered_map<std::string, float> parameters_;
        std::string name_, type_;
    };
    
    /**
     *  Stores the data for th network inputs and outputs,
     *  but is not resposible for any computations.
     */
    struct InputPlaceholder :
    public Node { };
    
    /**
     *  Unlike other layers, it doesn't receive the lossfunction from somewhere else,
     *  it initiates the gradient descent by
     */
    struct OutputPlaceholder :
    public Node { };
    
    struct LayerDense :
    public Node {
        static constexpr std::string_view mat_weights { "W" };
        static constexpr std::string_view mat_dweights { "dW" };

        void reconfigure(Dim input_size, Dim output_size);
                
        void pass_forward_from(Matrix const &) override;
        void pass_backward_into(Matrix const &, Matrix &) override;
        void update_weights() override;
    };
    
    struct LayerConvolution :
    public Node { };

    struct OperatorActivation :
    public Node {
        void pass_forward_from(Matrix const &) override;
        void pass_backward_into(Matrix const &, Matrix &) override;
    };
    
    /**
     *  Goals:
     *  -   only STL components.
     *  -   parallelizable.
     *  -   no templates in headers.
     *  -   no raw loops.
     */
    struct Network {

        Count depth() const;
        Matrix predict(Matrix && x, std::string, std::string);
        void train(Matrix const & xs, Matrix const & ys,
                   std::string xs_node_name, std::string ys_node_name);
        
        void train(std::vector<Matrix> const & xs_per_input,
                   std::vector<Matrix> const & ys_per_output,
                   Count epochs, float lr);

        Network & make_mlp(std::vector<Dim>);
        Network & make_chain(std::vector<NodePtr>);
        Network & import(std::string, NodePtr);
        Network & connect(std::string, std::string);
        
        void redefine_input(Matrix, std::string);
        void redefine_expectations(Matrix, std::string);
        void propagate_forward();
        void propagate_backward();
        void compute_costs();
        void apply_gradients(float lr);
        Matrix const & final_activations(std::string) const;

    private:
        std::multimap<NodeLink, NodeLink> edges_from;
        std::multimap<NodeLink, NodeLink> edges_to;
        std::unordered_map<std::string, NodePtr> nodes;
    };
}
