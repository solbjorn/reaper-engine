#pragma once

#include "../xr_3da/GameMtlLib.h"

class CPhysicsShellHolder;

class CPHSoundPlayer : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CPHSoundPlayer);

public:
    CPhysicsShellHolder* m_object{};
    xr_unordered_map<SGameMtlPair*, ref_sound> m_sound;
    xr_unordered_map<SGameMtlPair*, u32> m_next_snd_time;

    void Init(CPhysicsShellHolder* m_object);

    void Play(SGameMtlPair* mtl_pair, Fvector* pos, bool check_vel = true, float* vol = nullptr);
    void PlayNext(SGameMtlPair* mtl_pair, Fvector* pos, bool check_vel = true, float* vol = nullptr);

    CPHSoundPlayer() = default;
    explicit CPHSoundPlayer(CPhysicsShellHolder* m_object);
    ~CPHSoundPlayer() override;
};
