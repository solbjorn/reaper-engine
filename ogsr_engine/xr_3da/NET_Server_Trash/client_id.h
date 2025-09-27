#pragma once

class ClientID
{
    u32 id{};

public:
    ClientID() = default;
    ClientID(u32 val) : id{val} {}

    u32 value() const { return id; }
    void set(u32 v) { id = v; }
    void setBroadcast() { set(std::numeric_limits<u32>::max()); }
    bool compare(u32 v) const { return id == v; }
    bool operator==(const ClientID& other) const { return value() == other.value(); }
    bool operator!=(const ClientID& other) const { return value() != other.value(); }
    bool operator<(const ClientID& other) const { return value() < other.value(); }
};
