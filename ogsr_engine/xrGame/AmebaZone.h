#pragma once

class CAmebaZone : public CVisualZone, public CPHUpdateObject
{
    RTTI_DECLARE_TYPEINFO(CAmebaZone, CVisualZone, CPHUpdateObject);

public:
    typedef CVisualZone inherited;

    float m_fVelocityLimit{1.0f};

    CAmebaZone();
    ~CAmebaZone() override;

    void Affect(SZoneObjectInfo* O) override;

protected:
    void PhTune(f32 step) override;
    void PhDataUpdate(f32) override {}
    [[nodiscard]] bool BlowoutState() override;
    void SwitchZoneState(EZoneState new_state) override;
    void Load(gsl::czstring section) override;
    [[nodiscard]] f32 distance_to_center(CObject* O) override;
};
