#pragma once

#include "WeaponCustomPistol.h"
#include "script_export_space.h"

class CUIFrameWindow;
class CUIStatic;
class CBinocularsVision;

class CWeaponBinoculars : public CWeaponCustomPistol
{
    RTTI_DECLARE_TYPEINFO(CWeaponBinoculars, CWeaponCustomPistol);

private:
    typedef CWeaponCustomPistol inherited;

protected:
    HUD_SOUND sndZoomIn;
    HUD_SOUND sndZoomOut;

    [[nodiscard]] size_t GetWeaponTypeForCollision() const override { return Binocular; }
    [[nodiscard]] Fvector GetPositionForCollision() override { return Device.vCameraPosition; }
    [[nodiscard]] Fvector GetDirectionForCollision() override { return Device.vCameraDirection; }

public:
    CWeaponBinoculars();
    ~CWeaponBinoculars() override;

    void Load(gsl::czstring section) override;

    void OnZoomIn() override;
    void OnZoomOut() override;
    tmc::task<void> net_Destroy() override;
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;

    void save(NET_Packet& output_packet) override;
    void load(IReader& input_packet) override;

    [[nodiscard]] bool Action(EGameActions cmd, u32 flags) override;
    tmc::task<void> UpdateCL() override;
    void OnDrawUI() override;
    [[nodiscard]] bool use_crosshair() const override { return false; }
    void GetBriefInfo(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count) override;
    void net_Relcase(CObject* object) override;
    float GetZoomFactor() { return m_fRTZoomFactor; }
    void SetZoomFactor(float _zoom) { m_fRTZoomFactor = _zoom; }

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CWeaponBinoculars);

add_to_type_list(CWeaponBinoculars);
#undef script_type_list
#define script_type_list save_type_list(CWeaponBinoculars)
