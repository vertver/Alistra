#pragma once
#include <stdint.h>

typedef struct
{
    union
    {
        float       vector4_f32[4];
        uint32_t    vector4_u32[4];
    };
} VECTOR;

typedef struct
{
    union
    {
        float f[4];
        VECTOR v;
    };
} VECTORF32;

__declspec(align(16)) typedef struct
{
    union
    {
        uint32_t u[4];
        VECTOR v;
    };
} VECTORU32;

__declspec(align(16)) typedef struct
{
    union
    {
        VECTOR r[4];
        struct
        {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float m[4][4];
    };
} MATRIX4;

typedef struct  
{
    float x;
    float y;
    float z;
} FLOAT3;

typedef struct  
{
    float x;
    float y;
    float z;
    float w;
} FLOAT4;