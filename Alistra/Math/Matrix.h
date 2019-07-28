#pragma once
#include "Base_Math.h"

MATRIX4 Matrix4Identity();
MATRIX4 MATRIX4Transpose(MATRIX4 *m);
MATRIX4 MATRIX4RotationX(float angle);
MATRIX4 MATRIX4RotationY(float angle);
MATRIX4 Matrix4LookAtLH(VECTOR *eyePos, VECTOR *focusPos, VECTOR *UpDirection);
MATRIX4 Matrix4LookToLH(VECTOR *eyePos, VECTOR *eyeDir, VECTOR *UpDirection);
MATRIX4 Matrix4Translation(float OffsetX, float OffsetY, float OffsetZ);
MATRIX4 Matrix4Multiply(MATRIX4 *m1, const MATRIX4 *m2);