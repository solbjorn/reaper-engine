#pragma once

struct XR_NOVTABLE SEnumVerticesCallback
{
    virtual ~SEnumVerticesCallback() = 0;

    virtual void operator()(const Fvector& p) = 0;
};

inline SEnumVerticesCallback::~SEnumVerticesCallback() = default;
