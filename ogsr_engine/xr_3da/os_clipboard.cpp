////////////////////////////////////////////////////////////////////////////
// Module : os_clipboard.cpp
// Created : 21.02.2008
// Author : Evgeniy Sokolov
// Description : os clipboard class implementation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "os_clipboard.h"

void os_clipboard::copy_to_clipboard(LPCSTR buf)
{
    if (!OpenClipboard(nullptr))
        return;

    auto handle_size = xr_strlen(buf) + 1;
    HGLOBAL handle = GlobalAlloc(GHND, handle_size);
    if (!handle)
    {
        CloseClipboard();
        return;
    }

    char* memory = (char*)GlobalLock(handle);
    xr_strcpy(memory, handle_size, buf);
    GlobalUnlock(handle);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, handle);
    CloseClipboard();
}

void os_clipboard::paste_from_clipboard(LPSTR buffer, u32 const& buffer_size)
{
    VERIFY(buffer);
    VERIFY(buffer_size > 0);

    if (!OpenClipboard(nullptr))
        return;

    HGLOBAL hmem = GetClipboardData(CF_TEXT);
    if (!hmem)
    {
        CloseClipboard();
        return;
    }

    LPCSTR clipdata = (LPCSTR)GlobalLock(hmem);
    strncpy_s(buffer, buffer_size, clipdata, buffer_size - 1);
    buffer[buffer_size - 1] = 0;

    for (auto& c : std::span{buffer, gsl::narrow_cast<size_t>(xr_strlen(buffer))})
    {
        if (((isprint(c) == 0) && (c != char(-1))) || c == '\t' || c == '\n')
            c = ' ';
    }

    GlobalUnlock(hmem);
    CloseClipboard();
}
