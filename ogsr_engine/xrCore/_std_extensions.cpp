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

    const auto len = xr_strlen(string) + 1;
    char* memory = static_cast<char*>(xr_malloc(len));
    std::memcpy(memory, string, gsl::narrow_cast<size_t>(len));

    return memory;
}
