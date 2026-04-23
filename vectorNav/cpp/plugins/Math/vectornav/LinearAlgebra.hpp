// VectorNav SDK (v1.0.0)
// Copyright (c) 2024 VectorNav Technologies, LLC
// 
// WARNING â€“ This software contains Controlled Technical Data, export of which
// is restricted by the Export Administration Regulations ("EAR") (ECCN 7D994). 
// Disclosure to foreign persons contrary to the EAR is prohibited. Violations
// of these export laws and regulations are subject to severe civil and criminal
// penalties.
// 
// The MIT License (MIT)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef VN_LINEARALGEBRA_HPP_
#define VN_LINEARALGEBRA_HPP_

#include <cmath>
#include <numeric>

#include "vectornav/Debug.hpp"
#include "vectornav/TemplateLibrary/Matrix.hpp"

namespace VN
{
namespace Math
{

// Vector math

/**
 * @brief Computes the dot product of two 3D vectors.
 * @tparam T Type of elements in the left-hand side vector.
 * @tparam S Type of elements in the right-hand side vector.
 * @param lhs Left-hand side vector.
 * @param rhs Right-hand side vector.
 * @return The dot product (scalar).
 */
template <typename T, typename S>
T dot(const Matrix<3, 1, T>& lhs, const Matrix<3, 1, S>& rhs) noexcept
{
    return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
}

/**
 * @brief Computes the dot product of two 4D vectors.
 * @tparam T Type of elements in the left-hand side vector.
 * @tparam S Type of elements in the right-hand side vector.
 * @param lhs Left-hand side vector.
 * @param rhs Right-hand side vector.
 * @return The dot product (scalar).
 */
template <typename T, typename S>
T dot(const Matrix<4, 1, T>& lhs, const Matrix<4, 1, S>& rhs) noexcept
{
    return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3];
}

/**
 * @brief Computes the dot product of two vectors of arbitrary dimension.
 * @tparam m The size of the vectors.
 * @tparam T Type of elements in the left-hand side vector.
 * @tparam S Type of elements in the right-hand side vector.
 * @param lhs Left-hand side vector.
 * @param rhs Right-hand side vector.
 * @return The dot product (scalar).
 */
template <uint16_t m, typename T, typename S>
T dot(const Matrix<m, 1, T>& lhs, const Matrix<m, 1, S>& rhs) noexcept
{
    T sum = 0;
    for (uint16_t i = 0; i < m; i++) { sum += lhs[i] * rhs[i]; }
    return sum;
}

/**
 * @brief Computes the cross product of two 3D vectors.
 * @tparam T Type of elements in the input vectors.
 * @param lhs Left-hand side vector.
 * @param rhs Right-hand side vector.
 * @return The cross product.
 */
template <typename T>
Matrix<3, 1, T> cross(const Matrix<3, 1, T>& lhs, const Matrix<3, 1, T>& rhs) noexcept
{
    Matrix<3, 1, T> retMatrix;
    retMatrix[0] = lhs(1) * rhs(2) - lhs(2) * rhs(1);
    retMatrix[1] = lhs(2) * rhs(0) - lhs(0) * rhs(2);
    retMatrix[2] = lhs(0) * rhs(1) - lhs(1) * rhs(0);
    return retMatrix;
}

/**
 * @brief Computes the inverse of a square matrix.
 * @tparam n The size of the square matrix.
 * @tparam T The type of elements in the matrix.
 * @param mat The square matrix.
 * @return The inverse of the matrix, or a null matrix if the matrix is non-invertible.
 */
template <uint16_t n, typename T>
Matrix<n, n, T> inverse(const Matrix<n, n, T>& mat) noexcept
{
    Matrix<n, n, T> ac = mat;
    Matrix<n, n, T> retMatrix;

    uint16_t i, j, iPass, imx, icol, irow;
    T det = 1;
    T temp, pivot, factor;

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++) { retMatrix(i, j) = 0; }
        retMatrix(i, i) = 1;
    }

    for (iPass = 0; iPass < n; iPass++)
    {
        imx = iPass;
        for (irow = iPass; irow < n; irow++)
        {
            if (std::abs(ac(irow, iPass)) > std::abs(ac(imx, iPass))) { imx = irow; }
        }

        if (imx != iPass)
        {
            for (icol = 0; icol < n; icol++)
            {
                temp = retMatrix(iPass, icol);
                retMatrix(iPass, icol) = retMatrix(imx, icol);
                retMatrix(imx, icol) = temp;
                if (icol >= iPass)
                {
                    temp = ac(iPass, icol);
                    ac(iPass, icol) = ac(imx, icol);
                    ac(imx, icol) = temp;
                }
            }
        }

        pivot = ac(iPass, iPass);
        det = det * pivot;

        if (std::abs(det) < std::numeric_limits<T>::epsilon())
        {
            return Matrix<n, n, T>::null();  // Matrix is not invertible
        }

        for (icol = 0; icol < n; icol++)
        {
            retMatrix(iPass, icol) = retMatrix(iPass, icol) / pivot;
            if (icol >= iPass) { ac(iPass, icol) = ac(iPass, icol) / pivot; }
        }

        for (irow = 0; irow < n; irow++)
        {
            if (irow != iPass)
            {
                factor = ac(irow, iPass);
                for (icol = 0; icol < n; icol++)
                {
                    if (irow != iPass)
                    {
                        retMatrix(irow, icol) -= factor * retMatrix(iPass, icol);
                        ac(irow, icol) -= factor * ac(iPass, icol);
                    }
                }
            }
        }
    }

    return retMatrix;  // Matrix inversion successful
}

/**
 * @brief Computes the inverse of a 2x2 matrix.
 * @tparam T The type of elements in the matrix.
 * @param mat The 2x2 matrix.
 * @return The inverse of the matrix, or a null matrix if the matrix is non-invertible.
 */
template <typename T>
Matrix<2, 2, T> inverse(const Matrix<2, 2, T>& mat) noexcept
{
    Matrix<2, 2, T> nm({mat[3], -mat[1], -mat[2], mat[0]});

    T det = mat[0] * mat[3] - mat[1] * mat[2];

    if (std::abs(det) < 10 * std::numeric_limits<T>::epsilon()) { return Matrix<2, 2, T>::null(); }

    T invDet = 1 / det;
    return nm * invDet;
}

/**
 * @brief Computes the inverse of a 3x3 matrix.
 * @tparam T The type of elements in the matrix.
 * @param mat The 3x3 matrix.
 * @return The inverse of the matrix, or a null matrix if the matrix is non-invertible.
 */
template <typename T>
Matrix<3, 3, T> inverse(const Matrix<3, 3, T>& mat) noexcept
{
    Matrix<3, 3, T> nm;
    T det = mat(0, 0) * (mat(1, 1) * mat(2, 2) - mat(2, 1) * mat(1, 2)) - mat(0, 1) * (mat(1, 0) * mat(2, 2) - mat(1, 2) * mat(2, 0)) +
            mat(0, 2) * (mat(1, 0) * mat(2, 1) - mat(1, 1) * mat(2, 0));

    if (std::abs(det) < std::numeric_limits<T>::epsilon()) { return Matrix<3, 3, T>::null(); }

    T invDet = 1 / det;

    nm(0, 0) = (mat(1, 1) * mat(2, 2) - mat(2, 1) * mat(1, 2)) * invDet;
    nm(0, 1) = (mat(0, 2) * mat(2, 1) - mat(0, 1) * mat(2, 2)) * invDet;
    nm(0, 2) = (mat(0, 1) * mat(1, 2) - mat(0, 2) * mat(1, 1)) * invDet;
    nm(1, 0) = (mat(1, 2) * mat(2, 0) - mat(1, 0) * mat(2, 2)) * invDet;
    nm(1, 1) = (mat(0, 0) * mat(2, 2) - mat(0, 2) * mat(2, 0)) * invDet;
    nm(1, 2) = (mat(1, 0) * mat(0, 2) - mat(0, 0) * mat(1, 2)) * invDet;
    nm(2, 0) = (mat(1, 0) * mat(2, 1) - mat(2, 0) * mat(1, 1)) * invDet;
    nm(2, 1) = (mat(2, 0) * mat(0, 1) - mat(0, 0) * mat(2, 1)) * invDet;
    nm(2, 2) = (mat(0, 0) * mat(1, 1) - mat(1, 0) * mat(0, 1)) * invDet;

    return nm;
}

/**
 * @brief Solves a linear system of equations (`Ux = b`) using LU decomposition.
 * @tparam n The size of the square matrix (n x n) and vectors (n X 1).
 * @tparam T The type of elements in the matrix and vectors.
 * @param U The matrix `U` of the system.
 * @param b The vector `b` of the system.
 * @param conditionThreshold The condition number threshold above which the matrix is considered ill-conditioned (default: 1e12).
 * @return The solution vector `x` to the system, or nullopt if the system is singular or ill-conditioned.
 */
template <uint16_t n, typename T>
std::optional<Matrix<n, 1, T>> solveLinearSystemLU(Matrix<n, n, T> U, const Matrix<n, 1, T>& b,
                                                   T conditionThreshold = T(1) / std::numeric_limits<T>::epsilon()) noexcept
{
    auto L = Matrix<n, n, T>();
    auto ri = std::array<int, n>();
    std::iota(ri.begin(), ri.end(), 0);
    Matrix<n, 1, T> x;

    // Track pivot statistics for conditioning assessment
    T maxPivot = T(0);
    T minPivot = std::numeric_limits<T>::max();

    // i indicates which column (from ri) we are reducing
    // LU Decomposition
    for (uint16_t i = 0; i < n - 1; i++)
    {
        // find row to pivot
        T pivot = 0;
        int row = 0;

        for (uint16_t j = i; j < n; j++)
        {
            T tmp = U(ri[j], i);
            if (std::fabs(pivot) < std::fabs(tmp))
            {
                row = j;
                pivot = tmp;
            }
        }

        // Update pivot statistics
        T absPivot = std::fabs(pivot);
        maxPivot = std::max(maxPivot, absPivot);
        minPivot = std::min(minPivot, absPivot);

        // Check for ill-conditioning using pivot ratio
        // If condition number estimate > threshold, consider ill-conditioned
        if (minPivot < std::numeric_limits<T>::epsilon() || maxPivot / minPivot > conditionThreshold)
        {
            return std::nullopt;  // Ill-conditioned matrix
        }

        // keep track of pivot rows
        if (ri[i] != ri[row]) { std::swap(ri[i], ri[row]); }

        // Reduce columns to zero
        for (uint16_t j = i + 1; j < n; j++)
        {
            T c = -U(ri[j], i) / pivot;
            L(ri[j], i) = -c;
            for (uint16_t k = i; k < n; k++) { U(ri[j], k) += (c * U(ri[i], k)); }
        }
    }

    // Final pivot check
    T finalPivot = std::fabs(U(ri[n - 1], n - 1));
    maxPivot = std::max(maxPivot, finalPivot);
    minPivot = std::min(minPivot, finalPivot);

    // Final conditioning checks
    if (minPivot < std::numeric_limits<T>::epsilon() || maxPivot / minPivot > conditionThreshold)
    {
        return std::nullopt;  // Ill-conditioned matrix
    }

    // solve Ly = Pb via forward sub
    for (int i = 0; i < static_cast<int>(n); i++)
    {
        x[i] = b[ri[i]];
        for (int j = 0; j < i; j++) { x[i] -= x[j] * L(ri[i], j); }
    }

    // solve Ux = y via backward sub
    for (int i = n - 1; i >= 0; i--)
    {
        for (int j = n - 1; j > i; j--) { x[i] -= x[j] * U(ri[i], j); }
        x[i] /= U(ri[i], i);
    }

    return std::make_optional(x);
}

/**
 * @brief Computes the transpose of an n x m matrix.
 * @tparam m The number of rows of the matrix.
 * @tparam n The number of columns of the matrix.
 * @tparam T The type of elements in the matrix.
 * @param mat The matrix.
 * @return The transpose of the matrix.
 */
template <uint16_t m, uint16_t n, typename T>
Matrix<n, m, T> transpose(const Matrix<m, n, T>& mat) noexcept
{
    Matrix<n, m, T> nm;

    for (uint16_t row = 0; row < m; row++)
    {
        for (uint16_t col = 0; col < n; col++) { nm(col, row) = mat(row, col); }
    }

    return nm;
}

/**
 * @brief Returns the sign of a value.
 * @tparam T The type of the value (can be any numeric type).
 * @param val The value.
 * @return The sign of the value (-1, 0, or 1).
 */
template <typename T>
T sign(T val) noexcept
{
    return static_cast<T>((T(0) < val) - (val < T(0)));
}

/**
 * @brief Returns the skew-symmetric matrix of a 3D vector.
 * @tparam T The type of elements in the vector.
 * @param vec The 3D vector.
 * @return The skew-symmetric matrix corresponding to the vector.
 */
template <typename T>
Matrix<3, 3, T> skew(const Matrix<3, 1, T>& vec) noexcept
{
    return {0, -vec[2], vec[1], vec[2], 0, -vec[0], -vec[1], vec[0], 0};
}

/**
 * @brief Computes the norm of a matrix.
 * @tparam m The number of rows in the matrix.
 * @tparam n The number of columns in the matrix.
 * @tparam T The type of the elements in the matrix.
 * @param mat The matrix.
 * @return The norm of the matrix.
 */
template <uint16_t m, uint16_t n, typename T>
T norm(const Matrix<m, n, T>& mat) noexcept
{
    T sum{0};
    for (uint16_t i = 0; i < m * n; i++) { sum += mat[i] * mat[i]; }

    return std::sqrt(sum);
}

/**
 * @brief Finds the index of the maximum element in a vector.
 * @tparam n The size of the vector.
 * @tparam T The type of the elements in the vector.
 * @param vec The vector.
 * @return The index of the element with the maximum value.
 */
template <uint16_t n, typename T>
uint16_t maxi(const Matrix<n, 1, T>& vec) noexcept
{
    uint16_t maxIndex = 0;
    for (uint16_t i = 1; i < n; i++) { maxIndex = (vec[i] > vec[maxIndex]) ? i : maxIndex; }
    return maxIndex;
}

/**
 * @brief Finds the index of the minimum element in a vector.
 * @tparam n The size of the vector.
 * @tparam T The type of the elements in the vector.
 * @param vec The vector.
 * @return The index of the element with the minimum value.
 */

template <uint16_t n, typename T>
uint16_t mini(const Matrix<n, 1, T>& vec) noexcept
{
    uint16_t minIndex = 0;
    for (uint16_t i = 1; i < n; i++) { minIndex = (vec[i] < vec[minIndex]) ? i : minIndex; }
    return minIndex;
}

/**
 * @brief Computes the condition number of a matrix using its norm and inverse.
 * @tparam m The size of the square matrix.
 * @param A The matrix.
 * @param Ainv The inverse of the matrix.
 * @return The condition number of the matrix.
 */
template <uint16_t m>
float condition(const Matrix<m, m, float>& A, const Matrix<m, m, float>& Ainv) noexcept
{
    return norm(A) * norm(Ainv);
}

/**
 * @brief Normalizes a vector.
 * @tparam m The size of the vector.
 * @tparam T The type of the elements in the vector.
 * @param vec The vector.
 * @return An optional containing the normalized vector, or empty if the norm is too small.
 */
template <uint16_t m, typename T>
std::optional<Matrix<m, 1, T>> normalize(Matrix<m, 1, T>& vec) noexcept
{
    T normM = norm(vec);
    if (normM < std::numeric_limits<T>::epsilon()) { return std::nullopt; }
    return std::make_optional<Matrix<m, 1, T>>(vec / normM);
}

/**
 * @brief Computes the outer product of a vector with itself.
 * @tparam m The size of the vector.
 * @tparam T The type of the elements in the vector.
 * @param lhs The vector.
 * @return The resulting outer product matrix.
 */
template <uint16_t m, typename T>
Matrix<m, m, T> outer(const Matrix<m, 1, T>& lhs) noexcept
{
    return lhs * transpose(lhs);
}

/**
 * @brief Computes the outer product of two vectors.
 * @tparam m The size of the vector.
 * @tparam T The type of the elements in the vectors.
 * @param vec1 The first vector.
 * @param vec2 The second vector.
 * @return The resulting outer product matrix.
 */
template <uint16_t m, typename T>
Matrix<m, m, T> outer(const Matrix<m, 1, T>& vec1, const Matrix<m, 1, T>& vec2) noexcept
{
    return vec1 * transpose(vec2);
}

/**
 * @brief Concatenates two matrices vertically.
 * @tparam n1 The number of rows in the first matrix.
 * @tparam n2 The number of rows in the second matrix.
 * @tparam m The number of columns in both matrices.
 * @tparam T The type of the elements in the matrices.
 * @param a The first matrix.
 * @param b The second matrix.
 * @return The resulting matrix after vertical concatenation.
 */
template <uint16_t n1, uint16_t m, uint16_t n2, typename T>
Matrix<n1 + n2, m, T> vertcat(const Matrix<n1, m, T>& a, const Matrix<n2, m, T>& b) noexcept
{
    Matrix<n1 + n2, m, T> out;
    std::copy(a.begin(), a.end(), out.begin());
    std::copy(b.begin(), b.end(), out.begin() + n1 * m);
    return out;
}

/**
 * @brief Concatenates two matrices horizontally.
 * @tparam n The number of rows in both matrices.
 * @tparam m1 The number of columns in the first matrix.
 * @tparam m2 The number of columns in the second matrix.
 * @tparam T The type of the elements in the matrices.
 * @param a The first matrix.
 * @param b The second matrix.
 * @return The resulting matrix after horizontal concatenation.
 */
template <uint16_t n, uint16_t m1, uint16_t m2, typename T>
Matrix<n, m1 + m2, T> horzcat(const Matrix<n, m1, T>& a, const Matrix<n, m2, T>& b) noexcept
{
    return transpose(vertcat(transpose(a), transpose(b)));
}

}  // namespace Math
}  // namespace VN

#endif  // VN_LINEARALGEBRA_HPP_
