#include "Glacier/Math/SMatrix33.h"

#include "Rendering/Scene/VirtualNode.h"
#include "Utility/Math.h"

void VirtualNode::GetTransform(SVector3& basePosition, SQuaternion& baseRotation, SVector3& baseScale) const
{
    //(final TRS) = (rotation after) (base TRS) (rotation before)
    SMatrix33 matrix = Math::SwapScaleRotationMatrix(rotationBefore);

    basePosition = rotationAfter * this->basePosition;
    baseRotation = rotationAfter * this->baseRotation * rotationBefore;
    baseScale = matrix * this->baseScale;
}
