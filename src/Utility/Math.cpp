#include "Glacier/Math/SVector4.h"
#include "Glacier/Math/SQuaternion.h"
#include "Glacier/Math/SMatrix33.h"

#include "Utility/Math.h"
#include "Math/Vector3.h"

SVector3 Math::NormalizeVector(const SVector4& vector)
{
	double magnitude = Sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);

	return SVector3(static_cast<float>(vector.x / magnitude), static_cast<float>(vector.y / magnitude), static_cast<float>(vector.z / magnitude));
}

Vector3 Math::ConvertDegressToRadians(const Vector3& degrees)
{
	return Vector3(DirectX::XMConvertToRadians(degrees.x), DirectX::XMConvertToRadians(degrees.y), DirectX::XMConvertToRadians(degrees.z));
}

Vector3 Math::ConvertRadiansToDegress(const Vector3& radians)
{
	return Vector3(DirectX::XMConvertToDegrees(radians.x), DirectX::XMConvertToDegrees(radians.y), DirectX::XMConvertToDegrees(radians.z));
}

SMatrix33 Math::SwapScaleRotationMatrix(const SQuaternion& rotation)
{
	/*
	* Returns a matrix m st. Scale[s] Rot[rot] = Rot[rot] Scale[m s].
	* If rot.to_matrix() is a signed permutation matrix, works for any s.
	* Otherwise works only if s is a uniform scaling.
	*/
	SMatrix33 matrix = GetNearbySignedPermutationMatrix(rotation); //Snap to signed perm matrix
	SMatrix33::Transpose(matrix); //Invert permutation

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			matrix[i][j] = abs(matrix[i][j]); //Discard sign
		}
	}

	return matrix;
}

SMatrix33 Math::GetNearbySignedPermutationMatrix(const SQuaternion& rotation)
{
	/*
	* Returns a signed permutation matrix close to rot.to_matrix().
	* (A signed permutation matrix is like a permutation matrix, except
	* the non - zero entries can be ±1.)
	*/

	SMatrix33 m = rotation.ToMatrix();
	SVector3& x = m[0];
	SVector3& y = m[1];
	SVector3& z = m[2];

	//Set the largest entry in the first row to ±1
	float a = abs(x[0]);
	float b = abs(x[1]);
	float c = abs(x[2]);

	unsigned int i;

	if (a >= b && a >= c)
	{
		i = 0;
	}
	else if (b >= c)
	{
		i = 1;
	}
	else
	{
		i = 2;
	}

	if (x[i] > 0)
	{
		x[i] = 1;
	}
	else
	{
		x[i] = -1;
	}

	x[(i + 1) % 3] = 0;
	x[(i + 2) % 3] = 0;

	//Same for second row : only two columns to consider now.
	a = abs(y[(i + 1) % 3]);
	b = abs(y[(i + 2) % 3]);

	unsigned int j;

	if (a >= b)
	{
		j = (i + 1) % 3;
	}
	else
	{
		j = (i + 2) % 3;
	}

	if (y[j] > 0)
	{
		y[j] = 1;
	}
	else
	{
		y[j] = -1;
	}

	y[(j + 1) % 3] = 0;
	y[(j + 2) % 3] = 0;

	//Same for third row : only one column left
	unsigned int k = (0 + 1 + 2) - i - j;

	if (z[k] > 0)
	{
		z[k] = 1;
	}
	else
	{
		z[k] = -1;
	}

	z[(k + 1) % 3] = 0;
	z[(k + 2) % 3] = 0;

	return m;
}
