#pragma once

struct Triangle final
{
    dVector3 side0;
    dVector3 side1;
    dVector3 norm;
    dReal dist;
    dReal pos;
    dReal depth;
    const CDB::TRI* T{nullptr};

    constexpr Triangle()
#ifdef DEBUG
        : dist{-dInfinity}, depth{-dInfinity}
#endif
    {}
};
