#include "Matrix.h"
#include "Base_Math.h"
#include "Misc_Math.h"
#include "Vector.h"

MATRIX4 Matrix4Identity()
{
    MATRIX4 m;

    m.r[0] = (VECTOR)   {{{1.0f, 0.0f, 0.0f, 0.0f}}};
    m.r[1] = (VECTOR)   {{{0.0f, 1.0f, 0.0f, 0.0f}}};
    m.r[2] = (VECTOR)   {{{0.0f, 0.0f, 1.0f, 0.0f}}};
    m.r[3] = (VECTOR)   {{{0.0f, 0.0f, 0.0f, 1.0f}}};

    return m;
}

MATRIX4 MATRIX4Transpose(MATRIX4* m)
{
    MATRIX4 p;
    p.r[0] = VECTORMergeXY(&m->r[0], &m->r[2]); // m00m20m01m21
    p.r[1] = VECTORMergeXY(&m->r[1], &m->r[3]); // m10m30m11m31
    p.r[2] = VECTORMergeZW(&m->r[0], &m->r[2]); // m02m22m03m23
    p.r[3] = VECTORMergeZW(&m->r[1], &m->r[3]); // m12m32m13m33

    MATRIX4 mt;
    mt.r[0] = VECTORMergeXY(&p.r[0], &p.r[1]); // m00m10m20m30
    mt.r[1] = VECTORMergeZW(&p.r[0], &p.r[1]); // m01m11m21m31
    mt.r[2] = VECTORMergeXY(&p.r[2], &p.r[3]); // m02m12m22m32
    mt.r[3] = VECTORMergeZW(&p.r[2], &p.r[3]); // m03m13m23m33
    return mt;
}

MATRIX4 MATRIX4RotationX(float angle)
{
    float    sin_angle;
    float    cos_angle;
    XMScalarSinCos(&sin_angle, &cos_angle, angle);

    MATRIX4 m;
    m.m[0][0] = 1.0f;
    m.m[0][1] = 0.0f;
    m.m[0][2] = 0.0f;
    m.m[0][3] = 0.0f;

    m.m[1][0] = 0.0f;
    m.m[1][1] = cos_angle;
    m.m[1][2] = sin_angle;
    m.m[1][3] = 0.0f;

    m.m[2][0] = 0.0f;
    m.m[2][1] = -sin_angle;
    m.m[2][2] = cos_angle;
    m.m[2][3] = 0.0f;

    m.m[3][0] = 0.0f;
    m.m[3][1] = 0.0f;
    m.m[3][2] = 0.0f;
    m.m[3][3] = 1.0f;
    return m;
}

MATRIX4 MATRIX4RotationY(float angle)
{
    MATRIX4 m;

    float    fSinAngle;
    float    fCosAngle;
    XMScalarSinCos(&fSinAngle, &fCosAngle, angle);

    m.m[0][0] = fCosAngle;
    m.m[0][1] = 0.0f;
    m.m[0][2] = -fSinAngle;
    m.m[0][3] = 0.0f;

    m.m[1][0] = 0.0f;
    m.m[1][1] = 1.0f;
    m.m[1][2] = 0.0f;
    m.m[1][3] = 0.0f;

    m.m[2][0] = fSinAngle;
    m.m[2][1] = 0.0f;
    m.m[2][2] = fCosAngle;
    m.m[2][3] = 0.0f;

    m.m[3][0] = 0.0f;
    m.m[3][1] = 0.0f;
    m.m[3][2] = 0.0f;
    m.m[3][3] = 1.0f;

    return m;
}


MATRIX4 Matrix4LookAtLH(VECTOR* eyePos, VECTOR* focusPos, VECTOR* UpDirection)
{
    VECTOR EyeDir = VECTORSubstract(focusPos, eyePos);

    return Matrix4LookToLH(eyePos, &EyeDir, UpDirection);
}

MATRIX4 Matrix4LookToLH(VECTOR* eyePos, VECTOR* eyeDir, VECTOR* UpDirection)
{
    VECTOR R2 = VECTOR3Normalize(eyeDir);

    VECTOR R0 = VECTOR3Cross(UpDirection, &R2);
    R0 = VECTOR3Normalize(&R0);

    VECTOR R1 = VECTOR3Cross(&R2, &R0);

    VECTOR NegEyePosition = VECTORNegate(eyePos);

    VECTOR D0 = VECTOR3Dot(&R0, &NegEyePosition);
    VECTOR D1 = VECTOR3Dot(&R1, &NegEyePosition);
    VECTOR D2 = VECTOR3Dot(&R2, &NegEyePosition);

    MATRIX4 M;

    VECTORU32 g_XMSelect1110 = { { { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 } } };
    const VECTORF32 g_XMIdentityR3 = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };

    M.r[0] = VECTORSelect(&D0, &R0, &g_XMSelect1110.v);
    M.r[1] = VECTORSelect(&D1, &R1, &g_XMSelect1110.v);
    M.r[2] = VECTORSelect(&D2, &R2, &g_XMSelect1110.v);
    M.r[3] = g_XMIdentityR3.v;

    M = MATRIX4Transpose(&M);

    return M;
}

MATRIX4 Matrix4Translation(float OffsetX, float OffsetY, float OffsetZ)
{
    MATRIX4 m;
    m.m[0][0] = 1.0f;
    m.m[0][1] = 0.0f;
    m.m[0][2] = 0.0f;
    m.m[0][3] = 0.0f;

    m.m[1][0] = 0.0f;
    m.m[1][1] = 1.0f;
    m.m[1][2] = 0.0f;
    m.m[1][3] = 0.0f;

    m.m[2][0] = 0.0f;
    m.m[2][1] = 0.0f;
    m.m[2][2] = 1.0f;
    m.m[2][3] = 0.0f;

    m.m[3][0] = OffsetX;
    m.m[3][1] = OffsetY;
    m.m[3][2] = OffsetZ;
    m.m[3][3] = 1.0f;
    return m;
}

MATRIX4 Matrix4Multiply(MATRIX4* m1, const MATRIX4* m2)
{
    MATRIX4 mResult;
    // Cache the invariants in registers
    float x = m1->m[0][0];
    float y = m1->m[0][1];
    float z = m1->m[0][2];
    float w = m1->m[0][3];
    // Perform the operation on the first row
    mResult.m[0][0] = (m2->m[0][0] * x) + (m2->m[1][0] * y) + (m2->m[2][0] * z) + (m2->m[3][0] * w);
    mResult.m[0][1] = (m2->m[0][1] * x) + (m2->m[1][1] * y) + (m2->m[2][1] * z) + (m2->m[3][1] * w);
    mResult.m[0][2] = (m2->m[0][2] * x) + (m2->m[1][2] * y) + (m2->m[2][2] * z) + (m2->m[3][2] * w);
    mResult.m[0][3] = (m2->m[0][3] * x) + (m2->m[1][3] * y) + (m2->m[2][3] * z) + (m2->m[3][3] * w);
    // Repeat for all the other rows
    x = m1->m[1][0];
    y = m1->m[1][1];
    z = m1->m[1][2];
    w = m1->m[1][3];
    mResult.m[1][0] = (m2->m[0][0] * x) + (m2->m[1][0] * y) + (m2->m[2][0] * z) + (m2->m[3][0] * w);
    mResult.m[1][1] = (m2->m[0][1] * x) + (m2->m[1][1] * y) + (m2->m[2][1] * z) + (m2->m[3][1] * w);
    mResult.m[1][2] = (m2->m[0][2] * x) + (m2->m[1][2] * y) + (m2->m[2][2] * z) + (m2->m[3][2] * w);
    mResult.m[1][3] = (m2->m[0][3] * x) + (m2->m[1][3] * y) + (m2->m[2][3] * z) + (m2->m[3][3] * w);
    x = m1->m[2][0];
    y = m1->m[2][1];
    z = m1->m[2][2];
    w = m1->m[2][3];
    mResult.m[2][0] = (m2->m[0][0] * x) + (m2->m[1][0] * y) + (m2->m[2][0] * z) + (m2->m[3][0] * w);
    mResult.m[2][1] = (m2->m[0][1] * x) + (m2->m[1][1] * y) + (m2->m[2][1] * z) + (m2->m[3][1] * w);
    mResult.m[2][2] = (m2->m[0][2] * x) + (m2->m[1][2] * y) + (m2->m[2][2] * z) + (m2->m[3][2] * w);
    mResult.m[2][3] = (m2->m[0][3] * x) + (m2->m[1][3] * y) + (m2->m[2][3] * z) + (m2->m[3][3] * w);
    x = m1->m[3][0];
    y = m1->m[3][1];
    z = m1->m[3][2];
    w = m1->m[3][3];
    mResult.m[3][0] = (m2->m[0][0] * x) + (m2->m[1][0] * y) + (m2->m[2][0] * z) + (m2->m[3][0] * w);
    mResult.m[3][1] = (m2->m[0][1] * x) + (m2->m[1][1] * y) + (m2->m[2][1] * z) + (m2->m[3][1] * w);
    mResult.m[3][2] = (m2->m[0][2] * x) + (m2->m[1][2] * y) + (m2->m[2][2] * z) + (m2->m[3][2] * w);
    mResult.m[3][3] = (m2->m[0][3] * x) + (m2->m[1][3] * y) + (m2->m[2][3] * z) + (m2->m[3][3] * w);
    return mResult;
}

