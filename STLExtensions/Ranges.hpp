//
//  Ranges.hpp
//  SandboxAI
//
//  Created by Ashot Vardanian on 6/8/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#pragma once
#include <numeric> // `inner_product`
#include <algorithm> // `transform`

namespace range {
    template <typename arr_a_t, typename arr_b_t, typename arr_out_t, typename binary_op_t>
    decltype(auto) transform(arr_a_t const & a, arr_b_t const & b, arr_out_t & out, binary_op_t op) {
        return std::transform(/* std::execution::par_unseq, */ a.begin(), a.end(), b.begin(), out.begin(), op);
    }
    template <typename arr_in_t, typename arr_out_t, typename unary_op_t>
    decltype(auto) transform(arr_in_t const & in, arr_out_t & out, unary_op_t op) {
        return std::transform(/* std::execution::par_unseq, */ in.begin(), in.end(), out.begin(), op);
    }
    template <typename arr_a_t, typename arr_b_t, typename initial_t, typename unary_op_t, typename binary_op_t>
    decltype(auto) inner_product(arr_a_t const & a, arr_b_t const & b,
                                 initial_t init, unary_op_t op1, binary_op_t op2) {
        return std::inner_product(/* std::execution::par_unseq, */ a.begin(), a.end(), b.begin(), init, op1, op2);
    }
    template <typename arr_a_t, typename initial_t, typename binary_op_t>
    decltype(auto) accumulate(arr_a_t const & a, initial_t init, binary_op_t op) {
        return std::accumulate(/* std::execution::par_unseq, */ a.begin(), a.end(), init, op);
    }
    template <typename arr_in_t, typename unary_op_t>
    void for_each(arr_in_t const & in, unary_op_t op) {
        std::for_each(/* std::execution::par_unseq, */ in.begin(), in.end(), op);
    }
    template <typename arr_in_t, typename unary_op_t>
    void generate(arr_in_t & in, unary_op_t op) {
        std::generate(/* std::execution::par_unseq, */ in.begin(), in.end(), op);
    }
}

#include <string_view>
#include <vector>
#include <set>

namespace str {
    
    template <typename TFunc>
    inline std::vector<std::string_view> split_around(std::string_view remaining,
                                                      TFunc predicate) {
        std::vector<std::string_view> result;
        while (!remaining.empty()) {
            auto const next = std::find_if(remaining.begin(), remaining.end(), predicate);
            if (next != remaining.end()) {
                auto const next_idx = std::distance(remaining.begin(), next);
                result.push_back(remaining.substr(0, next_idx));
                remaining = remaining.substr(next_idx + 1);
            } else {
                result.push_back(remaining);
                break;
            }
        }
        return result;
    }

    inline std::vector<std::string_view> split(std::string_view full, char delim) {
        return split_around(full, [=](char c) {
            return c == delim;
        });
    }
    
    inline std::vector<std::string_view> split(std::string_view full,
                                               std::set<char> const & delims) {
        return split_around(full, [&](char c) {
            return delims.find(c) != delims.end();
        });
    }

    inline std::string_view trim(std::string_view full, std::set<char> unwanted) {
        auto const in_front = std::count_if(full.cbegin(), full.cend(), [&](char c) {
            return unwanted.find(c) != unwanted.end();
        });
        auto const in_back = std::count_if(full.crbegin(), full.crend(), [&](char c) {
            return unwanted.find(c) != unwanted.end();
        });
        return full.substr(in_front, full.size() - in_front - in_back);
    }
}
