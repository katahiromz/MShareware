// mdbg.h --- Debugging output for Win32 -*- C++ -*-
#pragma once

inline void mdbg_vprintfA(const char *fmt, va_list va)
{
#ifndef NDEBUG
    char buf[1024];
#ifdef _STRSAFE_H_INCLUDED_
    StringCchVPrintfA(buf, _countof(buf), fmt, va);
#else
    wvnsprintfA(buf, _countof(buf), fmt, va);
#endif
    OutputDebugStringA(buf);
    fputs(buf, stdout);
#endif
}

inline void mdbg_vprintfW(const wchar_t *fmt, va_list va)
{
#ifndef NDEBUG
    wchar_t buf[1024];
#ifdef _STRSAFE_H_INCLUDED_
    StringCchVPrintfW(buf, _countof(buf), fmt, va);
#else
    wvnsprintfW(buf, _countof(buf), fmt, va);
#endif
    OutputDebugStringW(buf);
    fputws(buf, stdout);
#endif
}

// デバッグ出力（ANSI版）
inline void mdbg_printfA(const char *fmt, ...)
{
#ifndef NDEBUG
    va_list va;
    va_start(va, fmt);
    mdbg_vprintfA(fmt, va);
    va_end(va);
#endif
}

// デバッグ出力（Unicode版）
inline void mdbg_printfW(const wchar_t *fmt, ...)
{
#ifndef NDEBUG
    va_list va;
    va_start(va, fmt);
    mdbg_vprintfW(fmt, va);
    va_end(va);
#endif
}

#ifdef UNICODE
    #define mdbg_tprintf mdbg_printfW
#else
    #define mdbg_tprintf mdbg_printfA
#endif

// デバッグ出力に足跡を残す
#define mdbg_footmark() \
    mdbg_printfA("%s (%d): %s\n", __FILE__, __LINE__, __func__)
