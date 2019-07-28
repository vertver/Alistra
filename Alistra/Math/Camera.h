#pragma once
#include "Base_Math.h"

MATRIX4 OrtographicLH(float ViewWidth, float ViewHeight, float NearZ, float FarZ);
MATRIX4 OrtographicRH(float ViewWidth, float ViewHeight, float NearZ, float FarZ);
MATRIX4 OrtographicOffCenterLH(float ViewLeft, float ViewRight, float ViewBottom, float ViewTop, float NearZ, float FarZ);
MATRIX4 OrtographicOffCenterRH(float ViewLeft, float ViewRight, float ViewBottom, float ViewTop, float NearZ, float FarZ);