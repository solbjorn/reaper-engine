#ifndef _MB_HELPERS_H_INCLUDED
#define _MB_HELPERS_H_INCLUDED

#define MAX_MB_CHARS 4096

typedef unsigned short int wide_char;

unsigned short int mbhMulti2Wide(wide_char* WideStr, wide_char* WidePos, const unsigned short int WideStrSize, const char* MultiStr);

[[nodiscard]] constexpr bool IsSpaceCharacter(wide_char wc)
{
    return ((wc == 0x0020) ||

            (wc == 0xFF01) || (wc == 0xFF0C) ||
            //( wc == 0xFF0D )  ||
            (wc == 0xFF0E) || (wc == 0xFF1A) || (wc == 0xFF1B) || (wc == 0xFF1F) ||

            (wc == 0x2026) ||

            (wc == 0x3002) || (wc == 0x3001));
}

#endif // _MB_HELPERS_H_INCLUDED
