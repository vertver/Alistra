/****************************************************************
* MZPE Team, 2019.
* Alistra intro
* License: MIT
*****************************************************************/
#include "Base_Camera.h"

typedef struct  
{
	float f[4];
} XMVECTORF32;

#ifndef XMGLOBALCONST
#define XMGLOBALCONST const __declspec(selectany)
#endif

const float XM_PI = 3.141592654f;
const float XM_2PI = 6.283185307f;
const float XM_1DIVPI = 0.318309886f;
const float XM_1DIV2PI = 0.159154943f;
const float XM_PIDIV2 = 1.570796327f;
const float XM_PIDIV4 = 0.785398163f;

XMGLOBALCONST XMVECTORF32 g_XMIdentityR0 = { 1.0f, 0.0f, 0.0f, 0.0f };
XMGLOBALCONST XMVECTORF32 g_XMIdentityR1 = { 0.0f, 1.0f, 0.0f, 0.0f };
XMGLOBALCONST XMVECTORF32 g_XMIdentityR2 = { 0.0f, 0.0f, 1.0f, 0.0f };
XMGLOBALCONST XMVECTORF32 g_XMIdentityR3 = { 0.0f, 0.0f, 0.0f, 1.0f };

inline float ConvertToRadians(float fDegrees) { return fDegrees * (XM_PI / 180.0f); }
inline float ConvertToDegrees(float fRadians) { return fRadians * (180.0f / XM_PI); }

inline void ScalarSinCos(
	float* pSin,
	float* pCos,
	float  Value
)
{
	assert(pSin);
	assert(pCos);

	// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
	float quotient = XM_1DIV2PI * Value;
	if (Value >= 0.0f)
	{
		quotient = (float)((int)(quotient + 0.5f));
	}
	else
	{
		quotient = (float)((int)(quotient - 0.5f));
	}
	float y = Value - XM_2PI * quotient;

	// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
	float sign;
	if (y > XM_PIDIV2)
	{
		y = XM_PI - y;
		sign = -1.0f;
	}
	else if (y < -XM_PIDIV2)
	{
		y = -XM_PI - y;
		sign = -1.0f;
	}
	else
	{
		sign = +1.0f;
	}

	float y2 = y * y;

	// 11-degree minimax approximation
	*pSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

	// 10-degree minimax approximation
	float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
	*pCos = sign * p;
}

CXMATRIX4X4 
MatrixIdentity()
{
	CXMATRIX4X4  M;

	M._11 = g_XMIdentityR0.f[0];
	M._12 = g_XMIdentityR0.f[1];
	M._13 = g_XMIdentityR0.f[2];
	M._14 = g_XMIdentityR0.f[3];

	M._21 = g_XMIdentityR1.f[0];
	M._22 = g_XMIdentityR1.f[1];
	M._23 = g_XMIdentityR1.f[2];
	M._24 = g_XMIdentityR1.f[3];

	M._31 = g_XMIdentityR0.f[0];
	M._32 = g_XMIdentityR1.f[1];
	M._33 = g_XMIdentityR2.f[2];
	M._34 = g_XMIdentityR3.f[3];

	M._41 = g_XMIdentityR0.f[0];
	M._42 = g_XMIdentityR1.f[1];
	M._43 = g_XMIdentityR2.f[2];
	M._44 = g_XMIdentityR3.f[3];

	return M;
}

boolean            IsOrthographic;
float             Proj_W, Proj_H, Proj_Fov, Proj_Near, Proj_Far, Proj_Scale;

boolean            IsThirdPerson;
CXMATRIX3        View_Pos, View_Angle;
float             View_Distance;

CAMERA_MATRIXS  Matrices;
CAMERA_MATRIXS  MatricesTranspose;

CXMATRIX3        View_UpVector;
CXMATRIX3         View_RightVector;
CXMATRIX3       View_ForwardVector;
CXVECTORF4        View_Dir;

void 
StoreFloat4x4(CXMATRIX4X4* M, CXMATRIX4X4 S)
{
	M->_11 = S._11;
	M->_12 = S._12;
	M->_13 = S._13;
	M->_14 = S._14;

	M->_21 = S._21;
	M->_22 = S._22;
	M->_23 = S._23;
	M->_24 = S._24;

	M->_31 = S._31;
	M->_32 = S._32;
	M->_33 = S._33;
	M->_34 = S._34; 
	
	M->_41 = S._41;
	M->_42 = S._42;
	M->_43 = S._43;
	M->_44 = S._44;
}

CXVECTORF4
LoadFloat3(
	CXMATRIX3* pSource
)
{
	CXVECTORF4 V;
	V.x = pSource->x;
	V.y = pSource->y;
	V.z = pSource->z;
	V.w = 0.f;
	return V;
}

void
StoreFloat3(
	CXMATRIX3* pDestination,
	CXVECTORF4 V
)
{
	pDestination->x = V.x;
	pDestination->y = V.y;
	pDestination->z = V.z;
}

void
InitMatrix()
{
	IsOrthographic = false;
	Proj_W = 1.0f;
	Proj_H = 1.0f;
	Proj_Fov = 90.0f;
	Proj_Near = 0.001f;
	Proj_Far = 1000.0f;
	Proj_Scale = 1.0f;
	IsThirdPerson = false;
	memset(&View_Pos, 0,sizeof(CXMATRIX3));
	memset(&View_Angle, 0,sizeof(CXMATRIX3));
	View_Distance = 0.0f;
	View_UpVector.x = 0; 
	View_UpVector.x = 1; 
	View_UpVector.x = 0; 

	CXMATRIX4X4 TempMatrix = MatrixIdentity();
	StoreFloat4x4(&Matrices.m_V, TempMatrix);
	StoreFloat4x4(&Matrices.m_iV, TempMatrix);
	StoreFloat4x4(&Matrices.m_P, TempMatrix);
	StoreFloat4x4(&Matrices.m_iP, TempMatrix);
	StoreFloat4x4(&Matrices.m_VP, TempMatrix);
	StoreFloat4x4(&Matrices.m_iVP, TempMatrix);
	StoreFloat4x4(&Matrices.m_W, TempMatrix);
	StoreFloat4x4(&Matrices.m_iW, TempMatrix);
	StoreFloat4x4(&Matrices.m_WV, TempMatrix);
	StoreFloat4x4(&Matrices.m_WVP, TempMatrix);

	Matrices.CameraPos.x = 0.0f;
	Matrices.CameraPos.y = 0.0f;
	Matrices.CameraPos.z = 0.0f;
	Matrices.CameraPos.w = 0.0f;

	Matrices.Resolution.x = 1.0f;
	Matrices.Resolution.y = 1.0f;
	Matrices.Resolution.z = 1.0f;
	Matrices.Resolution.w = 1.0f;

	StoreFloat4x4(&MatricesTranspose.m_V, TempMatrix);
	StoreFloat4x4(&MatricesTranspose.m_iV, TempMatrix);
	StoreFloat4x4(&MatricesTranspose.m_P, TempMatrix);
	StoreFloat4x4(&MatricesTranspose.m_iP, TempMatrix);
	StoreFloat4x4(&MatricesTranspose.m_VP, TempMatrix);
	StoreFloat4x4(&MatricesTranspose.m_iVP, TempMatrix);
	StoreFloat4x4(&MatricesTranspose.m_W, TempMatrix);
	StoreFloat4x4(&MatricesTranspose.m_iW, TempMatrix);
	StoreFloat4x4(&MatricesTranspose.m_WV, TempMatrix);
	StoreFloat4x4(&MatricesTranspose.m_WVP, TempMatrix);

	View_UpVector.x = 0.0f; 
	View_UpVector.y = 1.0f;
	View_UpVector.z = 0.0f;
	View_RightVector.x = 1.0f;
	View_RightVector.y = 0.0f;
	View_RightVector.z = 0.0f;
	View_ForwardVector.x = 0.0f;
	View_ForwardVector.y = 0.0f;
	View_ForwardVector.z = 1.0f;
}

void
SetProjPerspective(float W_, float H_, float Fov_, float Near_, float  Far_)
{
	IsOrthographic = false;
	Proj_W = W_;
	Proj_H = H_;
	Proj_Fov = Fov_;
	Proj_Near = Near_;
	Proj_Far = Far_;
	Proj_Scale = 1.0f;
}

void 
SetProjOrthographic(float W_, float H_, float Scale_, float Near_, float Far_)
{
	IsOrthographic = true;
	Proj_W = W_;
	Proj_H = H_;
	Proj_Fov = 90.0f;
	Proj_Near = Near_;
	Proj_Far = Far_;
	Proj_Scale = Scale_;
}

void         
SetThirdPersonView(CXMATRIX3* Pos_, CXMATRIX3* Angle_, float Distance_)
{
	IsThirdPerson = true;
	View_Pos = *Pos_;
	View_Angle = *Angle_;
	View_Distance = max(Distance_, 0.0001f);
}

void         
SetFirstPersonView(CXMATRIX3* Pos_, CXMATRIX3* Angle_)
{
	IsThirdPerson = false;
	View_Pos = *Pos_;
	View_Angle = *Angle_;
	View_Distance = 0.0001f;
}

inline boolean ScalarNearEqual
(
	float S1,
	float S2,
	float Epsilon
)
{
	float Delta = S1 - S2;
	return (fabsf(Delta) <= Epsilon);
}

CXMATRIX4X4
MatrixOrthographicLH(
	float ViewWidth,
	float ViewHeight,
	float NearZ,
	float FarZ
)
{	
	CXMATRIX4X4 M;

	(!ScalarNearEqual(ViewWidth, 0.0f, 0.00001f));
	(!ScalarNearEqual(ViewHeight, 0.0f, 0.00001f));
	(!ScalarNearEqual(FarZ, NearZ, 0.00001f));

	float fRange = 1.0f / (FarZ - NearZ);

	M._11 = 2.0f / ViewWidth;
	M._12 = 0.0f;
	M._13 = 0.0f;
	M._14 = 0.0f;

	M._21 = 0.0f;
	M._22 = 2.0f / ViewHeight;
	M._23 = 0.0f;
	M._24 = 0.0f;

	M._31 = 0.0f;
	M._32 = 0.0f;
	M._33 = fRange;
	M._34 = 0.0f;

	M._41 = 0.0f;
	M._42 = 0.0f;
	M._43 = -fRange * NearZ;
	M._44 = 1.0f;
	return M;
}

CXMATRIX4X4 
MatrixPerspectiveFovLH(
	float FovAngleY,
	float AspectRatio,
	float NearZ,
	float FarZ
)
{

	float    SinFov;
	float    CosFov;
	ScalarSinCos(&SinFov, &CosFov, 0.5f * FovAngleY);

	float Height = CosFov / SinFov;
	float Width = Height / AspectRatio;
	float fRange = FarZ / (FarZ - NearZ);

	CXMATRIX4X4  M;
	M._11 = Width;
	M._12 = 0.0f;
	M._13 = 0.0f;
	M._14 = 0.0f;

	M._21 = 0.0f;
	M._22 = Height;
	M._23 = 0.0f;
	M._24 = 0.0f;

	M._31 = 0.0f;
	M._32 = 0.0f;
	M._33 = fRange;
	M._34 = 1.0f;

	M._41 = 0.0f;
	M._42 = 0.0f;
	M._43 = -fRange * NearZ;
	M._44 = 0.0f;
	return M;
}

/*
	XMMATRIX - CXMATRIX4X4 
	XMVECTOR - CXMATRIX4
*/
CXMATRIX4U VectorMergeXY
(
	CXMATRIX4U V1,
	CXMATRIX4U V2
)
{
	CXMATRIX4U Result;
	Result.x = V1.x;
	Result.y = V2.x;
	Result.z = V1.y;
	Result.w = V2.y;

	return Result;
}

CXMATRIX4U VectorMergeZW
(
	CXMATRIX4U V1,
	CXMATRIX4U V2
)
{
	CXMATRIX4U Result;
	Result.x = V1.z;
	Result.y = V2.z;
	Result.z = V1.w;
	Result.w = V2.w;

	return Result;
}

CXMATRIX4X4 MatrixTranspose(
	CXMATRIX4X4 M
)
{
	CXMATRIX4X4  P;
	CXMATRIX4X4  MT;
	CXMATRIX4U tempVector1;
	CXMATRIX4U tempVector2;
	CXMATRIX4U tempVector3;

	{
		///////////////////////////////////////
		tempVector1.x = M._11;
		tempVector1.y = M._12;
		tempVector1.z = M._13;
		tempVector1.w = M._14;

		tempVector2.x = M._31;
		tempVector2.y = M._32;
		tempVector2.z = M._33;
		tempVector2.w = M._34;

		tempVector3 = VectorMergeXY(tempVector1, tempVector2); // m00m20m01m21
		P._11 = tempVector3.x;
		P._12 = tempVector3.y;
		P._13 = tempVector3.z;
		P._14 = tempVector3.w;

		///////////////////////////////////////
		tempVector1.x = M._21;
		tempVector1.y = M._22;
		tempVector1.z = M._23;
		tempVector1.w = M._24;

		tempVector2.x = M._41;
		tempVector2.y = M._42;
		tempVector2.z = M._43;
		tempVector2.w = M._44;

		tempVector3 = VectorMergeXY(tempVector1, tempVector2); // m00m20m01m21
		P._21 = tempVector3.x;
		P._22 = tempVector3.y;
		P._23 = tempVector3.z;
		P._24 = tempVector3.w;

		///////////////////////////////////////
		tempVector1.x = M._11;
		tempVector1.y = M._12;
		tempVector1.z = M._13;
		tempVector1.w = M._14;

		tempVector2.x = M._31;
		tempVector2.y = M._32;
		tempVector2.z = M._33;
		tempVector2.w = M._34;

		tempVector3 = VectorMergeZW(tempVector1, tempVector2); // m00m20m01m21
		P._31 = tempVector3.x;
		P._32 = tempVector3.y;
		P._33 = tempVector3.z;
		P._34 = tempVector3.w;

		///////////////////////////////////////
		tempVector1.x = M._21;
		tempVector1.y = M._22;
		tempVector1.z = M._23;
		tempVector1.w = M._24;

		tempVector2.x = M._41;
		tempVector2.y = M._42;
		tempVector2.z = M._43;
		tempVector2.w = M._44;

		tempVector3 = VectorMergeZW(tempVector1, tempVector2); // m00m20m01m21
		P._41 = tempVector3.x;
		P._42 = tempVector3.y;
		P._43 = tempVector3.z;
		P._44 = tempVector3.w;
	}

	{
		///////////////////////////////////////
		tempVector1.x = P._11;
		tempVector1.y = P._12;
		tempVector1.z = P._13;
		tempVector1.w = P._14;

		tempVector2.x = P._31;
		tempVector2.y = P._32;
		tempVector2.z = P._33;
		tempVector2.w = P._34;

		tempVector3 = VectorMergeXY(tempVector1, tempVector2); // m00m20m01m21
		MT._11 = tempVector3.x;
		MT._12 = tempVector3.y;
		MT._13 = tempVector3.z;
		MT._14 = tempVector3.w;

		///////////////////////////////////////
		tempVector1.x = P._21;
		tempVector1.y = P._22;
		tempVector1.z = P._23;
		tempVector1.w = P._24;

		tempVector2.x = P._41;
		tempVector2.y = P._42;
		tempVector2.z = P._43;
		tempVector2.w = P._44;

		tempVector3 = VectorMergeXY(tempVector1, tempVector2); // m00m20m01m21
		MT._21 = tempVector3.x;
		MT._22 = tempVector3.y;
		MT._23 = tempVector3.z;
		MT._24 = tempVector3.w;

		///////////////////////////////////////
		tempVector1.x = P._11;
		tempVector1.y = P._12;
		tempVector1.z = P._13;
		tempVector1.w = P._14;

		tempVector2.x = P._31;
		tempVector2.y = P._32;
		tempVector2.z = P._33;
		tempVector2.w = P._34;

		tempVector3 = VectorMergeZW(tempVector1, tempVector2); // m00m20m01m21
		MT._31 = tempVector3.x;
		MT._32 = tempVector3.y;
		MT._33 = tempVector3.z;
		MT._34 = tempVector3.w;

		///////////////////////////////////////
		tempVector1.x = P._21;
		tempVector1.y = P._22;
		tempVector1.z = P._23;
		tempVector1.w = P._24;

		tempVector2.x = P._41;
		tempVector2.y = P._42;
		tempVector2.z = P._43;
		tempVector2.w = P._44;

		tempVector3 = VectorMergeZW(tempVector1, tempVector2); // m00m20m01m21
		MT._41 = tempVector3.x;
		MT._42 = tempVector3.y;
		MT._43 = tempVector3.z;
		MT._44 = tempVector3.w;
	}

	return MT;
}

CXVECTORF4
XMVectorSwizzle
(
	CXVECTORF4 V,
	DWORD E0,
	DWORD E1,
	DWORD E2,
	DWORD E3
)
{
	CXVECTORF4 Result = {
		V.vector4_f32[E0],
		V.vector4_f32[E1],
		V.vector4_f32[E2],
		V.vector4_f32[E3]
	};
	return Result.v;

}

CXMATRIX4X4 MatrixInverse(
	CXVECTORF4* pDeterminant,
	CXMATRIX4X4 M
)
{
	CXMATRIX4X4 MT = MatrixTranspose(M);

	CXVECTORF4 V0[4], V1[4];
	V0[0] = XMVectorSwizzle<XM_SWIZZLE_X, XM_SWIZZLE_X, XM_SWIZZLE_Y, XM_SWIZZLE_Y>(MT.r[2]);
	V1[0] = XMVectorSwizzle<XM_SWIZZLE_Z, XM_SWIZZLE_W, XM_SWIZZLE_Z, XM_SWIZZLE_W>(MT.r[3]);
	V0[1] = XMVectorSwizzle<XM_SWIZZLE_X, XM_SWIZZLE_X, XM_SWIZZLE_Y, XM_SWIZZLE_Y>(MT.r[0]);
	V1[1] = XMVectorSwizzle<XM_SWIZZLE_Z, XM_SWIZZLE_W, XM_SWIZZLE_Z, XM_SWIZZLE_W>(MT.r[1]);
	V0[2] = XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_0Z, XM_PERMUTE_1X, XM_PERMUTE_1Z>(MT.r[2], MT.r[0]);
	V1[2] = XMVectorPermute<XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_1Y, XM_PERMUTE_1W>(MT.r[3], MT.r[1]);

	CXVECTORF4 D0 = XMVectorMultiply(V0[0], V1[0]);
	CXVECTORF4 D1 = XMVectorMultiply(V0[1], V1[1]);
	CXVECTORF4 D2 = XMVectorMultiply(V0[2], V1[2]);

	V0[0] = XMVectorSwizzle<XM_SWIZZLE_Z, XM_SWIZZLE_W, XM_SWIZZLE_Z, XM_SWIZZLE_W>(MT.r[2]);
	V1[0] = XMVectorSwizzle<XM_SWIZZLE_X, XM_SWIZZLE_X, XM_SWIZZLE_Y, XM_SWIZZLE_Y>(MT.r[3]);
	V0[1] = XMVectorSwizzle<XM_SWIZZLE_Z, XM_SWIZZLE_W, XM_SWIZZLE_Z, XM_SWIZZLE_W>(MT.r[0]);
	V1[1] = XMVectorSwizzle<XM_SWIZZLE_X, XM_SWIZZLE_X, XM_SWIZZLE_Y, XM_SWIZZLE_Y>(MT.r[1]);
	V0[2] = XMVectorPermute<XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_1Y, XM_PERMUTE_1W>(MT.r[2], MT.r[0]);
	V1[2] = XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_0Z, XM_PERMUTE_1X, XM_PERMUTE_1Z>(MT.r[3], MT.r[1]);

	D0 = XMVectorNegativeMultiplySubtract(V0[0], V1[0], D0);
	D1 = XMVectorNegativeMultiplySubtract(V0[1], V1[1], D1);
	D2 = XMVectorNegativeMultiplySubtract(V0[2], V1[2], D2);

	V0[0] = XMVectorSwizzle<XM_SWIZZLE_Y, XM_SWIZZLE_Z, XM_SWIZZLE_X, XM_SWIZZLE_Y>(MT.r[1]);
	V1[0] = XMVectorPermute<XM_PERMUTE_1Y, XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_0X>(D0, D2);
	V0[1] = XMVectorSwizzle<XM_SWIZZLE_Z, XM_SWIZZLE_X, XM_SWIZZLE_Y, XM_SWIZZLE_X>(MT.r[0]);
	V1[1] = XMVectorPermute<XM_PERMUTE_0W, XM_PERMUTE_1Y, XM_PERMUTE_0Y, XM_PERMUTE_0Z>(D0, D2);
	V0[2] = XMVectorSwizzle<XM_SWIZZLE_Y, XM_SWIZZLE_Z, XM_SWIZZLE_X, XM_SWIZZLE_Y>(MT.r[3]);
	V1[2] = XMVectorPermute<XM_PERMUTE_1W, XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_0X>(D1, D2);
	V0[3] = XMVectorSwizzle<XM_SWIZZLE_Z, XM_SWIZZLE_X, XM_SWIZZLE_Y, XM_SWIZZLE_X>(MT.r[2]);
	V1[3] = XMVectorPermute<XM_PERMUTE_0W, XM_PERMUTE_1W, XM_PERMUTE_0Y, XM_PERMUTE_0Z>(D1, D2);

	CXVECTORF4 C0 = XMVectorMultiply(V0[0], V1[0]);
	CXVECTORF4 C2 = XMVectorMultiply(V0[1], V1[1]);
	CXVECTORF4 C4 = XMVectorMultiply(V0[2], V1[2]);
	CXVECTORF4 C6 = XMVectorMultiply(V0[3], V1[3]);

	V0[0] = XMVectorSwizzle<XM_SWIZZLE_Z, XM_SWIZZLE_W, XM_SWIZZLE_Y, XM_SWIZZLE_Z>(MT.r[1]);
	V1[0] = XMVectorPermute<XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1X>(D0, D2);
	V0[1] = XMVectorSwizzle<XM_SWIZZLE_W, XM_SWIZZLE_Z, XM_SWIZZLE_W, XM_SWIZZLE_Y>(MT.r[0]);
	V1[1] = XMVectorPermute<XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_0X>(D0, D2);
	V0[2] = XMVectorSwizzle<XM_SWIZZLE_Z, XM_SWIZZLE_W, XM_SWIZZLE_Y, XM_SWIZZLE_Z>(MT.r[3]);
	V1[2] = XMVectorPermute<XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1Z>(D1, D2);
	V0[3] = XMVectorSwizzle<XM_SWIZZLE_W, XM_SWIZZLE_Z, XM_SWIZZLE_W, XM_SWIZZLE_Y>(MT.r[2]);
	V1[3] = XMVectorPermute<XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_1Z, XM_PERMUTE_0X>(D1, D2);

	C0 = XMVectorNegativeMultiplySubtract(V0[0], V1[0], C0);
	C2 = XMVectorNegativeMultiplySubtract(V0[1], V1[1], C2);
	C4 = XMVectorNegativeMultiplySubtract(V0[2], V1[2], C4);
	C6 = XMVectorNegativeMultiplySubtract(V0[3], V1[3], C6);

	V0[0] = XMVectorSwizzle<XM_SWIZZLE_W, XM_SWIZZLE_X, XM_SWIZZLE_W, XM_SWIZZLE_X>(MT.r[1]);
	V1[0] = XMVectorPermute<XM_PERMUTE_0Z, XM_PERMUTE_1Y, XM_PERMUTE_1X, XM_PERMUTE_0Z>(D0, D2);
	V0[1] = XMVectorSwizzle<XM_SWIZZLE_Y, XM_SWIZZLE_W, XM_SWIZZLE_X, XM_SWIZZLE_Z>(MT.r[0]);
	V1[1] = XMVectorPermute<XM_PERMUTE_1Y, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_1X>(D0, D2);
	V0[2] = XMVectorSwizzle<XM_SWIZZLE_W, XM_SWIZZLE_X, XM_SWIZZLE_W, XM_SWIZZLE_X>(MT.r[3]);
	V1[2] = XMVectorPermute<XM_PERMUTE_0Z, XM_PERMUTE_1W, XM_PERMUTE_1Z, XM_PERMUTE_0Z>(D1, D2);
	V0[3] = XMVectorSwizzle<XM_SWIZZLE_Y, XM_SWIZZLE_W, XM_SWIZZLE_X, XM_SWIZZLE_Z>(MT.r[2]);
	V1[3] = XMVectorPermute<XM_PERMUTE_1W, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_1Z>(D1, D2);

	CXVECTORF4 C1 = XMVectorNegativeMultiplySubtract(V0[0], V1[0], C0);
	C0 = XMVectorMultiplyAdd(V0[0], V1[0], C0);
	CXVECTORF4 C3 = XMVectorMultiplyAdd(V0[1], V1[1], C2);
	C2 = XMVectorNegativeMultiplySubtract(V0[1], V1[1], C2);
	CXVECTORF4 C5 = XMVectorNegativeMultiplySubtract(V0[2], V1[2], C4);
	C4 = XMVectorMultiplyAdd(V0[2], V1[2], C4);
	CXVECTORF4 C7 = XMVectorMultiplyAdd(V0[3], V1[3], C6);
	C6 = XMVectorNegativeMultiplySubtract(V0[3], V1[3], C6);

	CXMATRIX4X4 R;
	R.r[0] = XMVectorSelect(C0, C1, g_XMSelect0101.v);
	R.r[1] = XMVectorSelect(C2, C3, g_XMSelect0101.v);
	R.r[2] = XMVectorSelect(C4, C5, g_XMSelect0101.v);
	R.r[3] = XMVectorSelect(C6, C7, g_XMSelect0101.v);

	CXVECTORF4 Determinant = XMVector4Dot(R.r[0], MT.r[0]);

	if (pDeterminant != nullptr)
		*pDeterminant = Determinant;

	CXVECTORF4 Reciprocal = XMVectorReciprocal(Determinant);

	CXMATRIX4X4 Result;
	Result.r[0] = XMVectorMultiply(R.r[0], Reciprocal);
	Result.r[1] = XMVectorMultiply(R.r[1], Reciprocal);
	Result.r[2] = XMVectorMultiply(R.r[2], Reciprocal);
	Result.r[3] = XMVectorMultiply(R.r[3], Reciprocal);
	return Result;
}

void         
ComputeVP() 
{
	CXMATRIX4X4 Proj;
	float Aspect = Proj_W / max(Proj_H, 1);

	if (IsOrthographic) {
		float mn = max(Proj_W, Proj_H);
		float xx = Proj_W * Proj_Scale / mn; // ?????
		float yy = Proj_H * Proj_Scale / mn;

		Proj = MatrixOrthographicLH(xx, yy, Proj_Near, Proj_Far);
	}
	else {
		Proj = MatrixPerspectiveFovLH(XMConvertToRadians(Proj_Fov), Aspect, Proj_Near, Proj_Far);
	}

	CXMATRIX4X4    iProj = XMMatrixInverse(0, Proj);

	// view
	CXMATRIX4X4    camRotMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(View_Angle.x), XMConvertToRadians(View_Angle.y), XMConvertToRadians(View_Angle.z));
	CXVECTORF4    camPosition = XMLoadFloat3(&View_Pos);

	CXVECTORF4    camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	camUp = XMVector3TransformCoord(camUp, camRotMatrix);
	CXVECTORF4    camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	camRight = XMVector3TransformCoord(camRight, camRotMatrix);
	CXVECTORF4    camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	camForward = XMVector3TransformCoord(camForward, camRotMatrix); // is Target

	CXMATRIX4X4    View;
	CXVECTORF4    camTarget;

	if (IsThirdPerson) {
		camTarget = XMVector3Normalize(camForward);
		camTarget = camPosition - camTarget * View_Distance;

		View = XMMatrixLookAtLH(camTarget, camPosition, camUp);
	}
	else {
		camTarget = XMVector3Normalize(camForward);
		camTarget = camTarget + camPosition;

		View = XMMatrixLookAtLH(camPosition, camTarget, camUp);
	}

	CXMATRIX4X4    iView = XMMatrixInverse(0, View);

	CXMATRIX4X4   ViewProj = View * Proj;
	CXMATRIX4X4    iViewProj = XMMatrixInverse(0, ViewProj);

	// save
	StoreFloat4x4(&Matrices.m_P, Proj);
	StoreFloat4x4(&Matrices.m_iP, iProj);
	StoreFloat4x4(&Matrices.m_V, View);
	StoreFloat4x4(&Matrices.m_iV, iView);
	StoreFloat4x4(&Matrices.m_VP, ViewProj);
	StoreFloat4x4(&Matrices.m_iVP, iViewProj);

	StoreFloat3(&View_RightVector, camRight);
	StoreFloat3(&View_UpVector, camUp);
	StoreFloat3(&View_ForwardVector, camForward);

	// save view info
	StoreFloat4(&Matrices.CameraDir, camForward);
	Matrices.CameraDir.w = 1.0f;

	if (IsThirdPerson) {
		StoreFloat4(&Matrices.CameraPos, camTarget);

	}
	else {
		StoreFloat4(&Matrices.CameraPos, camPosition);
	}
	Matrices.CameraPos.w = 1.0f;

	Matrices.Resolution = XMFLOAT4(Proj_W, Proj_H, 1.0f / max(Proj_W, 1.0f), 1.0f / max(Proj_H, 1.0f));

	// save trasp
	StoreFloat4x4(&MatricesTranspose.m_P, MatrixTranspose(Proj));
	StoreFloat4x4(&MatricesTranspose.m_iP, MatrixTranspose(iProj));
	StoreFloat4x4(&MatricesTranspose.m_V, MatrixTranspose(View));
	StoreFloat4x4(&MatricesTranspose.m_iV, MatrixTranspose(iView));
	StoreFloat4x4(&MatricesTranspose.m_VP, MatrixTranspose(ViewProj));
	StoreFloat4x4(&MatricesTranspose.m_iVP, MatrixTranspose(iViewProj));
	MatricesTranspose.CameraPos = Matrices.CameraPos;
	MatricesTranspose.CameraDir = Matrices.CameraDir;
	MatricesTranspose.Resolution = Matrices.Resolution;
}

void           
CreateWorldMatrix(CXMATRIX4X4* WorldOut_, CXMATRIX3 Pos_, CXMATRIX3 Angle_, CXMATRIX3 Scale_)
{

}


void         
ComputeWVP(CXMATRIX4X4* World_, boolean IgnorePos)
{

}

void          
EditorViewMovePos(CXMATRIX3* TempPos, float AddLeftRight, float AddBackForward, float AddUpDawn)
{

}