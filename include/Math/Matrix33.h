#pragma once

#include "Math.h"
#include "Math/Vector3.h"

class Matrix44;
class Quaternion;

class Matrix33
{
public:
    Matrix33()
    {
        Unit();
    }

    Matrix33(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22)
    {
        this->m00 = m00; this->m01 = m01; this->m02 = m02;
        this->m10 = m10; this->m11 = m11; this->m12 = m12;
        this->m20 = m20; this->m21 = m21; this->m22 = m22;
    }

    Matrix33(const Matrix44& matrix);

    const Vector3& operator[](const unsigned int index) const
    {
        return m[index];
    }

    Vector3& operator[](const unsigned int index)
    {
        return m[index];
    }

    Matrix33 operator*(const Matrix33& matrixB) const
    {
        const Matrix33& matrixA = *this;

        return Matrix33(
            matrixA.m00 * matrixB.m00 + matrixA.m01 * matrixB.m10 + matrixA.m02 * matrixB.m20,
            matrixA.m00 * matrixB.m01 + matrixA.m01 * matrixB.m11 + matrixA.m02 * matrixB.m21,
            matrixA.m00 * matrixB.m02 + matrixA.m01 * matrixB.m12 + matrixA.m02 * matrixB.m22,

            matrixA.m10 * matrixB.m00 + matrixA.m11 * matrixB.m10 + matrixA.m12 * matrixB.m20,
            matrixA.m10 * matrixB.m01 + matrixA.m11 * matrixB.m11 + matrixA.m12 * matrixB.m21,
            matrixA.m10 * matrixB.m02 + matrixA.m11 * matrixB.m12 + matrixA.m12 * matrixB.m22,

            matrixA.m20 * matrixB.m00 + matrixA.m21 * matrixB.m10 + matrixA.m22 * matrixB.m20,
            matrixA.m20 * matrixB.m01 + matrixA.m21 * matrixB.m11 + matrixA.m22 * matrixB.m21,
            matrixA.m20 * matrixB.m02 + matrixA.m21 * matrixB.m12 + matrixA.m22 * matrixB.m22
        );
    }

    Vector3 operator*(const Vector3& other) const
    {
        return Vector3(
            other.x * m00 + other.y * m10 + other.z * m20,
            other.x * m01 + other.y * m11 + other.z * m21,
            other.x * m02 + other.y * m12 + other.z * m22
        );
    }

    float Determinant() const
    {
        return Determinant(*this);
    }

    static float Determinant(const Matrix33& matrix)
    {
        return matrix.m00 * (matrix.m11 * matrix.m22 - matrix.m12 * matrix.m21) -
            matrix.m10 * (matrix.m01 * matrix.m22 - matrix.m02 * matrix.m21) +
            matrix.m20 * (matrix.m01 * matrix.m12 - matrix.m02 * matrix.m11);
    }

    void Normalize()
    {
        for (unsigned int i = 0; i < 3; ++i)
        {
            m[i].Normalize();
        }
    }

    Matrix33 Normalized() const
    {
        Matrix33 matrix = *this;

        matrix.Normalize();

        return matrix;
    }

    void Unit()
    {
        SetIdentity();
    }

    static void Unit(Matrix33& matrix)
    {
        matrix.SetIdentity();
    }

    void SetIdentity()
    {
        m00 = 1; m01 = 0; m02 = 0;
        m10 = 0; m11 = 1; m12 = 0;
        m20 = 0; m21 = 0; m22 = 1;
    }

    void Negate()
    {
        Negate(*this);
    }

    static void Negate(Matrix33& matrix)
    {
        for (unsigned int i = 0; i < 3; ++i)
        {
            matrix[i].x = -1.f;
            matrix[i].y = -1.f;
            matrix[i].z = -1.f;
        }
    }

    Matrix33 Transposed() const
    {
        return Transpose(*this);
    }

    static Matrix33 Transpose(const Matrix33& matrix)
    {
        return Matrix33(
            matrix.m00, matrix.m10, matrix.m20,
            matrix.m01, matrix.m11, matrix.m21,
            matrix.m02, matrix.m12, matrix.m22
        );
    }

    static Matrix33 GetAdjointMatrix(const Matrix33& matrix)
    {
        return Matrix33(
            matrix.m11 * matrix.m22 - matrix.m12 * matrix.m21,
            -matrix.m01 * matrix.m22 + matrix.m02 * matrix.m21,
            matrix.m01 * matrix.m12 - matrix.m02 * matrix.m11,

            -matrix.m10 * matrix.m22 + matrix.m12 * matrix.m20,
            matrix.m00 * matrix.m22 - matrix.m02 * matrix.m20,
            -matrix.m00 * matrix.m12 + matrix.m02 * matrix.m10,

            matrix.m10 * matrix.m21 - matrix.m11 * matrix.m20,
            -matrix.m00 * matrix.m21 + matrix.m01 * matrix.m20,
            matrix.m00 * matrix.m11 - matrix.m01 * matrix.m10
        );
    }

    Matrix33 Inverted() const
    {
        return Invert(*this);
    }

    static Matrix33 Invert(const Matrix33& matrix)
    {
        float determinant = Determinant(matrix);
        Matrix33 adjointMatrix;

        if (determinant == 0.f)
        {
            const float eps = PSEUDOINVERSE_EPSILON;
            Matrix33 matrix2 = matrix;

            matrix2[0][0] += eps;
            matrix2[1][1] += eps;
            matrix2[2][2] += eps;

            determinant = Determinant(matrix);

            if (determinant == 0.f)
            {
                Unit(matrix2);
                determinant = 1.0f;
            }

            adjointMatrix = GetAdjointMatrix(matrix2);
        }
        else
        {
            adjointMatrix = GetAdjointMatrix(matrix);
        }

        Matrix33 result;

        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                result[j][i] = adjointMatrix[i][j] / determinant;
            }
        }

        return result;
    }

    static Matrix33 ScaleToMatrix(const Vector3& scale)
    {
        return Matrix33(
            scale[0], 0.f, 0.f,
            0.f, scale[1], 0.f,
            0.f, 0.f, scale[2]
        );
    }

    Quaternion MatrixToQuaternion();
    Quaternion NormalizedMatrixToQuaternionWithChecks();
    Quaternion NormalizedMatrixToQuaternion();

    static Matrix33 NormalizedAxisAngleToMatrixEx(const Vector3& axis, const float angleSin, const float angleCos)
    {
        Vector3 nsi;
        float ico;
        float n00, n01, n11, n02, n12, n22;

        ico = 1.0f - angleCos;
        nsi[0] = axis[0] * angleSin;
        nsi[1] = axis[1] * angleSin;
        nsi[2] = axis[2] * angleSin;

        n00 = (axis[0] * axis[0]) * ico;
        n01 = (axis[0] * axis[1]) * ico;
        n11 = (axis[1] * axis[1]) * ico;
        n02 = (axis[0] * axis[2]) * ico;
        n12 = (axis[1] * axis[2]) * ico;
        n22 = (axis[2] * axis[2]) * ico;

        return Matrix33(
            n00 + angleCos,
            n01 + nsi[2],
            n02 - nsi[1],
            n01 - nsi[2],
            n11 + angleCos,
            n12 + nsi[0],
            n02 + nsi[1],
            n12 - nsi[0],
            n22 + angleCos
        );
    }

    static Matrix33 NormalizedAxisAngleToMatrix(const Vector3& axis, const float angle)
    {
        return NormalizedAxisAngleToMatrixEx(axis, sinf(angle), cosf(angle));
    }

    union
    {
        struct
        {
            float m00, m10, m20;
            float m01, m11, m21;
            float m02, m12, m22;
        };

        Vector3 m[3];
    };

    static const Matrix33 Identity;
};
