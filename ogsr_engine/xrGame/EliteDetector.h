#pragma once

#include "CustomDetector.h"
#include "Level.h"

class CUIArtefactDetectorElite;

class CEliteDetector : public CCustomDetector
{
    RTTI_DECLARE_TYPEINFO(CEliteDetector, CCustomDetector);

private:
    typedef CCustomDetector inherited;

public:
    CEliteDetector();
    ~CEliteDetector() override = default;

    void render_item_3d_ui() override;
    bool render_item_3d_ui_query() override;
    [[nodiscard]] virtual gsl::czstring ui_xml_tag() const { return "elite"; }

protected:
    void UpdateAf() override;
    void CreateUI() override;
    CUIArtefactDetectorElite& ui();
};

class CScientificDetector : public CEliteDetector
{
    RTTI_DECLARE_TYPEINFO(CScientificDetector, CEliteDetector);

private:
    typedef CEliteDetector inherited;

public:
    CScientificDetector();
    ~CScientificDetector() override;

    void Load(gsl::czstring section) override;
    void OnH_B_Independent(bool just_before_destroy) override;
    tmc::task<void> shedule_Update(u32 dt) override;
    [[nodiscard]] gsl::czstring ui_xml_tag() const override { return "scientific"; }

protected:
    void UpfateWork() override;

    CZoneList m_zones;
};
