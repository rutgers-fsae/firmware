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

#ifndef VN_LINEARLEASTSQUARES_HPP_
#define VN_LINEARLEASTSQUARES_HPP_

#include <cmath>

#include "vectornav/Debug.hpp"
#include "vectornav/EigenDecomposition.hpp"
#include "vectornav/LinearAlgebra.hpp"
#include "vectornav/TemplateLibrary/Matrix.hpp"

namespace VN
{
namespace Math
{

/**
 * @brief Class for solving Linear Least Squares problems.
 * This class provides an implementation of the Linear Least Squares (LLS) method.
 * It solves the equation `Ax = b` using the normal equation `HTH * x = HTy * b`,
 * where `H` is a matrix and `b` is a vector, by performing eigen decomposition
 */
class LinearLeastSquares
{
public:
    /**
     * @brief Enum for error codes related to Linear Least Squares.
     */
    enum class LlsError
    {
        None = 0,                      ///< No error occurred.
        InsufficientData = 9,          ///< Not enough data was provided.
        FailedEigenDecomposition = 11  ///< Eigen decomposition failed.
    };

    /**
     * @brief Structure holding the solution of a Linear Least Squares problem.
     * @tparam n The size of the matrix (n x n) and vectors (n x 1).
     * @tparam T The type of elements in the matrix and vector.
     */
    template <uint16_t n, typename T>
    struct LeastSquaresSolution
    {
        Matrix<n, 1, T> solution;         ///< Solution vector `x` for the system `Ax = b`.
        Matrix<n, n, T> eigenVectors;     ///< Matrix of eigenvectors from the decomposition.
        Matrix<n, 1, T> eigenValuesReal;  ///< Real parts of the eigenvalues.
        Matrix<n, 1, T> eigenValuesImag;  ///< Imaginary parts of the eigenvalues.
        T FOM;                            ///< Figure of merit.
        LlsError error;                   ///< Error code indicating the status of the solution.
    };

    /**
     * @brief Default constructor.
     */
    LinearLeastSquares() = default;

    /**
     * @brief Solves the Linear Least Squares problem using eigen decomposition.
     * @tparam n The size of the matrix (n x n) and vector (n x 1).
     * @tparam T The type of elements in the matrix and vector.
     * @param HTH The matrix `HTH`.
     * @param HTy The matrix `HTy`.
     * @param condition_threshold The threshold below which the smallest eigenvalue will be treated as problematic.
     * @return A structure containing the solution and additional information.
     */
    template <uint16_t n, typename T>
    LeastSquaresSolution<n, T> solve(Matrix<n, n, T>& HTH, Matrix<n, 1, T>& HTy, T condition_threshold) noexcept
    {
        // Initialize structures
        LeastSquaresSolution<n, T> solution{Matrix<n, 1, T>(0.0), Matrix<n, n, T>(0.0), Matrix<n, 1, T>(0.0), Matrix<n, 1, T>(0.0), 0.0, LlsError::None};

        EigenDecomposition eigenDecomp;
        if (eigenDecomp.computeDecomposition(HTH, solution.eigenValuesReal, solution.eigenValuesImag, solution.eigenVectors))
        {
            solution.error = LlsError::FailedEigenDecomposition;
            return solution;
        }

        // Check if ill conditioned
        T minEigenValue = solution.eigenValuesReal(mini(solution.eigenValuesReal));
        if (minEigenValue < condition_threshold)
        {
            solution.error = LlsError::InsufficientData;
            return solution;
        }

        solution.FOM = 1.0 / std::sqrt(minEigenValue);

        // Compute LLS solution
        Matrix<n, n, T> tempMat = transpose(solution.eigenVectors);
        Matrix<n, 1, T> tempVec = tempMat * HTy;
        for (uint16_t i{0}; i < n; ++i)
        {
            for (uint16_t j{0}; j < n; ++j) { tempMat(i, j) = solution.eigenVectors(i, j) / solution.eigenValuesReal(j); }
        }

        solution.solution = tempMat * tempVec;

        return solution;
    }
};

/**
 * @brief Solves the Linear Least Squares problem using LU decomposition.
 * @tparam m The number of rows in the matrix and size of vector.
 * @tparam n The number of columns in the matrix.
 * @tparam T The the type of elements in the matrix and vector.
 * @param A The matrix `A` in the equation `Ax = y`.
 * @param y The vector `y` in the equation `Ax = y`.
 * @return The solution vector `x`.
 */
template <uint16_t m, uint16_t n, typename T>
std::optional<Matrix<n, 1, T>> solveLinearLeastSquares(Matrix<m, n, T> A, const Matrix<m, 1, T>& y) noexcept
{
    return solveLinearSystemLU(transpose(A) * A, transpose(A) * y);
}

}  // namespace Math
}  // namespace VN

#endif  // VN_LINEARLEASTSQUARES_HPP_
