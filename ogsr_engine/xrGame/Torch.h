#pragma once

#include "inventory_item_object.h"
#include "hudsound.h"
#include "script_export_space.h"

class CLAItem;
class CMonsterEffector;

class CTorch : public CInventoryItemObject
{
    RTTI_DECLARE_TYPEINFO(CTorch, CInventoryItemObject);

private:
    typedef CInventoryItemObject inherited;

protected:
    float fBrightness{1.f};
    CLAItem* lanim{};
    float time2hide{};

    u16 guid_bone{};
    shared_str light_trace_bone;

    float m_delta_h{};
    Fvector2 m_prev_hp{};
    bool m_switched_on{};
    ref_light light_render;
    ref_light light_omni;
    ref_glow glow_render;
    Fvector m_focus;
    Fcolor m_color;

    Fvector m_camera_torch_offset;
    Fvector m_camera_omni_offset;
    f32 m_min_target_dist{0.5f};
    bool m_bind_to_camera{};

private:
    bool useVolumetric{}, useVolumetricForActor{};

public:
    CTorch();
    ~CTorch() override;

    void Load(gsl::czstring section) override;
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    void net_Export(CSE_Abstract* E) override;

    void OnH_A_Chield() override;
    void OnH_B_Independent(bool just_before_destroy) override;

    tmc::task<void> UpdateCL() override;

    void Switch();
    void Switch(bool light_on);
    [[nodiscard]] bool torch_active() const;

    [[nodiscard]] bool can_be_attached() const override;
    void calc_m_delta_h(float);
    float get_range() const;

public:
    void SwitchNightVision();
    void SwitchNightVision(bool light_on);
    void UpdateSwitchNightVision();

protected:
    bool m_bNightVisionEnabled{};
    bool m_bNightVisionOn{};

    HUD_SOUND m_NightVisionOnSnd;
    HUD_SOUND m_NightVisionOffSnd;
    HUD_SOUND m_NightVisionIdleSnd;
    HUD_SOUND m_NightVisionBrokenSnd;
    HUD_SOUND sndTurnOn;
    HUD_SOUND sndTurnOff;

    enum EStats
    {
        eTorchActive = (1 << 0),
        eNightVisionActive = (1 << 1),
        eAttached = (1 << 2)
    };

public:
    [[nodiscard]] bool use_parent_ai_locations() const override { return !H_Parent(); }
    void create_physic_shell() override;
    void activate_physic_shell() override;
    void setup_physic_shell() override;

    void afterDetach() override;

    // alpet: управление светом фонаря
    [[nodiscard]] IRender_Light* GetLight(int target = 0) const;

    void SetAnimation(LPCSTR name);
    void SetBrightness(float brightness);
    void SetColor(const Fcolor& color, int target = 0);
    void SetRGB(float r, float g, float b, int target = 0);
    void SetAngle(float angle, int target = 0);
    void SetRange(float range, int target = 0);
    void SetTexture(LPCSTR texture, int target = 0);
    void SetVirtualSize(float size, int target = 0);

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CTorch);

add_to_type_list(CTorch);
#undef script_type_list
#define script_type_list save_type_list(CTorch)
