#pragma once

// refs
struct xr_token;

[[nodiscard]] gsl::index _GetItemCount(LPCSTR, char separator = ',');

[[nodiscard]] LPSTR _GetItem(LPCSTR, gsl::index, LPSTR, char separator = ',', LPCSTR = "", bool trim = true);
[[nodiscard]] LPCSTR _GetItem(LPCSTR, gsl::index, xr_string& p, char separator = ',', LPCSTR = "", bool trim = true);

[[nodiscard]] LPSTR _GetItems(LPCSTR, gsl::index, gsl::index, LPSTR, char separator = ',');

[[nodiscard]] gsl::index _ParseItem(LPCSTR src, xr_token* token_list);
void _SequenceToList(SStringVec& lst, LPCSTR in, char separator = ',');

[[nodiscard]] LPSTR _Trim(LPSTR str);
[[nodiscard]] LPSTR _TrimLeft(LPSTR str);
[[nodiscard]] LPSTR _TrimRight(LPSTR str);

[[nodiscard]] xr_string& _Trim(xr_string& src);
[[nodiscard]] xr_string& _TrimLeft(xr_string& src);
[[nodiscard]] xr_string& _TrimRight(xr_string& src);
