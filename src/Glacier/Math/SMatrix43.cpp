#include <DirectXMath.h>
#include <cmath>

#include "Glacier/Math/SMatrix43.h"
#include "Utility/Math.h"

SMatrix43::SMatrix43()
{
}

SMatrix43::SMatrix43(const SMatrix43& other)
{
	Rot = other.Rot;
	Trans = other.Trans;
}

SMatrix43& SMatrix43::operator=(const SMatrix43& other)
{
	if (this != &other)
	{
		Rot = other.Rot;
		Trans = other.Trans;
	}

	return *this;
}

void SMatrix43::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("Rot");
    //Rot.ConvertRotationMatrixToEulerAngles().SerializeToJson(writer);
    Rot.SerializeToJson(writer);

    writer.String("Trans");
    Trans.SerializeToJson(writer);

    writer.EndObject();
}

void SMatrix43::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
    unsigned int rotOffset = offset + offsetof(SMatrix43, Rot);
    unsigned int transOffset = offset + offsetof(SMatrix43, Trans);

    Rot.SerializeToMemory(binarySerializer, rotOffset);
    Trans.SerializeToMemory(binarySerializer, transOffset);
}

SMatrix43* SMatrix43::DeserializeFromJson(const rapidjson::Value& object)
{
    SMatrix43* matrix = new SMatrix43();

    //matrix->Rot = SMatrix33::ConvertEulerAnglesToRotationMatrix(*SVector3::DeserializeFromJson(object["Rot"].GetObj()));
    matrix->Rot = *SMatrix33::DeserializeFromJson(object["Rot"].GetObj());
    matrix->Trans = *SVector3::DeserializeFromJson(object["Trans"].GetObj());

    return matrix;
}

bool SMatrix43::operator==(const SMatrix43& other) const
{
    return Rot == other.Rot && Trans == other.Trans;
}

void SMatrix43::Decompose(SVector3& position, SVector3& rotation, SVector3& scale)
{
	SMatrix43 matrix = *this;
	float n11 = matrix.XAxis.x;
	float n12 = matrix.XAxis.y;
	float n13 = matrix.XAxis.z;
	float n14 = 0.f;
	float n21 = matrix.YAxis.x;
	float n22 = matrix.YAxis.y;
	float n23 = matrix.YAxis.z;
	float n24 = 0.f;
	float n31 = matrix.ZAxis.x;
	float n32 = matrix.ZAxis.y;
	float n33 = matrix.ZAxis.z;
	float n34 = 0.f;
	float n41 = matrix.Trans.x;
	float n42 = matrix.Trans.y;
	float n43 = matrix.Trans.z;
	float n44 = 1.f;

	float det =
		n41 * (n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34
			- n12 * n23 * n34) + n42
		* (n11 * n23 * n34 - n11 * n24 * n33 + n14 * n21 * n33 - n13 * n21 * n34 + n13 * n24 * n31
			- n14 * n23 * n31) + n43
		* (n11 * n24 * n32 - n11 * n22 * n34 - n14 * n21 * n32 + n12 * n21 * n34 + n14 * n22 * n31
			- n12 * n24 * n31) + n44
		* (-n13 * n22 * n31 - n11 * n23 * n32 + n11 * n22 * n33 + n13 * n21 * n32 - n12 * n21 * n33
			+ n12 * n23 * n31);

	float sx = n11 * n11 + n21 * n21 + n31 * n31;
	float sy = n12 * n12 + n22 * n22 + n32 * n32;
	float sz = n13 * n13 + n23 * n23 + n33 * n33;

	if (det < 0.f)
	{
		sx = -sx;
	}

	float invSx = 1.f / sx;
	float invSy = 1.f / sy;
	float invSz = 1.f / sz;

	matrix.XAxis.x *= invSx;
	matrix.YAxis.x *= invSx;
	matrix.ZAxis.x *= invSx;
	matrix.XAxis.y *= invSy;
	matrix.YAxis.y *= invSy;
	matrix.ZAxis.y *= invSy;
	matrix.XAxis.z *= invSz;
	matrix.YAxis.z *= invSz;
	matrix.ZAxis.z *= invSz;

	double rotationX, rotationY, rotationZ;
	const double radToDeg = 180.f / DirectX::XM_PI;

	if (std::abs(matrix.XAxis.z) < 0.9999999)
	{
		rotationX = std::atan2(-matrix.YAxis.z, matrix.ZAxis.z) * radToDeg;
		rotationZ = std::atan2(-matrix.XAxis.y, matrix.XAxis.x) * radToDeg;
	}
	else
	{
		rotationX = std::atan2(matrix.ZAxis.y, matrix.YAxis.y) * radToDeg;
		rotationZ = 0.f;
	}

	rotationY = std::asin(Math::Clamp(matrix.XAxis.z, -1.0f, 1.0f)) * radToDeg;

	position.x = n41;
	position.y = n42;
	position.z = n43;

	rotation.x = static_cast<float>(rotationX);
	rotation.y = static_cast<float>(rotationY);
	rotation.z = static_cast<float>(rotationZ);

	scale.x = sx;
	scale.y = sy;
	scale.z = sz;
}

SMatrix43 SMatrix43::Recompose(SVector3& position, SVector3& rotation, SVector3& scale)
{
	SMatrix43 matrix;
	const double degToRad = DirectX::XM_PI / 180.0f;

	double x = rotation.x * degToRad;
	double y = rotation.y * degToRad;
	double z = rotation.z * degToRad;

	double c1 = std::cos(x / 2.0);
	double c2 = std::cos(y / 2.0);
	double c3 = std::cos(z / 2.0);

	double s1 = std::sin(x / 2.0);
	double s2 = std::sin(y / 2.0);
	double s3 = std::sin(z / 2.0);

	double quatX = s1 * c2 * c3 + c1 * s2 * s3;
	double quatY = c1 * s2 * c3 - s1 * c2 * s3;
	double quatZ = c1 * c2 * s3 + s1 * s2 * c3;
	double quatW = c1 * c2 * c3 - s1 * s2 * s3;

	double x2 = quatX + quatX;
	double y2 = quatY + quatY;
	double z2 = quatZ + quatZ;
	double xx = quatX * x2;
	double xy = quatX * y2;
	double xz = quatX * z2;
	double yy = quatY * y2;
	double yz = quatY * z2;
	double zz = quatZ * z2;
	double wx = quatW * x2;
	double wy = quatW * y2;
	double wz = quatW * z2;

	float sx = scale.x;
	float sy = scale.y;
	float sz = scale.z;

	matrix.XAxis.x = static_cast<float>((1.0 - (yy + zz)) * sx);
	matrix.XAxis.y = static_cast<float>((xy - wz) * sy);
	matrix.XAxis.z = static_cast<float>((xz + wy) * sz);

	matrix.YAxis.x = static_cast<float>((xy + wz) * sx);
	matrix.YAxis.y = static_cast<float>((1.0 - (xx + zz)) * sy);
	matrix.YAxis.z = static_cast<float>((yz - wx) * sz);

	matrix.ZAxis.x = static_cast<float>((xz - wy) * sx);
	matrix.ZAxis.y = static_cast<float>((yz + wx) * sy);
	matrix.ZAxis.z = static_cast<float>((1.0 - (xx + yy)) * sz);

	matrix.Trans.x = position.x;
	matrix.Trans.y = position.y;
	matrix.Trans.z = position.z;

	return matrix;
}
