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

#ifndef VN_SAMPLESTATS_HPP_
#define VN_SAMPLESTATS_HPP_

#include "vectornav/Implementation/MeasurementDatatypes.hpp"
#include "vectornav/LinearAlgebra.hpp"
#include "vectornav/TemplateLibrary/Matrix.hpp"

namespace VN
{
namespace Math
{

template <uint16_t n, typename T>
class SampleStats
{
public:
    SampleStats() = default;

    void addSample(Matrix<n, 1, T> meas) noexcept
    {
        _cnt++;

        T measNorm = Math::norm(meas);
        if (_cnt == 1)
        {
            _mean = meas;
            _Q = meas - _mean;  // initializes to zero
            _max = measNorm;
        }
        else
        {
            Matrix<n, 1, T> res = (meas - _mean) / static_cast<T>(_cnt);
            _mean += res;
            _Q += static_cast<T>((_cnt - 1) * _cnt) * element_mul(res, res);
            if (measNorm > _max) { _max = measNorm; }
        }
    }

    Matrix<n, 1, T> getMean() const noexcept { return _mean; }
    Matrix<n, 1, T> getVariance() const noexcept { return (_cnt > 1) ? _Q / static_cast<T>(_cnt - 1) : Matrix<n, 1, T>{0}; }
    Matrix<n, 1, T> getStdDev() const noexcept { return element_sqrt(getVariance()); }
    T getMaxNorm() const noexcept { return _max; }
    uint16_t getCount() const noexcept { return _cnt; }
    void reset() noexcept { _cnt = 0; }

private:
    Matrix<n, 1, T> _mean = Matrix<n, 1, T>(0);
    Matrix<n, 1, T> _Q = Matrix<n, 1, T>(0);
    T _max{0};

    uint16_t _cnt = 0;
};

}  // namespace Math
}  // namespace VN

#endif  // VN_SAMPLESTATS_HPP_
