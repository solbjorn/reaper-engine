////////////////////////////////////////////////////////////////////////////
//	Module 		: vision_client.h
//	Created 	: 11.06.2007
//  Modified 	: 11.06.2007
//	Author		: Dmitriy Iassenev
//	Description : vision client
////////////////////////////////////////////////////////////////////////////

#ifndef VISION_CLIENT_H
#define VISION_CLIENT_H

#include "..\xr_3da\feel_vision.h"

class CObject;
class CEntity;
class CVisualMemoryManager;

class XR_NOVTABLE vision_client : public ISheduled, public Feel::Vision
{
    RTTI_DECLARE_TYPEINFO(vision_client, ISheduled, Feel::Vision);

private:
    typedef ISheduled inherited;

    CEntity* m_object;
    CVisualMemoryManager* m_visual;

    u32 m_state{};
    u32 m_time_stamp{};
    Fvector m_position{};

    IC const CEntity& object() const;

    void eye_pp_s01();
    void eye_pp_s2();

public:
    explicit vision_client(CEntity* object, const u32& update_interval);
    ~vision_client() override;

    [[nodiscard]] f32 shedule_Scale() const override;
    tmc::task<void> shedule_Update(u32 dt) override;
    [[nodiscard]] shared_str shedule_Name() const override;
    [[nodiscard]] bool shedule_Needed() override;

    [[nodiscard]] f32 feel_vision_mtl_transp(CObject* object, u32 element) override;

    [[nodiscard]] BOOL feel_vision_isRelevant(CObject* object) override = 0;
    virtual void camera(Fvector3& position, Fvector3& direction, Fvector3& normal, f32& field_of_view, f32& aspect_ratio, f32& near_plane, f32& far_plane) = 0;

    virtual void reinit();
    virtual void reload(LPCSTR section);
    void remove_links(CObject* object);

    IC CVisualMemoryManager& visual() const;
};

#include "vision_client_inline.h"

#endif // VISION_CLIENT_H
