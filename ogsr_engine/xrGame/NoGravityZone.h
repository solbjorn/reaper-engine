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
    void enter_Zone(SZoneObjectInfo& io) override;
    void exit_Zone(SZoneObjectInfo& io) override;

private:
    void switchGravity(SZoneObjectInfo& io, bool val);
    void UpdateWorkload(u32) override;
};
