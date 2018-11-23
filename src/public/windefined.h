#pragma  once

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)

#endif
#endif

#if defined(_MSC_VER) || defined(__BORLANDC__)

#else 
#define INFINITE 			0xffffffff
/*typedef unsigned char       BYTE;
//typedef unsigned int		size_t;
typedef unsigned long		ULONG;
typedef unsigned char		byte;
typedef unsigned int		UINT;
typedef unsigned long		DWORD;

typedef char TCHAR, *PTCHAR;
typedef unsigned char TBYTE , *PTBYTE ;
typedef void*				PVOID
*/

#define INVALID_HANDLE_VALUE   -1
#define _MAX_PATH       260 /* max. length of full pathname */

#define HANDLE            int
#define MAX_PATH        260
#define TRUE                true
#define FALSE               false
#define __stdcall
#define __declspec(x)
#define __cdecl
template <typename T>
T max(T a, T b) {return (a > b) ? a : b;}

template <typename T>
T min(T a, T b) {return (a < b) ? a : b;}

typedef unsigned char		byte;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef char                CHAR,TCHAR;
typedef unsigned char       UCHAR;
typedef unsigned char       *PUCHAR;
typedef short               SHORT;
typedef unsigned short      USHORT;
typedef unsigned short      *PUSHORT;
typedef long                LONG;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;
typedef long long           LONGLONG;
typedef unsigned long long  ULONGLONG;
typedef ULONGLONG           *PULONGLONG;
typedef unsigned long       ULONG;
typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;
typedef void                VOID;
typedef void*				LPVOID;

typedef char               *LPSTR;
typedef const char         *LPCSTR;
typedef wchar_t            WCHAR;
typedef WCHAR              *LPWSTR;
typedef const WCHAR        *LPCWSTR;
typedef DWORD              *LPDWORD;
typedef unsigned long      UINT_PTR;
typedef UINT_PTR           SIZE_T;
typedef LONGLONG           USN;
typedef BYTE               BOOLEAN;
typedef void                *PVOID;

#endif
