#pragma once

class XR_NOVTABLE IDamageSource : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IDamageSource);

public:
    virtual ~IDamageSource() = 0;

    virtual void SetInitiator(u16 id) = 0;
    virtual u16 Initiator() = 0;
    virtual IDamageSource* cast_IDamageSource() { return this; }
};

inline IDamageSource::~IDamageSource() = default;
