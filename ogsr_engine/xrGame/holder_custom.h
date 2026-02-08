#pragma once

// #include "gameobject.h"

#include "script_export_space.h"

class CActor;
class CCameraBase;
class CGameObject;
class CInventory;
enum class EGameActions : s32;

class XR_NOVTABLE CHolderCustom : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CHolderCustom);

private:
    CGameObject* m_owner{};
    CActor* m_ownerActor{};

public:
    CHolderCustom() = default;
    ~CHolderCustom() override = 0;

    CGameObject* Owner() { return m_owner; }
    CGameObject* Owner() const { return m_owner; }
    CActor* OwnerActor() { return m_ownerActor; }
    CActor* OwnerActor() const { return m_ownerActor; }

    virtual tmc::task<void> UpdateEx(f32) { co_return; } // called by owner
    virtual CHolderCustom* cast_holder_custom() { return this; }
    bool Engaged() { return !!m_owner; }
    virtual void OnMouseMove(int x, int y) = 0;
    virtual void OnKeyboardPress(EGameActions cmd) = 0;
    virtual void OnKeyboardRelease(EGameActions cmd) = 0;
    virtual void OnKeyboardHold(EGameActions cmd) = 0;
    // Inventory for the car
    virtual CInventory* GetInventory() = 0;

    virtual tmc::task<void> cam_Update(f32 dt, f32 fov = 90.0f) = 0;

    virtual bool Use(const Fvector& pos, const Fvector& dir, const Fvector& foot_pos) = 0;
    virtual bool attach_Actor(CGameObject* actor);
    virtual void detach_Actor();
    virtual bool allowWeapon() const = 0;
    virtual bool HUDView() const = 0;
    virtual Fvector ExitPosition() = 0;
    [[nodiscard]] virtual Fvector ExitVelocity() { return Fvector{}; }
    virtual CCameraBase* Camera() = 0;
    virtual void Action(EGameActions, u32) {}
    virtual void SetParam(int, Fvector2) {}
    virtual void SetParam(int, Fvector) {}

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};

inline CHolderCustom::~CHolderCustom() = default;

add_to_type_list(CHolderCustom);
#undef script_type_list
#define script_type_list save_type_list(CHolderCustom)
