#pragma once

#include "pure_relcase.h"

class CObject;

namespace Feel
{
class Touch : public virtual RTTI::Enable, private pure_relcase
{
    RTTI_DECLARE_TYPEINFO(Touch, pure_relcase);

public:
    friend class pure_relcase;

    struct DenyTouch
    {
        CObject* O;
        DWORD Expire;
    };

protected:
    xr_vector<DenyTouch> feel_touch_disable;

public:
    xr_vector<CObject*> feel_touch;
    xr_vector<CObject*> q_nearest;

public:
    void feel_touch_relcase(CObject*);

public:
    Touch();
    virtual ~Touch();

    virtual BOOL feel_touch_contact(CObject* O);
    virtual void feel_touch_update(Fvector&, float, const std::function<void(CObject*, bool)>& = {}, const std::function<bool(CObject*)>& = {});
    virtual void feel_touch_deny(CObject* O, DWORD T);
    virtual void feel_touch_new(CObject* O) {};
    virtual void feel_touch_delete(CObject* O) {};
    virtual void feel_touch_relcase2(CObject*, const std::function<void(CObject*, bool)>& = {});
};
}; // namespace Feel
