#pragma once

class XR_NOVTABLE IDamageSource : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IDamageSource);

public:
    ~IDamageSource() override = 0;

    virtual void SetInitiator(u16 id) = 0;
    [[nodiscard]] virtual u16 Initiator() = 0;
    [[nodiscard]] virtual IDamageSource* cast_IDamageSource() { return this; }
};

inline IDamageSource::~IDamageSource() = default;
