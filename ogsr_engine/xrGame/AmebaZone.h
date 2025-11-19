#pragma once

class CAmebaZone : public CVisualZone, public CPHUpdateObject
{
    RTTI_DECLARE_TYPEINFO(CAmebaZone, CVisualZone, CPHUpdateObject);

public:
    typedef CVisualZone inherited;

    float m_fVelocityLimit{1.0f};

    CAmebaZone();
    ~CAmebaZone() override;

    virtual void Affect(SZoneObjectInfo* O);

protected:
    virtual void PhTune(dReal step);
    void PhDataUpdate(dReal) override {}
    virtual bool BlowoutState();
    virtual void SwitchZoneState(EZoneState new_state);
    virtual void Load(LPCSTR section);
    virtual float distance_to_center(CObject* O);
};
