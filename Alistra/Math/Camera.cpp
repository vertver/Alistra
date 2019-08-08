#include "Camera.h"
#include "Vector.h"

MATRIX4 OrtographicLH(float ViewWidth, float ViewHeight, float NearZ, float FarZ)
{
    const float range = 1.0f / (FarZ - NearZ);
    MATRIX4 m;
    m.m[0][0] = 2.0f / ViewWidth;
    m.m[0][1] = 0.0f;
    m.m[0][2] = 0.0f;
    m.m[0][3] = 0.0f;

    m.m[1][0] = 0.0f;
    m.m[1][1] = 2.0f / ViewHeight;
    m.m[1][2] = 0.0f;
    m.m[1][3] = 0.0f;

    m.m[2][0] = 0.0f;
    m.m[2][1] = 0.0f;
    m.m[2][2] = range;
    m.m[2][3] = 0.0f;

    m.m[3][0] = 0.0f;
    m.m[3][1] = 0.0f;
    m.m[3][2] = -range * NearZ;
    m.m[3][3] = 1.0f;
    return m;
}

MATRIX4 OrtographicRH(float ViewWidth, float ViewHeight, float NearZ, float FarZ)
{
    const float range = 1.0f / (NearZ - FarZ);

    MATRIX4 m;
    m.m[0][0] = 2.0f / ViewWidth;
    m.m[0][1] = 0.0f;
    m.m[0][2] = 0.0f;
    m.m[0][3] = 0.0f;

    m.m[1][0] = 0.0f;
    m.m[1][1] = 2.0f / ViewHeight;
    m.m[1][2] = 0.0f;
    m.m[1][3] = 0.0f;

    m.m[2][0] = 0.0f;
    m.m[2][1] = 0.0f;
    m.m[2][2] = range;
    m.m[2][3] = 0.0f;

    m.m[3][0] = 0.0f;
    m.m[3][1] = 0.0f;
    m.m[3][2] = range * NearZ;
    m.m[3][3] = 1.0f;
    return m;
}

MATRIX4 OrtographicOffCenterLH(float ViewLeft, float ViewRight, float ViewBottom, float ViewTop, float NearZ,
    float FarZ)
{
    const float ReciprocalWidth = 1.0f / (ViewRight - ViewLeft);
    const float ReciprocalHeight = 1.0f / (ViewTop - ViewBottom);
    const float range = 1.0f / (FarZ - NearZ);

    MATRIX4 m;
    m.m[0][0] = ReciprocalWidth + ReciprocalWidth;
    m.m[0][1] = 0.0f;
    m.m[0][2] = 0.0f;
    m.m[0][3] = 0.0f;

    m.m[1][0] = 0.0f;
    m.m[1][1] = ReciprocalHeight + ReciprocalHeight;
    m.m[1][2] = 0.0f;
    m.m[1][3] = 0.0f;

    m.m[2][0] = 0.0f;
    m.m[2][1] = 0.0f;
    m.m[2][2] = range;
    m.m[2][3] = 0.0f;

    m.m[3][0] = -(ViewLeft + ViewRight) * ReciprocalWidth;
    m.m[3][1] = -(ViewTop + ViewBottom) * ReciprocalHeight;
    m.m[3][2] = -range * NearZ;
    m.m[3][3] = 1.0f;
    return m;
}

MATRIX4 OrtographicOffCenterRH(float ViewLeft, float ViewRight, float ViewBottom, float ViewTop, float NearZ,
    float FarZ)
{
    const float ReciprocalWidth = 1.0f / (ViewRight - ViewLeft);
    const float ReciprocalHeight = 1.0f / (ViewTop - ViewBottom);
    const float range = 1.0f / (NearZ - FarZ);

    MATRIX4 M;
    M.m[0][0] = ReciprocalWidth + ReciprocalWidth;
    M.m[0][1] = 0.0f;
    M.m[0][2] = 0.0f;
    M.m[0][3] = 0.0f;

    M.m[1][0] = 0.0f;
    M.m[1][1] = ReciprocalHeight + ReciprocalHeight;
    M.m[1][2] = 0.0f;
    M.m[1][3] = 0.0f;

    M.m[2][0] = 0.0f;
    M.m[2][1] = 0.0f;
    M.m[2][2] = range;
    M.m[2][3] = 0.0f;

    M.r[3] = VECTORSet(-(ViewLeft + ViewRight) * ReciprocalWidth,
        -(ViewTop + ViewBottom) * ReciprocalHeight,
        range * NearZ,
        1.0f);
    return M;
}
