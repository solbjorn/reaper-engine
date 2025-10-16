#pragma once

#define DEBUG_INFO __FILE__, __LINE__, __FUNCTION__

// KRodin: Добавил ASSERT как в скриптах, с поддержкой форматирования строки и неограниченным кол-вом аргументов.
#define FATAL(...) ::Debug.fatal(DEBUG_INFO, ##__VA_ARGS__)

#define ASSERT_FMT(expr, ...) \
    do \
    { \
        if (!(expr)) [[unlikely]] \
            FATAL(__VA_ARGS__); \
    } while (0)

#define CHECK_OR_EXIT(expr, message) \
    do \
    { \
        if (!(expr)) [[unlikely]] \
            ::Debug.do_exit(message); \
    } while (0)

#define R_ASSERT(expr, ...) \
    do \
    { \
        if (!(expr)) [[unlikely]] \
            ::Debug.fail(#expr, ##__VA_ARGS__, DEBUG_INFO); \
    } while (0)

#define R_ASSERT2 R_ASSERT
#define R_ASSERT3 R_ASSERT

#define R_CHK(expr, ...) \
    do \
    { \
        HRESULT hr = expr; \
        if (FAILED(hr)) [[unlikely]] \
            ::Debug.error(hr, #expr, ##__VA_ARGS__, DEBUG_INFO); \
    } while (0)

#define R_CHK2 R_CHK

#ifdef VERIFY
#undef VERIFY
#endif

#ifdef DEBUG
#define NODEFAULT FATAL("nodefault reached")
#define VERIFY R_ASSERT
#define CHK_DX R_CHK
#else
#define NODEFAULT std::unreachable()
#define VERIFY(expr, ...) R_ASSERT(!std::is_constant_evaluated() || (expr), ##__VA_ARGS__)
#define CHK_DX(a) a
#endif

#define VERIFY2 VERIFY
#define VERIFY3 VERIFY
#define VERIFY4 VERIFY
