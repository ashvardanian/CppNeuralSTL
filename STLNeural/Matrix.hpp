//
//  Matrix.hpp
//  AILibsComparator
//
//  Created by Ashot Vardanian on 6/8/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#pragma once
#include <vector>

namespace av::neural {
    
    using Count = size_t;
    using Dim = int;
    using Weight = float;
    using Weights = std::vector<Weight>;
    
    struct Idxs {
        struct Idx {
            Dim operator * () const { return val; }
            Idx & operator ++ () { val++; return *this; };
            void operator ++ (int) { val++; };
            bool operator != (Idx const & v) const { return val != v.val; }
            Dim val = 0;
        };
        Idxs(Dim f, Dim l) : begin_(f), end_(l) { }
        Idxs(Count cnt) : begin_(0), end_(static_cast<Dim>(cnt)) { }
        Idxs(Idxs const &) = default;
        Idx begin() const { return { begin_ }; }
        Idx end() const { return { end_ }; }
    private:
        Dim begin_, end_;
    };

    struct MatrixSize {
        Dim row = 0;
        Dim col = 0;
    };
    
    struct transposition;
    
    struct Matrix :
    public std::enable_shared_from_this<Matrix> {
        
        Dim rows() const;
        Dim cols() const;
        MatrixSize size() const;
        Matrix & resize(MatrixSize);
        Matrix & randomize_normal(Weight stddev);
        Matrix & randomize_uniform(Weight min, Weight max);

        Weight & operator () (Dim, Dim);
        Weight operator () (Dim, Dim) const;

        Weights & flat();
        Weights const & flat() const;
        transposition transposed() const;
        Matrix & operator = (Matrix const &);
        Matrix operator * (Weight const) const;
        Matrix operator * (Matrix const &) const;
        Matrix operator * (transposition const &) const;
        Matrix operator + (Matrix const &) const;
        Matrix operator - (Matrix const &) const;
        Matrix hadamar(Matrix const &) const;
        
        static void mul(Matrix const & a, Matrix const & b, Matrix & r);
        static void mul(Matrix const & a, transposition const & b, Matrix & r);
        static void mul(transposition const & a, Matrix const & b, Matrix & r);
        static void hadamar(Matrix const & a, Matrix const & b, Matrix & r);
    private:
        MatrixSize shape;
        Weights values;
    };
    
    struct transposition {
        MatrixSize size() const;
        Dim rows() const;
        Dim cols() const;
        Weight operator () (Dim, Dim) const;
        Matrix operator * (Matrix const &) const;
        
        Matrix const & original;
    };
}

namespace av::opensource {
    using Matrix = neural::Matrix;
    using Idxs = neural::Idxs;
}
