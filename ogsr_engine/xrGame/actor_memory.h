////////////////////////////////////////////////////////////////////////////
//	Module 		: actor_memory.h
//	Created 	: 15.09.2005
//  Modified 	: 15.09.2005
//	Author		: Dmitriy Iassenev
//	Description : actor memory
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "vision_client.h"

class CActor;

class CActorMemory : public vision_client
{
    RTTI_DECLARE_TYPEINFO(CActorMemory, vision_client);

private:
    typedef vision_client inherited;

    CActor* m_actor;

public:
    explicit CActorMemory(CActor* actor);
    ~CActorMemory() override = default;

    [[nodiscard]] BOOL feel_vision_isRelevant(CObject* object) override;
    void camera(Fvector3& position, Fvector3& direction, Fvector3& normal, f32& field_of_view, f32& aspect_ratio, f32& near_plane, f32& far_plane) override;
};
