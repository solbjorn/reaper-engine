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
#include <cstring>

#ifndef _INC_STRING
#error Wrong include order to override UCRT functions
#endif

// llvm-libc
extern "C" {
extern int strcasecmp(const char* left, const char* right);
}

#define _stricmp(l, r) strcasecmp(l, r)

#include <windows.h>
#include <windowsx.h>
#pragma warning(pop)

// KRodin: фикс, чтобы intellisense нормально работал с многочисленными методами типа CUIWindow::SendMessage и тп.
#ifdef __INTELLISENSE__
#ifdef SendMessage
#undef SendMessage
#endif
#endif
