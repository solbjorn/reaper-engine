////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_human_object_handler.h
//	Created 	: 07.10.2005
//  Modified 	: 07.10.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife human object handler class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"

class CSE_ALifeItemWeapon;
class CSE_ALifeInventoryItem;
class CSE_ALifeGroupAbstract;
class CSE_ALifeHumanAbstract;

class CALifeHumanObjectHandler
{
public:
    typedef CSE_ALifeHumanAbstract object_type;

private:
    object_type* m_object;

public:
    inline CALifeHumanObjectHandler(object_type* object);
    inline object_type& object() const;

    [[nodiscard]] u16 get_available_ammo_count(const CSE_ALifeItemWeapon*, ALife::OBJECT_VECTOR&);
    [[nodiscard]] u16 get_available_ammo_count(const CSE_ALifeItemWeapon*, ALife::ITEM_P_VECTOR&, ALife::OBJECT_VECTOR* = nullptr);
    void attach_available_ammo(CSE_ALifeItemWeapon*, ALife::ITEM_P_VECTOR&, ALife::OBJECT_VECTOR* = nullptr);
    [[nodiscard]] bool can_take_item(CSE_ALifeInventoryItem*);
    void collect_ammo_boxes();

    void detach_all(bool);
    void update_weapon_ammo();
    void process_items();
    CSE_ALifeDynamicObject* best_detector();
    CSE_ALifeItemWeapon* best_weapon();

    [[nodiscard]] int choose_equipment(ALife::OBJECT_VECTOR* = nullptr);
    [[nodiscard]] int choose_weapon(const ALife::EWeaponPriorityType&, ALife::OBJECT_VECTOR* = nullptr);
    [[nodiscard]] int choose_food(ALife::OBJECT_VECTOR* = nullptr);
    [[nodiscard]] int choose_medikit(ALife::OBJECT_VECTOR* = nullptr);

    [[nodiscard]] int choose_valuables();
    [[nodiscard]] bool choose_fast();
    void choose_group(CSE_ALifeGroupAbstract*);
    void attach_items();
};

#include "alife_human_object_handler_inline.h"
