#include "Base_Math.h"
#include "Vector.h"
#include <math.h>

VECTOR VECTORSet(float x, float y, float z, float w)
{
    const VECTORF32 vResult = { {{x, y, z, w}} };

    return vResult.v;
}

VECTOR VECTORSubstract(VECTOR *V1, VECTOR *V2)
{
    const VECTORF32 result = { {{
            V1->vector4_f32[0] - V2->vector4_f32[0],
            V1->vector4_f32[1] - V2->vector4_f32[1],
            V1->vector4_f32[2] - V2->vector4_f32[2],
            V1->vector4_f32[3] - V2->vector4_f32[3]
        }} };

    return result.v;
}

VECTOR VECTORMergeXY(VECTOR* V1, VECTOR* V2)
{
    const VECTORU32 Result = { { {
            V1->vector4_u32[0],
            V2->vector4_u32[0],
            V1->vector4_u32[1],
            V2->vector4_u32[1],
        } } };
    return Result.v;
}

VECTOR VECTORMergeZW(VECTOR* V1, VECTOR* V2)
{
    const VECTORU32 Result = { { {
            V1->vector4_u32[2],
            V2->vector4_u32[2],
            V1->vector4_u32[3],
            V2->vector4_u32[3],
        } } };
    return Result.v;
}

VECTOR VECTOR3Dot(VECTOR* V1, VECTOR* V2)
{
    const float value = V1->vector4_f32[0] * V2->vector4_f32[0] + V1->vector4_f32[1] * V2->vector4_f32[1] + V1->vector4_f32[2] * V2->vector4_f32[2];
    VECTORF32 vResult;
    vResult.f[0] =
        vResult.f[1] =
        vResult.f[2] =
        vResult.f[3] = value;
    return vResult.v;
}

VECTOR VECTOR3LengthSq(VECTOR* V)
{
    return VECTOR3Dot(V, V);
}

VECTOR VECTORSqrt(VECTOR* V)
{
    const VECTORF32 result = { { {
            sqrtf(V->vector4_f32[0]),
            sqrtf(V->vector4_f32[1]),
            sqrtf(V->vector4_f32[2]),
            sqrtf(V->vector4_f32[3])
        } } };
    return result.v;
}

VECTOR VECTOR3Length(VECTOR* V)
{
    VECTOR result = VECTOR3LengthSq(V);
    result = VECTORSqrt(&result);



    return result;
}

VECTOR VECTOR3Normalize(VECTOR* V)
{
    VECTOR result = VECTOR3Length(V);
    float length = result.vector4_f32[0];

    // Prevent divide by zero
    if (length > 0) {
        length = 1.0f / length;
    }

    result.vector4_f32[0] = V->vector4_f32[0] * length;
    result.vector4_f32[1] = V->vector4_f32[1] * length;
    result.vector4_f32[2] = V->vector4_f32[2] * length;
    result.vector4_f32[3] = V->vector4_f32[3] * length;
    return result;
}

VECTOR VECTOR3Cross(VECTOR* V1, VECTOR* V2)
{
    const VECTORF32 vResult = { { {
            (V1->vector4_f32[1] * V2->vector4_f32[2]) - (V1->vector4_f32[2] * V2->vector4_f32[1]),
            (V1->vector4_f32[2] * V2->vector4_f32[0]) - (V1->vector4_f32[0] * V2->vector4_f32[2]),
            (V1->vector4_f32[0] * V2->vector4_f32[1]) - (V1->vector4_f32[1] * V2->vector4_f32[0]),
            0.0f
        } } };
    return vResult.v;
}

VECTOR VECTORNegate(VECTOR* V)
{
    const VECTORF32 result = { { {
       -V->vector4_f32[0],
       -V->vector4_f32[1],
       -V->vector4_f32[2],
       -V->vector4_f32[3]
   } } };
    return result.v;
}

VECTOR VECTORSelect(VECTOR* V1, VECTOR* V2, VECTOR* Control)
{
    const VECTORU32 result = { { {
            (V1->vector4_u32[0] & ~Control->vector4_u32[0]) | (V2->vector4_u32[0] & Control->vector4_u32[0]),
            (V1->vector4_u32[1] & ~Control->vector4_u32[1]) | (V2->vector4_u32[1] & Control->vector4_u32[1]),
            (V1->vector4_u32[2] & ~Control->vector4_u32[2]) | (V2->vector4_u32[2] & Control->vector4_u32[2]),
            (V1->vector4_u32[3] & ~Control->vector4_u32[3]) | (V2->vector4_u32[3] & Control->vector4_u32[3]),
        } } };
    return result.v;
}
