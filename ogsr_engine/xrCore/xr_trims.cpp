#include "stdafx.h"

#include "xr_trims.h"

LPSTR _TrimLeft(LPSTR str)
{
    LPSTR p = str;
    while (*p && gsl::narrow_cast<u8>(*p) <= gsl::narrow_cast<u8>(' '))
        p++;

    if (p != str)
    {
        LPSTR t = str;
        for (; *p; t++, p++)
            *t = *p;

        *t = '\0';
    }

    return str;
}

LPSTR _TrimRight(LPSTR str)
{
    LPSTR p = str + xr_strlen(str);
    while (p != str && gsl::narrow_cast<u8>(*p) <= gsl::narrow_cast<u8>(' '))
        p--;

    *(++p) = '\0';

    return str;
}

LPSTR _Trim(LPSTR str)
{
    std::ignore = _TrimLeft(str);
    std::ignore = _TrimRight(str);

    return str;
}

namespace
{
LPCSTR _SetPos(LPCSTR src, gsl::index pos, char separator)
{
    LPCSTR res = src;
    gsl::index p{};

    while ((p < pos) && (nullptr != (res = strchr(res, separator))))
    {
        res++;
        p++;
    }

    return res;
}
} // namespace

gsl::index _GetItemCount(LPCSTR src, char separator)
{
    gsl::index cnt{};

    if (src && src[0])
    {
        LPCSTR res = src;
        LPCSTR last_res = res;

        while (nullptr != (res = strchr(res, separator)))
        {
            res++;
            last_res = res;
            cnt++;
            // if (res[0] == separator)
            //     break;
        }

        if (xr_strlen(last_res) > 0)
            cnt++;
    }

    return cnt;
}

namespace
{
LPCSTR _CopyVal(LPCSTR src, LPSTR dst, char separator)
{
    const char* p = strchr(src, separator);
    gsl::index n{p ? (p - src) : xr_strlen(src)};

    strncpy(dst, src, gsl::narrow_cast<size_t>(n));
    dst[n] = '\0';

    return dst;
}
} // namespace

LPSTR _GetItem(LPCSTR src, gsl::index index, LPSTR dst, char separator, LPCSTR def, bool trim)
{
    LPCSTR ptr = _SetPos(src, index, separator);
    if (ptr)
        _CopyVal(ptr, dst, separator);
    else
        strcpy(dst, def);

    if (trim)
        std::ignore = _Trim(dst);

    return dst;
}

LPSTR _GetItems(LPCSTR src, gsl::index idx_start, gsl::index idx_end, LPSTR dst, char separator)
{
    LPSTR n = dst;
    gsl::index level{};

    for (LPCSTR p = src; *p != 0; p++)
    {
        if ((level >= idx_start) && (level < idx_end))
            *n++ = *p;
        if (*p == separator)
            level++;
        if (level >= idx_end)
            break;
    }

    *n = '\0';

    return dst;
}

gsl::index _ParseItem(LPCSTR src, xr_token* token_list)
{
    for (gsl::index i{}; token_list[i].name; i++)
    {
        if (std::is_eq(xr::strcasecmp(src, token_list[i].name)))
            return token_list[i].id;
    }

    return -1;
}

void _SequenceToList(SStringVec& lst, LPCSTR in, char separator)
{
    lst.clear();

    gsl::index t_cnt = _GetItemCount(in, separator);
    xr_string T;

    for (gsl::index i{}; i < t_cnt; i++)
    {
        std::ignore = _GetItem(in, i, T, separator, nullptr);
        std::ignore = _Trim(T);

        if (!T.empty())
            lst.push_back(T.c_str());
    }
}

xr_string& _TrimLeft(xr_string& str)
{
    LPCSTR b = str.c_str();
    LPCSTR p = str.c_str();

    while (*p && gsl::narrow_cast<u8>(*p) <= gsl::narrow_cast<u8>(' '))
        p++;

    if (p != b)
        str.erase(0, gsl::narrow_cast<size_t>(p - b));

    return str;
}

xr_string& _TrimRight(xr_string& str)
{
    LPCSTR b = str.c_str();
    auto l = std::ssize(str);
    if (l)
    {
        LPCSTR p = str.c_str() + l - 1;
        while (p != b && gsl::narrow_cast<u8>(*p) <= gsl::narrow_cast<u8>(' '))
            p--;

        if (p != str + b)
            str.erase(gsl::narrow_cast<size_t>(p - b + 1), gsl::narrow_cast<size_t>(l - (p - b)));
    }

    return str;
}

xr_string& _Trim(xr_string& str)
{
    std::ignore = _TrimLeft(str);
    std::ignore = _TrimRight(str);

    return str;
}

namespace
{
LPCSTR _CopyVal(LPCSTR src, xr_string& dst, char separator)
{
    const char* p = strchr(src, separator);
    const gsl::index n{p ? (p - src) : xr_strlen(src)};
    dst = src;
    dst = dst.erase(gsl::narrow_cast<size_t>(n), dst.length());
    return dst.c_str();
}
} // namespace

LPCSTR _GetItem(LPCSTR src, gsl::index index, xr_string& dst, char separator, LPCSTR def, bool trim)
{
    LPCSTR ptr = _SetPos(src, index, separator);
    if (ptr)
        _CopyVal(ptr, dst, separator);
    else
        dst = def;

    if (trim)
        std::ignore = _Trim(dst);

    return dst.c_str();
}
