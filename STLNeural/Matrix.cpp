//
//  Matrix.cpp
//  AILibsComparator
//
//  Created by Ashot Vardanian on 6/8/19.
//  Copyright © 2019 Ashot Vardanian. All rights reserved.
//

#include "Matrix.hpp"
#include "Ranges.hpp"
#include <random>

#if defined(LIB_USE_EIGEN) && LIB_USE_EIGEN
#include <Eigen/Dense>
#endif

using namespace av::neural;

#if defined(LIB_USE_EIGEN) && LIB_USE_EIGEN

using EMat = Eigen::Matrix<Weight, Eigen::Dynamic, Eigen::Dynamic>;
decltype(auto) make_eigen(Matrix & a) {
    return Eigen::Map<EMat> { a.flat().data(), a.rows(), a.cols() };
}
decltype(auto) make_eigen(Matrix const & a) {
    return Eigen::Map<EMat> { const_cast<Weight *>(a.flat().data()), a.rows(), a.cols() };
}
void mat_mul_compute(Matrix const & a, Matrix const & b, Matrix & r) {
    make_eigen(r) = make_eigen(a) * make_eigen(b);
}
void mat_mul_compute(Matrix const & a, transposition const & b, Matrix & r) {
    make_eigen(r) = make_eigen(a) * make_eigen(b.original).transpose();
}
void mat_mul_compute(transposition const & a, Matrix const & b, Matrix & r) {
    make_eigen(r) = make_eigen(a.original).transpose() * make_eigen(b);
}

#else

template <typename mat_a_t, typename mat_b_t>
void mat_mul_compute(mat_a_t const & a, mat_b_t const & b, Matrix & r) {
    range::for_each(Idxs { 0, a.rows() }, [&](Dim a_row) {
        range::for_each(Idxs { 0, b.cols() }, [&](Dim b_col) {
            Weight & r_cell = r(a_row, b_col);
            for (Dim i : Idxs { 0, a.cols() }) {
                r_cell += a(a_row, i) * b(i, b_col);
            }
        });
    });
}

#endif

template <typename mat_a_t, typename mat_b_t>
void mat_mul(mat_a_t const & a, mat_b_t const & b, Matrix & r) {
    if (a.cols() != b.rows())
        throw std::logic_error { "Incompatiable Matrix sizes!" };
    r.resize({ a.rows(), b.cols() });
    mat_mul_compute(a, b, r);
}

template <typename mat_a_t, typename mat_b_t, typename binary_op_t>
void mat_elemwise(mat_a_t const & a, mat_b_t const & b, Matrix & r, binary_op_t op) {
    if ((a.rows() != b.rows()) or (a.cols() != b.cols()))
        throw std::logic_error { "Incompatiable Matrix sizes!" };
    r.resize(a.size());
    range::transform(a.flat(), b.flat(), r.flat(), op);
}

#pragma mark - Transposition

Dim transposition::rows() const { return original.cols(); }
Dim transposition::cols() const { return original.rows(); }
MatrixSize transposition::size() const {
    return { original.cols(), original.rows() };
}
Weight transposition::operator () (Dim r, Dim c) const {
    return original(c, r);
}

#pragma mark - Martix

MatrixSize Matrix::size() const { return shape; }
std::vector<Weight> const & Matrix::flat() const { return values; }
std::vector<Weight> & Matrix::flat() { return values; }
Dim Matrix::rows() const { return shape.row; }
Dim Matrix::cols() const { return shape.col; }
Matrix & Matrix::resize(MatrixSize s) { shape = s; values.resize(s.row * s.col); return *this; }
Weight & Matrix::operator () (Dim r, Dim c) { return values[r * shape.col + c]; }
Weight Matrix::operator () (Dim r, Dim c) const { return values[r * shape.col + c]; }
void Matrix::mul(Matrix const & a, Matrix const & b, Matrix & r) { mat_mul(a, b, r); }
void Matrix::mul(transposition const & a, Matrix const & b, Matrix & r) { mat_mul(a, b, r); }
void Matrix::mul(Matrix const & a, transposition const & b, Matrix & r) { mat_mul(a, b, r); }
transposition Matrix::transposed() const { return transposition { *this }; }

Matrix & Matrix::operator = (Matrix const & other) {
    shape = other.shape;
    values.resize(other.values.size());
    std::copy(other.values.begin(), other.values.end(), values.begin());
    return *this;
}
Matrix Matrix::operator * (Weight const v) const {
    Matrix r { *this };
    range::transform(r.flat(), r.flat(), [=](Weight w) {
        return w * v;
    });
    return r;
}
Matrix Matrix::operator * (Matrix const & v) const {
    Matrix r;
    mat_mul(*this, v, r);
    return r;
}
Matrix Matrix::operator * (transposition const & b) const {
    Matrix r;
    mat_mul(*this, b, r);
    return r;
}
Matrix transposition::operator * (Matrix const & b) const {
    Matrix r;
    mat_mul(*this, b, r);
    return r;
}
Matrix & Matrix::randomize_normal(Weight stddev) {
    std::normal_distribution<Weight> dist { 0, stddev };
    std::default_random_engine generator;
    range::generate(values, [&] { return dist(generator); });
    return *this;
}
Matrix & Matrix::randomize_uniform(Weight min, Weight max) {
    std::uniform_real_distribution<Weight> dist { min, max };
    std::default_random_engine generator;
    range::generate(values, [&] { return dist(generator); });
    return *this;
}

Matrix Matrix::hadamar(Matrix const & b) const {
    Matrix r;
    Matrix::hadamar(*this, b, r);
    return r;
}
void Matrix::hadamar(Matrix const & a, Matrix const & b, Matrix & r) {
    r.resize(a.size());
    mat_elemwise(a, b, r, std::multiplies<Weight> { });
}
Matrix Matrix::operator - (Matrix const & b) const {
    Matrix r;
    r.resize(size());
    mat_elemwise(*this, b, r, std::minus<Weight> { });
    return r;
}
