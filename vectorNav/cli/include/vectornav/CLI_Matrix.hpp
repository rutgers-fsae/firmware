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

#ifndef VN_CLI_MATRIX_HPP_
#define VN_CLI_MATRIX_HPP_

#pragma managed(push, off)
#include "vectornav/TemplateLibrary/Matrix.hpp"
#pragma managed(pop)

namespace VNSDK
{
public value class Mat3f
{
private:
    array<float>^ matrix;
    size_t length;

public:

    // Constructors
    Mat3f(float value)
    {
        length = 9;
        matrix = gcnew array<float>(length);
        for (int i = 0; i < length; i++) { matrix[i] = value; }
    }
    Mat3f(array<float>^ values)
    {
        length = 9;
        matrix = gcnew array<float>(length);
        if (values->Length != length) { throw gcnew System::ArgumentException("array must contain exactly 9 elements for a 3x3 matrix."); }
        for (int i = 0; i < length; i++) { matrix[i] = values[i]; }
    }
    Mat3f(const VN::Mat3f& other)
    {
        length = 9;
        matrix = gcnew array<float>(length);
        for (uint8_t i = 0; i < length; i++) { matrix[i] = other[i]; }
    }
    Mat3f(Mat3f^ other)
    {
        length = 9;
        matrix = gcnew array<float>(length);
        for (uint8_t i = 0; i < length; i++) { matrix[i] = other->matrix[i]; }
    }

    static Mat3f^ Identity() {
        VN::Mat3f mat = VN::Mat3f::identity();
        return gcnew Mat3f(mat);
    }

    // Accessors
    property float default[ int ]
    {
        float get(int index) { return matrix[index]; }
        void set(int index, float value) { matrix[index] = value; }
    }
    property float default[ int, int ]
    {
        float get(int row, int col) { return matrix[col + row * 3]; }
        void set(int row, int col, float value) { matrix[col + row * 3] = value; }
    }

    float get(int index) { return matrix[index]; }
    void set(int index, float value) { matrix[index] = value; }
    float get(int row, int col) { return matrix[col + row * 3]; }
    void set(int row, int col, float value) { matrix[col + row * 3] = value; }

    // Conversions
    static std::array<float, 9> ToStdArray(Mat3f^ other)
    {
        std::array<float, 9> stdArray;
        for (uint8_t i = 0; i < 9; i++) { stdArray[i] = other[i]; }
        return stdArray;
    }

    array<float>^ ToArray()
    {
        array<float>^ arr = gcnew array<float>(length);
        for (uint8_t i = 0; i < length; i++) { arr[i] = matrix[i]; }
        return arr;
    }
    
    array<float>^ ToColMajorArray()
    {
        uint8_t num_rows = 3;
        uint8_t num_cols = 3;
        array<float>^ arr = gcnew array<float>(length);
        for (uint8_t i = 0; i < num_rows; i++)
        {
            for (uint8_t j = 0; j < num_cols; i++) { arr[i + (j * 3)] = matrix[j + (i * 3)]; }
        }
        return arr;
    }

    VN::Mat3f GetNative()
    {
        return VN::Mat3f(ToStdArray(*this));
    }
    
};

public value class Vec2f
{
private:
    array<float>^ vector;
    size_t length;

public:

    // Constructors
    Vec2f(float value)
    {
        length = 2;
        vector = gcnew array<float>(length);
        for (int i = 0; i < length; i++) { vector[i] = value; }
    }
    Vec2f(array<float>^ values)
    {
        length = 2;
        vector = gcnew array<float>(length);
        if (values->Length != length) { throw gcnew System::ArgumentException("array must contain exactly 2 elements for a 2x1 matrix."); }
        for (int i = 0; i < length; i++) { vector[i] = values[i]; }
    }
    Vec2f(const VN::Vec2f& other)
    {
        length = 2;
        vector = gcnew array<float>(length);
        for (uint8_t i = 0; i < length; i++) { vector[i] = other[i]; }
    }
    Vec2f(Vec2f^ other)
    {
        length = 2;
        vector = gcnew array<float>(length);
        for (uint8_t i = 0; i < length; i++) { vector[i] = other->vector[i]; }
    }

    // Accessors
    property float default[ int ]
    {
        float get(int index) { return vector[index]; }
        void set(int index, float value) { vector[index] = value; }
    }

    float get(int index) { return vector[index]; }
    void set(int index, float value) { vector[index] = value; }

    property float x
    {
        float get() { return vector[0]; }
        void set(float value) { vector[0] = value; }
    }
    property float y
    {
        float get() { return vector[1]; }
        void set(float value) { vector[1] = value; }
    }
    
    // Conversions
    static std::array<float, 2> ToStdArray(Vec2f^ other)
    {
        std::array<float, 2> stdArray;
        for (uint8_t i = 0; i < 2; i++) { stdArray[i] = other[i]; }
        return stdArray;
    }
    array<float>^ ToArray()
    {
        array<float>^ arr = gcnew array<float>(length);
        for (uint8_t i = 0; i < length; i++) { arr[i] = vector[i]; }
        return arr;
    }

    VN::Vec2f GetNative()
    {
        return VN::Vec2f(ToStdArray(*this));
    }
};

public value class Vec3f
{
private:
    array<float>^ vector;
    size_t length;

public:

    // Constructors
    Vec3f(float value)
    {
        length = 3;
        vector = gcnew array<float>(length);
        for (int i = 0; i < length; i++) { vector[i] = value; }
    }
    Vec3f(array<float>^ values)
    {
        length = 3;
        vector = gcnew array<float>(length);
        if (values->Length != length) { throw gcnew System::ArgumentException("array must contain exactly 3 elements for a 3x1 matrix."); }
        for (int i = 0; i < length; i++) { vector[i] = values[i]; }
    }
    Vec3f(const VN::Vec3f& other)
    {
        length = 3;
        vector = gcnew array<float>(length);
        for (uint8_t i = 0; i < length; i++) { vector[i] = other[i]; }
    }
    Vec3f(Vec3f^ other)
    {
        length = 3;
        vector = gcnew array<float>(length);
        for (uint8_t i = 0; i < length; i++) { vector[i] = other->vector[i]; }
    }

    // Accessors
    property float default[ int ]
    {
        float get(int index) { return vector[index]; }
        void set(int index, float value) { vector[index] = value; }
    }

    float get(int index) { return vector[index]; }
    void set(int index, float value) { vector[index] = value; }

    property float x
    {
        float get() { return vector[0]; }
        void set(float value) { vector[0] = value; }
    }
    property float y
    {
        float get() { return vector[1]; }
        void set(float value) { vector[1] = value; }
    }
    property float z
    {
        float get() { return vector[2]; }
        void set(float value) { vector[2] = value; }
    }

    // Conversions
    static std::array<float, 3> ToStdArray(Vec3f^ other)
    {
        std::array<float, 3> stdArray;
        for (uint8_t i = 0; i < 3; i++) { stdArray[i] = other[i]; }
        return stdArray;
    }
    array<float>^ ToArray()
    {
        array<float>^ arr = gcnew array<float>(length);
        for (uint8_t i = 0; i < length; i++) { arr[i] = vector[i]; }
        return arr;
    }

    VN::Vec3f GetNative()
    {
        return VN::Vec3f(ToStdArray(*this));
    }
};

public value class Vec4f
{
private:
    array<float>^ vector;
    size_t length;

public:
    // Constructors
    Vec4f(float value)
    {
        length = 4;
        vector = gcnew array<float>(length);
        for (int i = 0; i < length; i++) { vector[i] = value; }
    }
    Vec4f(array<float>^ values)
    {
        length = 4;
        vector = gcnew array<float>(length);
        if (values->Length != length) { throw gcnew System::ArgumentException("array must contain exactly 3 elements for a 4x1 matrix."); }
        for (int i = 0; i < length; i++) { vector[i] = values[i]; }
    }
    Vec4f(const VN::Vec4f& other)
    {
        length = 4;
        vector = gcnew array<float>(length);
        for (uint8_t i = 0; i < length; i++) { vector[i] = other[i]; }
    }
    Vec4f(Vec4f^ other)
    {
        length = 4;
        vector = gcnew array<float>(length);
        for (uint8_t i = 0; i < length; i++) { vector[i] = other->vector[i]; }
    }

    // Accessors
    property float default[ int ]
    {
        float get(int index) { return vector[index]; }
        void set(int index, float value) { vector[index] = value; }
    }

    float get(int index) { return vector[index]; }
    void set(int index, float value) { vector[index] = value; }

    // Conversions
    static std::array<float, 4> ToStdArray(Vec4f^ other)
    {
        std::array<float, 4> stdArray;
        for (uint8_t i = 0; i < 4; i++) { stdArray[i] = other[i]; }
        return stdArray;
    }

    array<float>^ ToArray() {
        array<float>^ arr = gcnew array<float>(length);
        for (uint8_t i = 0; i < length; i++) { arr[i] = vector[i]; }
        return arr;
    }

    VN::Vec4f GetNative()
    {
        return VN::Vec4f(ToStdArray(*this));
    }
};

public value class Vec8f
{
private:
    array<float>^ vector;
    size_t length;

public:
    // Constructors
    Vec8f(float value)
    {
        length = 8;
        vector = gcnew array<float>(length);
        for (int i = 0; i < length; i++) { vector[i] = value; }
    }
    Vec8f(array<float>^ values)
    {
        length = 8;
        vector = gcnew array<float>(length);
        if (values->Length != length) { throw gcnew System::ArgumentException("array must contain exactly 3 elements for a 8x1 matrix."); }
        for (int i = 0; i < length; i++) { vector[i] = values[i]; }
    }
    Vec8f(const VN::Matrix<8, 1, float>& other)
    {
        length = 8;
        vector = gcnew array<float>(length);
        for (uint8_t i = 0; i < length; i++) { vector[i] = other[i]; }
    }
    Vec8f(Vec8f^ other)
    {
        length = 8;
        vector = gcnew array<float>(length);
        for (uint8_t i = 0; i < length; i++) { vector[i] = other->vector[i]; }
    }

    // Accessors
    property float default[ int ]
    {
        float get(int index) { return vector[index]; }
        void set(int index, float value) { vector[index] = value; }
    }

    float get(int index) { return vector[index]; }
    void set(int index, float value) { vector[index] = value; }

    // Conversions
    static std::array<float, 8> ToStdArray(Vec8f^ other)
    {
        std::array<float, 8> stdArray;
        for (uint8_t i = 0; i < 8; i++) { stdArray[i] = other[i]; }
        return stdArray;
    }
    array<float>^ ToArray()
    {
        array<float>^ arr = gcnew array<float>(length);
        for (uint8_t i = 0; i < length; i++) { arr[i] = vector[i]; }
        return arr;
    }

    VN::Matrix<8, 1, float> GetNative()
    {
        return VN::Matrix<8, 1, float>(ToStdArray(*this));
    }
};

public value class Vec3d
{
private:
    array<double>^ vector;
    size_t length;

public:

    // Constructors
    Vec3d(double value)
    {
        length = 3;
        vector = gcnew array<double>(length);
        for (int i = 0; i < length; i++) { vector[i] = value; }
    }
    Vec3d(array<double>^ values)
    {
        length = 3;
        vector = gcnew array<double>(length);
        if (values->Length != length) { throw gcnew System::ArgumentException("array must contain exactly 3 elements for a 3x1 matrix."); }
        for (int i = 0; i < length; i++) { vector[i] = values[i]; }
    }
    Vec3d(const VN::Vec3d& other)
    {
        length = 3;
        vector = gcnew array<double>(length);
        for (uint8_t i = 0; i < length; i++) { vector[i] = other[i]; }
    }
    Vec3d(Vec3d^ other)
    {
        length = 3;
        vector = gcnew array<double>(length);
        for (uint8_t i = 0; i < length; i++) { vector[i] = other->vector[i]; }
    }

    // Accessors
    property double default[ int ]
    {
        double get(int index) { return vector[index]; }
        void set(int index, double value) { vector[index] = value; }
    }
    double get(int index) { return vector[index]; }
    void set(int index, double value) { vector[index] = value; }

    property double x
    {
        double get() { return vector[0]; }
        void set(double value) { vector[0] = value; }
    }
    property double y
    {
        double get() { return vector[1]; }
        void set(double value) { vector[1] = value; }
    }
    property double z
    {
        double get() { return vector[2]; }
        void set(double value) { vector[2] = value; }
    }

    // Conversions
    static std::array<double, 3> ToStdArray(Vec3d^ other)
    {
        std::array<double, 3> stdArray;
        for (uint8_t i = 0; i < 3; i++) { stdArray[i] = other[i]; }
        return stdArray;
    }
    array<float>^ ToArray()
    {
        array<float>^ arr = gcnew array<float>(length);
        for (uint8_t i = 0; i < length; i++) { arr[i] = vector[i]; }
        return arr;
    }
    VN::Vec3d GetNative()
    {
        return VN::Vec3d(ToStdArray(*this));
    }
};

}  // namespace VNSDK

#endif  // VN_CLI_MATRIX_HPP_
