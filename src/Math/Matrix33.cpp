#include "Math/Matrix33.h"
#include "Math/Matrix44.h"

const Matrix33 Matrix33::Identity
(
    1, 0, 0,
    0, 1, 0,
    0, 0, 1
);

Matrix33::Matrix33(const Matrix44& matrix)
{
    this->m00 = matrix.m00; this->m01 = matrix.m01; this->m02 = matrix.m02;
    this->m10 = matrix.m10; this->m11 = matrix.m11; this->m12 = matrix.m12;
    this->m20 = matrix.m20; this->m21 = matrix.m21; this->m22 = matrix.m22;
}

Quaternion Matrix33::MatrixToQuaternion()
{
    Normalize();

    return NormalizedMatrixToQuaternionWithChecks();
}

Quaternion Matrix33::NormalizedMatrixToQuaternionWithChecks()
{
    const float determinant = Determinant();

    if (!isfinite(determinant))
    {
        Unit();
    }
    else if (determinant < 0.f)
    {
        Negate();
    }

    return NormalizedMatrixToQuaternion();
}

Quaternion Matrix33::NormalizedMatrixToQuaternion()
{
    const Matrix33& matrix = *this;
    Quaternion quaternion;
    const float trace = matrix.m00 + matrix.m11 + matrix.m22;

    if (trace > 0)
    {
        float s = 2.0f * sqrtf(1.0f + trace);

        quaternion.w = 0.25f * s;

        s = 1.0f / s;

        quaternion.x = (matrix.m12 - matrix.m21) * s;
        quaternion.y = (matrix.m20 - matrix.m02) * s;
        quaternion.z = (matrix.m01 - matrix.m10) * s;
    }
    else
    {
        /* Find the biggest diagonal element to choose the best formula.
         * Here trace should also be always >= 0, avoiding bad precision. */
        if (matrix.m00 > matrix.m11 && matrix.m00 > matrix.m22)
        {
            float s = 2.0f * sqrtf(1.0f + matrix.m00 - matrix.m11 - matrix.m22);

            quaternion.x = 0.25f * s;

            s = 1.0f / s;

            quaternion.w = (matrix.m12 - matrix.m21) * s;
            quaternion.y = (matrix.m10 + matrix.m01) * s;
            quaternion.z = (matrix.m20 + matrix.m02) * s;
        }
        else if (matrix.m11 > matrix.m22)
        {
            float s = 2.0f * sqrtf(1.0f + matrix.m11 - matrix.m00 - matrix.m22);

            quaternion.y = 0.25f * s;

            s = 1.0f / s;

            quaternion.w = (matrix.m20 - matrix.m02) * s;
            quaternion.x = (matrix.m10 + matrix.m01) * s;
            quaternion.z = (matrix.m21 + matrix.m12) * s;
        }
        else
        {
            float s = 2.0f * sqrtf(1.0f + matrix.m22 - matrix.m00 - matrix.m11);

            quaternion.z = 0.25f * s;

            s = 1.0f / s;

            quaternion.w = (matrix.m01 - matrix.m10) * s;
            quaternion.x = (matrix.m20 + matrix.m02) * s;
            quaternion.y = (matrix.m21 + matrix.m12) * s;
        }

        /* Make sure W is non-negative for a canonical result. */
        if (quaternion.w < 0)
        {
            quaternion.Negate();
        }
    }

    Quaternion::Normalize(quaternion);

    return quaternion;
}
