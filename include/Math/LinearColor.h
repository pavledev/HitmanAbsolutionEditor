#pragma once

#include "Color.h"

class LinearColor
{
public:
	LinearColor() = default;
	LinearColor(const float r, const float g, const float b, const float a = 1.f);
	LinearColor(const Color& Color);
	const float* Data() const;
	const bool operator==(const LinearColor& rhs) const;
	const bool operator!=(const LinearColor& rhs) const;

	/** Quantizes the linear color with rounding and returns the result as a FColor.  This bypasses the SRGB conversion.
	* QuantizeRound can be dequantized back to linear with FColor::ReinterpretAsLinear (just /255.f)
	* this matches the GPU UNORM<->float conversion spec and should be preferred
	*/
	Color QuantizeRound() const;

	/** Quantizes the linear color and returns the result as a FColor with optional sRGB conversion.
	* Clamps in [0,1] range before conversion.
	* ToFColor(false) is QuantizeRound
	*/
	Color ToFColorSRGB() const;

	Color ToColor(const bool bSRGB) const;

	/**
	 * Helper for pixel format conversions. Clamps to [0,1], mapping NaNs to 0,
	 * for consistency with GPU conversions.
	 *
	 * @param InValue The input value.
	 * @return InValue clamped to [0,1]. NaNs map to 0.
	 */
	static float Clamp01NansTo0(const float InValue);

	float r;
	float g;
	float b;
	float a;

	/** Static lookup table used for FColor -> FLinearColor conversion. sRGB */
	static float sRGBToLinearTable[256];
};
