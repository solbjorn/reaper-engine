#pragma once

#include "customzone.h"
#include "script_export_space.h"

class CMosquitoBald : public CCustomZone
{
    RTTI_DECLARE_TYPEINFO(CMosquitoBald, CCustomZone);

private:
    typedef CCustomZone inherited;

public:
    CMosquitoBald();
    ~CMosquitoBald() override;

    void Load(gsl::czstring section) override;
    void Postprocess(f32) override;

    void Affect(SZoneObjectInfo* O) override;

protected:
    [[nodiscard]] bool BlowoutState() override;

    // для того чтобы blowout обновился один раз
    // после того как зона перключилась в другое состояние
    bool m_bLastBlowoutUpdate{};

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CMosquitoBald);

add_to_type_list(CMosquitoBald);
#undef script_type_list
#define script_type_list save_type_list(CMosquitoBald)
