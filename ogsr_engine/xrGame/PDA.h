#pragma once

#include "..\xr_3da\feel_touch.h"
#include "hud_item_object.h"
#include "InfoPortionDefs.h"
#include "character_info_defs.h"
#include "PdaMsg.h"
#include "HudSound.h"

class CPda : public CHudItemObject, public Feel::Touch
{
    RTTI_DECLARE_TYPEINFO(CPda, CHudItemObject, Feel::Touch);

public:
    using inherited = CHudItemObject;

    CPda();
    ~CPda() override;

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    void Load(gsl::czstring section) override;
    tmc::task<void> net_Destroy() override;
    void net_Relcase(CObject* O) override;

    void OnH_A_Chield() override;
    void OnH_B_Independent(bool just_before_destroy) override;

    tmc::task<void> shedule_Update(u32 dt) override;

    void feel_touch_new(CObject* O) override;
    void feel_touch_delete(CObject* O) override;
    [[nodiscard]] BOOL feel_touch_contact(CObject* O) override;

    u16 GetOriginalOwnerID() const { return m_idOriginalOwner; }
    CInventoryOwner* GetOriginalOwner() const;

    void TurnOn();
    void TurnOff();

    bool IsActive() const { return IsOn(); }
    bool IsOn() const { return !m_bTurnedOff; }
    bool IsOff() const { return m_bTurnedOff; }

    xr_map<u16, CPda*> ActivePDAContacts();
    CPda* GetPdaFromOwner(CObject* owner);
    u32 ActiveContactsNum() const { return m_active_contacts.size(); }

    void save(NET_Packet& output_packet) override;
    void load(IReader& input_packet) override;

    [[nodiscard]] gsl::czstring Name() override;

private:
    void UpdateActiveContacts();

    xr_vector<CObject*> m_active_contacts;
    float m_fRadius;
    bool m_changed;

    u16 m_idOriginalOwner;
    shared_str m_SpecificChracterOwner;
    xr_string m_sFullName;

    bool m_bTurnedOff;

    const char* m_joystick_bone{};
    u16 joystick{BI_NONE};
    static void JoystickCallback(CBoneInstance* B);

    HUD_SOUND sndShow, sndHide, sndBtnPress, sndBtnRelease;

    bool this_is_3d_pda{};

public:
    bool Is3DPDA() const { return this_is_3d_pda; }
    void OnStateSwitch(u32 S, u32 oldState) override;
    void OnAnimationEnd(u32 state) override;

    void PlayAnimIdle() override;
    bool ThumbAnimsAllowed() const { return joystick == BI_NONE; }

    void OnMoveToRuck(EItemPlace prevPlace) override;
    tmc::task<void> UpdateCL() override;
    void UpdateXForm() override;
    void OnActiveItem() override;
    void OnHiddenItem() override;

    bool m_bZoomed{};
    float m_thumb_rot[2]{};
    xr_string thumb_anim_name;

    u8 GetCurrentHudOffsetIdx() const override { return IsZoomed() ? 1 : 0; }
    bool IsZoomed() const override { return m_bZoomed; }
};
