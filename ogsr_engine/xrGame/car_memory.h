////////////////////////////////////////////////////////////////////////////
//	Module 		: car_memory.h
//	Created 	: 11.06.2007
//  Modified 	: 11.06.2007
//	Author		: Dmitriy Iassenev
//	Description : car memory
////////////////////////////////////////////////////////////////////////////

#ifndef CAR_MEMORY_H
#define CAR_MEMORY_H

#include "vision_client.h"

class CCar;

class car_memory : public vision_client
{
    RTTI_DECLARE_TYPEINFO(car_memory, vision_client);

private:
    typedef vision_client inherited;

private:
    CCar* m_object;
    float m_fov_deg{};
    float m_aspect{};
    float m_far_plane;
    Fvector m_view_position;
    Fvector m_view_direction;
    Fvector m_view_normal;

public:
    explicit car_memory(CCar* object);
    ~car_memory() override = default;

    void reload(gsl::czstring section) override;

    [[nodiscard]] BOOL feel_vision_isRelevant(CObject* object) override;
    void camera(Fvector3& position, Fvector3& direction, Fvector3& normal, f32& field_of_view, f32& aspect_ratio, f32& near_plane, f32& far_plane) override;
    void set_camera(const Fvector& position, const Fvector& direction, const Fvector& normal);
};

#endif // CAR_MEMORY_H
