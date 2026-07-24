#pragma once

#define DEBUG_INFO std::source_location::current().file_name(), std::source_location::current().line(), std::source_location::current().function_name()

#define R_ASSERT(expr, ...) \
    do \
    { \
        if (!(expr)) [[unlikely]] \
            ::Debug.fail(#expr, ##__VA_ARGS__, DEBUG_INFO); \
    } while (0)

#define R_ASSERT2 R_ASSERT
#define R_ASSERT3 R_ASSERT

#ifdef VERIFY
#undef VERIFY
#endif

#ifdef DEBUG
#define VERIFY R_ASSERT
#else
#define VERIFY(expr, ...) R_ASSERT(!std::is_constant_evaluated() || (expr), ##__VA_ARGS__)
#endif

#define VERIFY2 VERIFY
#define VERIFY3 VERIFY
