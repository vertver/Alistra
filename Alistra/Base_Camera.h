/****************************************************************
* MZPE Team, 2019.
* Alistra intro
* License: MIT
*****************************************************************/
#pragma once
#include "Base.h"

typedef struct
{
	float _11, _12, _13, _14;
	float _21, _22, _23, _24;
	float _31, _32, _33, _34;
	float _41, _42, _43, _44;
} CXMATRIX4X4;

typedef union
{
	float x;
	float y;
	float z;
	float w;
} CXVECTORF4;

typedef union
{
	DWORD x;
	DWORD y;
	DWORD z;
	DWORD w;
} CXMATRIX4U;

typedef union
{
	float x;
	float y;
	float z;
} CXMATRIX3;

/*
	Set 16-byte align for GPU memory texture
*/
typedef __declspec(align(16)) struct {
	CXMATRIX4X4 m_V; 
	CXMATRIX4X4 m_iV; 
	CXMATRIX4X4 m_P;
	CXMATRIX4X4 m_iP;
	CXMATRIX4X4 m_VP;
	CXMATRIX4X4 m_iVP;
	CXMATRIX4X4 m_W;
	CXMATRIX4X4 m_iW;
	CXMATRIX4X4 m_WV;
	CXMATRIX4X4 m_WVP;
	CXVECTORF4 CameraPos;
	CXVECTORF4 CameraDir;
	CXVECTORF4 Resolution;
} CAMERA_MATRIXS;

void InitMatrix();