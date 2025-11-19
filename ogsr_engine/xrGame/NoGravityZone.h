#pragma once

#include "CustomZone.h"

class CNoGravityZone : public CCustomZone
{
    RTTI_DECLARE_TYPEINFO(CNoGravityZone, CCustomZone);

private:
    typedef CCustomZone inherited;

public:
    CNoGravityZone() = default;
    ~CNoGravityZone() override = default;

protected:
    virtual void enter_Zone(SZoneObjectInfo& io);
    virtual void exit_Zone(SZoneObjectInfo& io);

private:
    void switchGravity(SZoneObjectInfo& io, bool val);
    virtual void UpdateWorkload(u32);
};
