////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluators.h
//	Created 	: 12.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object property evaluators
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "property_evaluator_const.h"
#include "property_evaluator_member.h"

class CGameObject;
class CAI_Stalker;
class CWeapon;
class CMissile;
class CFoodItem;
class CWeaponMagazined;

typedef CPropertyEvaluatorMember<CAI_Stalker> CObjectPropertyEvaluatorMember;

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorBase
//////////////////////////////////////////////////////////////////////////

template <typename _item_type>
class CObjectPropertyEvaluatorBase : public CPropertyEvaluator<CAI_Stalker>
{
    RTTI_DECLARE_TYPEINFO(CObjectPropertyEvaluatorBase<_item_type>, CPropertyEvaluator<CAI_Stalker>);

protected:
    typedef CPropertyEvaluator<CAI_Stalker> inherited;
    _item_type* m_item;

public:
    inline explicit CObjectPropertyEvaluatorBase(_item_type* item, CAI_Stalker* owner);
    ~CObjectPropertyEvaluatorBase() override = default;

    IC CAI_Stalker& object() const;
};

typedef CObjectPropertyEvaluatorBase<CGameObject> CSObjectPropertyEvaluatorBase;
typedef CPropertyEvaluatorConst<CAI_Stalker> CObjectPropertyEvaluatorConst;

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorState
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorState : public CObjectPropertyEvaluatorBase<CWeapon>
{
    RTTI_DECLARE_TYPEINFO(CObjectPropertyEvaluatorState, CObjectPropertyEvaluatorBase<CWeapon>);

protected:
    typedef CObjectPropertyEvaluatorBase<CWeapon> inherited;
    u32 m_state;
    bool m_equality;

public:
    explicit CObjectPropertyEvaluatorState(CWeapon* item, CAI_Stalker* owner, u32 state, bool equality = true);
    ~CObjectPropertyEvaluatorState() override = default;

    virtual _value_type evaluate();
};

class CObjectPropertyEvaluatorWeaponHidden : public CObjectPropertyEvaluatorBase<CWeapon>
{
    RTTI_DECLARE_TYPEINFO(CObjectPropertyEvaluatorWeaponHidden, CObjectPropertyEvaluatorBase<CWeapon>);

protected:
    typedef CObjectPropertyEvaluatorBase<CWeapon> inherited;

public:
    explicit CObjectPropertyEvaluatorWeaponHidden(CWeapon* item, CAI_Stalker* owner);
    ~CObjectPropertyEvaluatorWeaponHidden() override = default;

    virtual _value_type evaluate();
};

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorAmmo
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorAmmo : public CObjectPropertyEvaluatorBase<CWeapon>
{
    RTTI_DECLARE_TYPEINFO(CObjectPropertyEvaluatorAmmo, CObjectPropertyEvaluatorBase<CWeapon>);

protected:
    typedef CObjectPropertyEvaluatorBase<CWeapon> inherited;
    u32 m_ammo_type;

public:
    explicit CObjectPropertyEvaluatorAmmo(CWeapon* item, CAI_Stalker* owner, u32 ammo_type);
    ~CObjectPropertyEvaluatorAmmo() override = default;

    virtual _value_type evaluate();
};

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorEmpty
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorEmpty : public CObjectPropertyEvaluatorBase<CWeapon>
{
    RTTI_DECLARE_TYPEINFO(CObjectPropertyEvaluatorEmpty, CObjectPropertyEvaluatorBase<CWeapon>);

protected:
    typedef CObjectPropertyEvaluatorBase<CWeapon> inherited;
    u32 m_ammo_type;

public:
    explicit CObjectPropertyEvaluatorEmpty(CWeapon* item, CAI_Stalker* owner, u32 ammo_type);
    ~CObjectPropertyEvaluatorEmpty() override = default;

    virtual _value_type evaluate();
};

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorFull
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorFull : public CObjectPropertyEvaluatorBase<CWeapon>
{
    RTTI_DECLARE_TYPEINFO(CObjectPropertyEvaluatorFull, CObjectPropertyEvaluatorBase<CWeapon>);

protected:
    typedef CObjectPropertyEvaluatorBase<CWeapon> inherited;
    u32 m_ammo_type;

public:
    explicit CObjectPropertyEvaluatorFull(CWeapon* item, CAI_Stalker* owner, u32 ammo_type);
    ~CObjectPropertyEvaluatorFull() override = default;

    virtual _value_type evaluate();
};

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorReady
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorReady : public CObjectPropertyEvaluatorBase<CWeapon>
{
    RTTI_DECLARE_TYPEINFO(CObjectPropertyEvaluatorReady, CObjectPropertyEvaluatorBase<CWeapon>);

protected:
    typedef CObjectPropertyEvaluatorBase<CWeapon> inherited;
    u32 m_ammo_type;

public:
    explicit CObjectPropertyEvaluatorReady(CWeapon* item, CAI_Stalker* owner, u32 ammo_type);
    ~CObjectPropertyEvaluatorReady() override = default;

    virtual _value_type evaluate();
};

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorQueue
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorQueue : public CObjectPropertyEvaluatorBase<CWeapon>
{
    RTTI_DECLARE_TYPEINFO(CObjectPropertyEvaluatorQueue, CObjectPropertyEvaluatorBase<CWeapon>);

protected:
    typedef CObjectPropertyEvaluatorBase<CWeapon> inherited;

protected:
    u32 m_type;
    CWeaponMagazined* m_magazined;

public:
    explicit CObjectPropertyEvaluatorQueue(CWeapon* item, CAI_Stalker* owner, u32 type);
    ~CObjectPropertyEvaluatorQueue() override = default;

    virtual _value_type evaluate();
};

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorNoItems
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorNoItems : public CPropertyEvaluator<CAI_Stalker>
{
    RTTI_DECLARE_TYPEINFO(CObjectPropertyEvaluatorNoItems, CPropertyEvaluator<CAI_Stalker>);

protected:
    typedef CPropertyEvaluator<CAI_Stalker> inherited;

public:
    explicit CObjectPropertyEvaluatorNoItems(CAI_Stalker* owner);
    ~CObjectPropertyEvaluatorNoItems() override = default;

    virtual _value_type evaluate();
    IC CAI_Stalker& object() const;
};

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorMissile
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorMissile : public CObjectPropertyEvaluatorBase<CMissile>
{
    RTTI_DECLARE_TYPEINFO(CObjectPropertyEvaluatorMissile, CObjectPropertyEvaluatorBase<CMissile>);

protected:
    typedef CObjectPropertyEvaluatorBase<CMissile> inherited;
    u32 m_state;
    bool m_equality;

public:
    explicit CObjectPropertyEvaluatorMissile(CMissile* item, CAI_Stalker* owner, u32 state, bool equality = true);
    ~CObjectPropertyEvaluatorMissile() override = default;

    virtual _value_type evaluate();
};

#include "object_property_evaluators_inline.h"
