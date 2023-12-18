#pragma once

#include <numbers>
#include <cmath>
#include <DirectXMath.h>

struct SVector3;
struct SVector4;
struct SMatrix33;
struct SQuaternion;

class Vector3;
class Matrix33;
class Quaternion;

#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880 /* sqrt(2) */
#endif

#ifndef M_SQRT1_2
#define M_SQRT1_2 0.70710678118654752440 /* 1/sqrt(2) */
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923 /* pi/2 */
#endif

/** \note 0.0001 is too small because normals may be converted from short's: see T34322. */
#define UNIT_EPSILON 0.0002f

#define PSEUDOINVERSE_EPSILON 1e-8f

class Math
{
public:
	static float CotF(float x) { return cosf(x) / sinf(x); }

	template <typename T>
	static constexpr T Sqrt(T x)
	{
		return std::sqrt(x);
	}

	template <typename T>
	static constexpr T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	static constexpr T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}

	template <typename T>
	constexpr T Max3(T a, T b, T c)
	{
		return Max(a, Max(b, c));
	}

	template <typename T>
	constexpr T Min3(T a, T b, T c)
	{
		return Min(a, Min(b, c));
	}

	template <typename T>
	static constexpr T Clamp(T value, T min, T max)
	{
		return Max(min, Min(max, value));
	}

	template <typename T>
	static constexpr T ASin(const T factor)
	{
		if (factor <= -1.0f)
		{
			return static_cast<float>(-M_PI_2);
		}
		else if (factor >= 1.0f)
		{
			return static_cast<float>(M_PI_2);
		}
		else
		{
			return std::asin(factor);
		}
	}

	template <typename T>
	static constexpr T Floor(T x)
	{
		return floor(x);
	}

	template <typename T>
	static constexpr T Ceil(T x)
	{
		return ceil(x);
	}

	template <typename T>
	static constexpr T Round(T x)
	{
		return round(x);
	}

	template <typename T>
	static constexpr T Tan(T x)
	{
		return tan(x);
	}

	template <typename T>
	static constexpr T Cos(T x)
	{
		return cos(x);
	}

	template <typename T>
	static constexpr T Sin(T x)
	{
		return sin(x);
	}

	template <typename T>
	static constexpr int Sign(T x)
	{
		return (static_cast<T>(0) < x) - (x < static_cast<T>(0));
	}

	template <typename T>
	static constexpr T Pow(T x, T y)
	{
		return pow(x, y);
	}

	template <typename T>
	static constexpr T Log(T x)
	{
		return log(x);
	}

	// Divides two integers and rounds to nearest
	template <typename T>
	static constexpr T DivideAndRoundNearest(T Dividend, T Divisor)
	{
		return (Dividend >= 0)
			? (Dividend + Divisor / 2) / Divisor
			: (Dividend - Divisor / 2 + 1) / Divisor;
	}

	static constexpr int TruncToInt32(float F)
	{
		return (int)F;
	}

	static constexpr int TruncToInt32(double F)
	{
		return (int)F;
	}

	static constexpr long long TruncToInt64(double F)
	{
		return (long long)F;
	}

	static constexpr int TruncToInt(float F)
	{
		return TruncToInt32(F);
	}

	static constexpr long long TruncToInt(double F)
	{
		return TruncToInt64(F);
	}

	static int FloorToInt32(float F)
	{
		int I = TruncToInt32(F);
		I -= ((float)I > F);

		return I;
	}
	static int FloorToInt32(double F)
	{
		int I = TruncToInt32(F);
		I -= ((double)I > F);

		return I;
	}
	static long long FloorToInt64(double F)
	{
		long long I = TruncToInt64(F);
		I -= ((double)I > F);

		return I;
	}

	static int FloorToInt(float F)
	{
		return FloorToInt32(F);
	}

	static long long FloorToInt(double F)
	{
		return FloorToInt64(F);
	}

	static int RoundToInt32(float F)
	{
		return FloorToInt32(F + 0.5f);
	}

	static int RoundToInt32(double F)
	{
		return FloorToInt32(F + 0.5);
	}

	static long long RoundToInt64(double F)
	{
		return FloorToInt64(F + 0.5);
	}

	static int RoundToInt(float F)
	{
		return RoundToInt32(F);
	}

	static long long RoundToInt(double F)
	{
		return RoundToInt64(F);
	}

	static SVector3 NormalizeVector(const SVector4& vector);

	static Vector3 ConvertDegressToRadians(const Vector3& degrees);
	static Vector3 ConvertRadiansToDegress(const Vector3& radians);

	static SMatrix33 SwapScaleRotationMatrix(const SQuaternion& rotation);
	static SMatrix33 GetNearbySignedPermutationMatrix(const SQuaternion& rotation);
};
