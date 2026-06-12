#pragma once

#include "hud_item_object.h"
#include "hit_immunity.h"
#include "PHObject.h"
#include "script_export_space.h"
#include "patrol_path.h"

struct SArtefactActivation;

struct SArtefactDetectorsSupport
{
    CArtefact* m_parent;
    ref_sound m_sound;

    Fvector m_path_moving_force;
    u32 m_switchVisTime{};
    const CPatrolPath* m_currPatrolPath{};
    const CPatrolPath::CVertex* m_currPatrolVertex{};
    Fvector m_destPoint;

    SArtefactDetectorsSupport(CArtefact* A);
    ~SArtefactDetectorsSupport();
    void SetVisible(bool);
    void FollowByPath(LPCSTR path_name, int start_idx, Fvector force);
    void UpdateOnFrame();
    void Blink();
};

class CArtefact : public CHudItemObject, public CPHUpdateObject
{
    RTTI_DECLARE_TYPEINFO(CArtefact, CHudItemObject, CPHUpdateObject);

private:
    typedef CHudItemObject inherited;

public:
    CArtefact();
    ~CArtefact() override = default;

    void Load(gsl::czstring section) override;

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;

    void OnH_A_Chield() override;
    void OnH_B_Independent(bool just_before_destroy) override;

    tmc::task<void> UpdateCL() override;
    tmc::task<void> shedule_Update(u32 dt) override;
    tmc::task<void> UpdateWorkload();

    [[nodiscard]] bool CanTake() const override;

    [[nodiscard]] BOOL renderable_ShadowGenerate() override { return FALSE; }
    [[nodiscard]] BOOL renderable_ShadowReceive() override { return TRUE; }
    void create_physic_shell() override;

    // for smart_cast
    [[nodiscard]] CArtefact* cast_artefact() override { return this; }

protected:
    virtual void UpdateCLChild() {}

    u16 m_CarringBoneID;
    shared_str m_sParticlesName;

protected:
    SArtefactActivation* m_activationObj;
    //////////////////////////////////////////////////////////////////////////
    //	Lights
    //////////////////////////////////////////////////////////////////////////
    // флаг, что подсветка может быть включена
    bool m_bLightsEnabled;
    // подсветка во время полета и работы двигателя
    ref_light m_pTrailLight;
    Fcolor m_TrailLightColor;
    float m_fTrailLightRange;

    SArtefactDetectorsSupport* m_detectorObj{};
    u8 m_af_rank{};

protected:
    virtual void UpdateLights();

public:
    virtual void StartLights();
    virtual void StopLights();
    tmc::task<void> ActivateArtefact();
    bool CanBeActivated() { return m_bCanSpawnZone; } // does artefact can spawn anomaly zone

    void PhDataUpdate(dReal step) override;
    void PhTune(dReal) override {}

    bool m_bCanSpawnZone;

    float m_fHealthRestoreSpeed;
    float m_fSatietyRestoreSpeed;
    float m_fPowerRestoreSpeed;
    float m_fBleedingRestoreSpeed;
    float m_fThirstRestoreSpeed;

    float m_additional_weight;
    float m_additional_weight2;

    CHitImmunity m_ArtefactHitImmunities;

public:
    enum EAFHudStates
    {
        eIdle = 0,
        eShowing,
        eHiding,
        eHidden,
        eActivating,
    };

public:
    void Hide(bool = false) override;
    void Show(bool = false) override;
    void UpdateXForm() override;
    [[nodiscard]] bool Action(EGameActions cmd, u32 flags) override;
    void PlayAnimIdle() override;
    void OnStateSwitch(u32 S, u32 oldState) override;
    void OnAnimationEnd(u32 state) override;
    [[nodiscard]] bool IsHidden() const override { return GetState() == eHidden; }
    void GetBriefInfo(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count) override;

    // optimization FAST/SLOW mode
    u32 o_render_frame{};
    bool o_fastmode{};

    void o_switch_2_fast() { o_fastmode = true; }
    void o_switch_2_slow() { o_fastmode = false; }

    void FollowByPath(LPCSTR path_name, int start_idx, Fvector magic_force);
    bool CanBeInvisible();
    void SwitchVisibility(bool);
    void SwitchAfParticles(bool bOn);
    IC u8 GetAfRank() const { return m_af_rank; }

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CArtefact);

add_to_type_list(CArtefact);
#undef script_type_list
#define script_type_list save_type_list(CArtefact)
