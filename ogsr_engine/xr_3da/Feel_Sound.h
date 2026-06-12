#pragma once

#include "../xrSound/Sound.h"

class CObject;

namespace Feel
{
class Sound : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(Sound);

public:
    ~Sound() override = default;

    virtual void feel_sound_new(CObject*, s32, CSound_UserDataPtr, const Fvector3&, f32, f32) {}
};
} // namespace Feel
