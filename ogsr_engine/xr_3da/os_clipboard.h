////////////////////////////////////////////////////////////////////////////
// Module : os_clipboard.h
// Created : 21.02.2008
// Modified : 04.03.2008
// Author : Evgeniy Sokolov
// Description : os clipboard class
////////////////////////////////////////////////////////////////////////////

#ifndef OS_CLIPBOARD_H_INCLUDED
#define OS_CLIPBOARD_H_INCLUDED

namespace os_clipboard
{
void copy_to_clipboard(LPCSTR buf);
void paste_from_clipboard(LPSTR buf, u32 const& buf_size);
} // namespace os_clipboard

#endif // OS_CLIPBOARD_H_INCLUDED
