/****************************************************************
* MZPE Team, 2019.
* Alistra intro
* License: MIT
*****************************************************************/
#pragma once
#include "DemoSettings.h"

typedef signed char         i8;
typedef unsigned char       u8;
typedef signed short        i16;
typedef unsigned short      u16;
typedef signed int          i32;
typedef unsigned int        u32;
typedef signed long long    i64;
typedef unsigned long long  u64;
typedef float               f32;
typedef double              f64;

typedef signed char         s8;
typedef signed short        s16;
typedef signed int          s32;
typedef signed long long    s64;

typedef u64                 uint64;
typedef i8                  int8;
typedef i16                 int16;
typedef i64                 int64;
typedef f32                 r32;
typedef f64                 r64;
typedef i32                 b32;
typedef f64                 real64;

typedef char                utf8;
typedef utf8*               PStr;
typedef utf8 const*         PConstStr;

typedef struct
{
	WORD MajorVersion;			// 6 for Windows Seven
	WORD MinorVersion;			// 1 for Windows Seven
	WORD BuildVersion;			// ex. 18362
	WORD MarketVersion;			// ex. 1903
} BASE_OS_VERSION_INFO;

#define _RELEASE(p)                       { if(p){(p)->Release(); (p)=NULL;} }

constexpr unsigned int BASE_WIDTH   { 1280 };
constexpr unsigned int BASE_HEIGHT  {  720 };

int RealEntryPoint(char** argv, int argc);
bool GetSystemVersion(BASE_OS_VERSION_INFO* pVersion);
bool LoadFile(const wchar_t* PathToFile, void** OutFile, size_t* OutSize);

#ifndef GUID_SECT
#define GUID_SECT
#endif

#define __DEFINE_GUID(n,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) static const GUID n GUID_SECT = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
#define __DEFINE_IID(n,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) static const IID n GUID_SECT = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
#define __DEFINE_CLSID(n,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) static const CLSID n GUID_SECT = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
#define DEFINE_CLSID(className, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    __DEFINE_CLSID(A_CLSID_##className, 0x##l, 0x##w1, 0x##w2, 0x##b1, 0x##b2, 0x##b3, 0x##b4, 0x##b5, 0x##b6, 0x##b7, 0x##b8)
#define DEFINE_IID(interfaceName, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    __DEFINE_IID(A_IID_##interfaceName, 0x##l, 0x##w1, 0x##w2, 0x##b1, 0x##b2, 0x##b3, 0x##b4, 0x##b5, 0x##b6, 0x##b7, 0x##b8)
