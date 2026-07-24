#pragma once

#include "xrMessages.h"

class NET_Event
{
public:
    u16 ID;
    u32 timestamp;
    u16 type;
    u16 destination;
    xr_vector<u8> data;

public:
    void import(NET_Packet& P)
    {
        data.clear();
        std::ignore = P.r_begin(ID);

        switch (xr::msg{ID})
        {
        case xr::msg::M_SPAWN: P.read_start(); break;
        case xr::msg::M_EVENT:
            P.r_u32(timestamp);
            P.r_u16(type);
            P.r_u16(destination);
            break;
        default: xr::unreachable();
        }

        if (const auto size = P.r_elapsed(); size > 0)
        {
            data.resize(size);
            P.r(data.data(), size);
        }
    }

    void n_export(NET_Packet& P)
    {
        P.w_begin(gsl::narrow<u16>(xr::msg::M_EVENT));
        P.w_u32(timestamp);
        P.w_u16(type);
        P.w_u16(destination);

        if (!data.empty())
            P.w(data.data(), data.size());
    }

    void implication(NET_Packet& P) const
    {
        std::ranges::copy(data, P.B.data);
        P.B.count = data.size();
        P.r_pos = 0;
    }
};

IC bool operator<(const NET_Event& A, const NET_Event& B) { return A.timestamp < B.timestamp; }

class NET_Queue_Event
{
public:
    xr_deque<NET_Event> queue;

    IC void insert(NET_Packet& P)
    {
        NET_Event E;
        E.import(P);
        queue.push_back(E);
    }

    bool available() const { return !queue.empty(); }

    IC void get(u16& ID, u16& dest, u16& type, NET_Packet& P)
    {
        const NET_Event& E = *queue.begin();
        ID = E.ID;
        dest = E.destination;
        type = E.type;
        E.implication(P);
        queue.pop_front();
    }
};
