////////////////////////////////////////////////////////////////////////////
//	Module 		: item_manager.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Item manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_manager.h"
#include "GameObject.h"

class CCustomMonster;

class CItemManager final : public CObjectManager<const CGameObject>
{
    RTTI_DECLARE_TYPEINFO(CItemManager, CObjectManager<const CGameObject>);

public:
    typedef CObjectManager<const CGameObject> inherited;
    typedef OBJECTS ITEMS;

private:
    CCustomMonster* m_object;

public:
    inline explicit CItemManager(CCustomMonster* object);
    ~CItemManager() override = default;

    [[nodiscard]] bool useful(const CGameObject* object) const;
    [[nodiscard]] bool is_useful(const CGameObject* object) const override;
    [[nodiscard]] f32 evaluate(const CGameObject* object) const;
    [[nodiscard]] f32 do_evaluate(const CGameObject* object) const override;
    void update() override;
    void remove_links(CObject* object);
    void on_restrictions_change();
};

#include "item_manager_inline.h"
