#ifndef RainRender_included
#define RainRender_included

class CEffect_Rain;

#include "../../xrCore/_sphere.h"

class XR_NOVTABLE IRainRender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IRainRender);

public:
    virtual ~IRainRender() = 0;

    virtual void Calculate(CEffect_Rain& owner) = 0;
    virtual void Render(CEffect_Rain& owner) = 0;

    virtual const Fsphere& GetDropBounds() const = 0;
};

inline IRainRender::~IRainRender() = default;

#endif //	RainRender_included
