#pragma once

struct XR_NOVTABLE SEnumVerticesCallback : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(SEnumVerticesCallback);

public:
    ~SEnumVerticesCallback() override = 0;

    virtual void operator()(const Fvector& p) = 0;
};

inline SEnumVerticesCallback::~SEnumVerticesCallback() = default;
