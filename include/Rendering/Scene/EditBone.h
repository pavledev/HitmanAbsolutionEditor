#pragma once

#include "Glacier/Math/SVector3.h"

struct EditBone
{
	SVector3 headPosition;
	SVector3 tailPosition;
	float length;
	EditBone* parent;

	/*
	* Roll along axis. We'll ultimately use the axis/angle method
	* for determining the transformation matrix of the bone. The axis
	* is tail-head while roll provides the angle.
	*/
	float roll;
};
