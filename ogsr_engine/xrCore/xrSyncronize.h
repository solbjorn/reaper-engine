#pragma once

#include <mutex>

// KRodin: Теперь это просто обёртка над std::recursive_mutex, т.к. мне лень кучу кода переделывать.

class xrCriticalSection final : std::recursive_mutex
{
public:
    inline void Enter() { __super::lock(); }
    inline void Leave() { __super::unlock(); }
    inline bool TryEnter() { return __super::try_lock(); }
};
