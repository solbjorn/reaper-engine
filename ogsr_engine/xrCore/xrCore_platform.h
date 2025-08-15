#pragma once

#define VC_EXTRALEAN // Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#define DIRECTINPUT_VERSION 0x0800 //

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#define NOGDICAPMASKS
// #define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NODRAWTEXT
#define NOMEMMGR
// #define NOMETAFILE
#define NOSERVICE
#define NOCOMM
#define NOHELP
#define NOPROFILER
#define NOMCX
#define NOMINMAX
#define DOSWIN32
#define _WIN32_DCOM

#pragma warning(push)
#pragma warning(disable : 4005)
#include <string.h>

_CRT_BEGIN_C_HEADER

static __forceinline _Check_return_ int _stricmp(const char* left, const char* right)
{
    extern int strcasecmp(const char* left, const char* right);
    return strcasecmp(left, right);
}

_CRT_END_C_HEADER

#include <windows.h>
#include <windowsx.h>
#pragma warning(pop)

// KRodin: фикс, чтобы intellisense нормально работал с многочисленными методами типа CUIWindow::SendMessage и тп.
#ifdef __INTELLISENSE__
#ifdef SendMessage
#undef SendMessage
#endif
#endif
