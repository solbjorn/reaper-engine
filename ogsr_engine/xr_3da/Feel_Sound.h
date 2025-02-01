#pragma once

#include "../xrSound/Sound.h"

class CObject;

namespace Feel
{
class Sound
{
public:
    virtual void feel_sound_new(CObject* who, int type, CSound_UserDataPtr user_data, const Fvector& Position, float power) {};
};
}; // namespace Feel
