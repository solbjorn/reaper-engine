#include "stdafx.h"

#include "xr_time.h"
#include "ui/UIInventoryUtilities.h"
#include "level.h"
#include "date_time.h"
#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_time_manager.h"

namespace
{
constexpr float sec2ms{1000.0f};

ALife::_TIME_ID __game_time() { return (ai().get_alife() ? ai().alife().time().game_time() : Level().GetGameTime()); }
} // namespace

u32 get_time() { return gsl::narrow_cast<u32>(__game_time()); }
xrTime get_time_struct() { return xrTime{__game_time()}; }

LPCSTR xrTime::dateToString(int mode) { return *InventoryUtilities::GetDateAsString(m_time, (InventoryUtilities::EDatePrecision)mode); }
LPCSTR xrTime::timeToString(int mode) { return *InventoryUtilities::GetTimeAsString(m_time, (InventoryUtilities::ETimePrecision)mode); }

xrTime& xrTime::add(const xrTime& other)
{
    m_time += other.m_time;
    return *this;
}

xrTime& xrTime::sub(const xrTime& other)
{
    if (*this > other)
        m_time -= other.m_time;
    else
        m_time = 0;

    return *this;
}

xrTime& xrTime::setHMS(int h, int m, int s)
{
    m_time = 0;
    m_time += generate_time(1, 1, 1, h, m, s);

    return *this;
}

xrTime& xrTime::setHMSms(int h, int m, int s, int ms)
{
    m_time = 0;
    m_time += generate_time(1, 1, 1, h, m, s, ms);

    return *this;
}

xrTime& xrTime::set(int y, int mo, int d, int h, int mi, int s, int ms)
{
    m_time = 0;
    m_time += generate_time(y, mo, d, h, mi, s, ms);

    return *this;
}

std::tuple<u32, u32, u32, u32, u32, u32, u32> xrTime::get() const
{
    u32 y, mo, d, h, mi, s, ms;

    split_time(m_time, y, mo, d, h, mi, s, ms);

    return std::make_tuple(y, mo, d, h, mi, s, ms);
}

float xrTime::diffSec(const xrTime& other) const
{
    if (*this > other)
        return (m_time - other.m_time) / sec2ms;

    return ((other.m_time - m_time) / sec2ms) * (-1.0f);
}
