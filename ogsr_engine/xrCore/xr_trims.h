#pragma once

// refs
struct xr_token;

int _GetItemCount(LPCSTR, char separator = ',');

LPSTR _GetItem(LPCSTR, int, LPSTR, char separator = ',', LPCSTR = "", bool trim = true);
LPCSTR _GetItem(LPCSTR, int, xr_string& p, char separator = ',', LPCSTR = "", bool trim = true);

LPSTR _GetItems(LPCSTR, int, int, LPSTR, char separator = ',');

u32 _ParseItem(LPCSTR src, xr_token* token_list);
void _SequenceToList(SStringVec& lst, LPCSTR in, char separator = ',');

LPSTR _Trim(LPSTR str);
LPSTR _TrimLeft(LPSTR str);
LPSTR _TrimRight(LPSTR str);

xr_string& _Trim(xr_string& src);
xr_string& _TrimLeft(xr_string& src);
xr_string& _TrimRight(xr_string& src);
