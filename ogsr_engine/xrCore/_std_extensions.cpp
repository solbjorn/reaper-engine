#include "stdafx.h"

#include <codecvt>
#include <locale>

char* timestamp(string64& dest)
{
    string64 temp;

    /* Set time zone from TZ environment variable. If TZ is not set,
     * the operating system is queried to obtain the default value
     * for the variable.
     */
    _tzset();
    u32 it;

    // date
    _strdate(temp);
    for (it = 0; it < xr_strlen(temp); it++)
        if ('/' == temp[it])
            temp[it] = '-';
    strconcat(sizeof(dest), dest, temp, "_");

    // time
    _strtime(temp);
    for (it = 0; it < xr_strlen(temp); it++)
        if (':' == temp[it])
            temp[it] = '-';
    strcat(dest, temp);
    return dest;
}

char* xr_strdup(const char* string)
{
    VERIFY(string);

    const auto len = gsl::narrow_cast<size_t>(xr_strlen(string) + 1);
    char* memory = static_cast<char*>(xr_malloc(len));
    std::memcpy(memory, string, len);

    return memory;
}

// Очень полезная штука из OpenXRay
std::string StringToUTF8(const char* in)
{
    const auto len = gsl::narrow_cast<size_t>(xr_strlen(in));
    static const std::locale locale{""};
    using wcvt = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>;
    std::wstring wstr(len, L'\0');
    std::use_facet<std::ctype<wchar_t>>(locale).widen(in, in + len, wstr.data());
    return wcvt{}.to_bytes(wstr.data(), wstr.data() + wstr.size());
}

std::string StringFromUTF8(const char* in)
{
    using wcvt = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>;
    const std::wstring wstr = wcvt{}.from_bytes(in);
    static const std::locale locale{""};
    std::string result(wstr.size(), '\0');
    std::use_facet<std::ctype<wchar_t>>(locale).narrow(wstr.data(), wstr.data() + wstr.size(), '?', result.data());
    return result;
}
