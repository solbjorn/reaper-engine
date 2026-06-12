#ifndef PH_SPLITED_SELL
#define PH_SPLITED_SELL

#include "PHShell.h"

class CPHSplitedShell : public CPHShell
{
    RTTI_DECLARE_TYPEINFO(CPHSplitedShell, CPHShell);

private:
    float m_max_AABBradius{dInfinity};

    void SetMaxAABBRadius(f32 size) override { m_max_AABBradius = size; }

protected:
    void Collide() override;
    void get_spatial_params() override;
    void DisableObject() override;

public:
    CPHSplitedShell() = default;
    ~CPHSplitedShell() override = default;
};

#endif
