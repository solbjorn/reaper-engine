#pragma once

#include "alife_space.h"

class xrTime
{
    ALife::_TIME_ID m_time;

public:
    xrTime() : m_time(0) {}
    xrTime(const xrTime& other) : m_time(other.m_time) {}
    xrTime(ALife::_TIME_ID t) : m_time(t) {}

    bool operator<(const xrTime& other) const { return m_time < other.m_time; }
    bool operator>(const xrTime& other) const { return m_time > other.m_time; }
    bool operator>=(const xrTime& other) const { return m_time >= other.m_time; }
    bool operator<=(const xrTime& other) const { return m_time <= other.m_time; }
    bool operator==(const xrTime& other) const { return m_time == other.m_time; }
    xrTime operator+(const xrTime& other) { return xrTime(m_time + other.m_time); }
    xrTime operator-(const xrTime& other) { return xrTime(m_time - other.m_time); }

    float diffSec(const xrTime& other);
    xrTime& add(const xrTime& other);
    xrTime& sub(const xrTime& other);

    xrTime& add_script(xrTime* other) { return add(*other); };
    xrTime& sub_script(xrTime* other) { return sub(*other); };
    float diffSec_script(xrTime* other) { return diffSec(*other); };

    xrTime& setHMS(int h, int m, int s);
    xrTime& setHMSms(int h, int m, int s, int ms);
    xrTime& set(int y, int mo, int d, int h, int mi, int s, int ms);
    std::tuple<u32, u32, u32, u32, u32, u32, u32> get();

    LPCSTR dateToString(int mode);
    LPCSTR timeToString(int mode);
};

extern u32 get_time();
extern xrTime get_time_struct();
