#ifndef PH_SPLITED_SELL
#define PH_SPLITED_SELL

#include "PHShell.h"

class CPHSplitedShell : public CPHShell
{
    RTTI_DECLARE_TYPEINFO(CPHSplitedShell, CPHShell);

private:
    float m_max_AABBradius{dInfinity};

    virtual void SetMaxAABBRadius(float size) { m_max_AABBradius = size; }

protected:
    virtual void Collide();
    virtual void get_spatial_params();
    virtual void DisableObject();

public:
    CPHSplitedShell() = default;
    ~CPHSplitedShell() override = default;
};

#endif
