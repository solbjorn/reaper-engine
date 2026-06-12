////////////////////////////////////////////////////////////////////////////
//	Module 		: object_actions.h
//	Created 	: 12.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object actions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_base.h"

class CAI_Stalker;
class CInventoryItem;
class CFoodItem;
class CWeaponMagazined;
class CWeapon;

//////////////////////////////////////////////////////////////////////////
// CObjectActionBase
//////////////////////////////////////////////////////////////////////////

template <typename _item_type>
class CObjectActionBase : public CActionBase<CAI_Stalker>
{
    RTTI_DECLARE_TYPEINFO(CObjectActionBase<_item_type>, CActionBase<CAI_Stalker>);

protected:
    typedef CActionBase<CAI_Stalker> inherited;
    typedef GraphEngineSpace::_solver_condition_type _condition_type;
    typedef GraphEngineSpace::_solver_value_type _value_type;

protected:
    _item_type* m_item;

public:
    inline explicit CObjectActionBase(_item_type* item, CAI_Stalker* owner, CPropertyStorage* storage, LPCSTR action_name = "");
    ~CObjectActionBase() override = default;

    void initialize() override;
    IC void set_property(_condition_type condition_id, _value_type value);
    [[nodiscard]] inline CAI_Stalker& object() const;
};

class CGameObject;

typedef CObjectActionBase<CGameObject> CSObjectActionBase;

//////////////////////////////////////////////////////////////////////////
// CObjectActionMember
//////////////////////////////////////////////////////////////////////////

template <typename _item_type>
class CObjectActionMember : public CObjectActionBase<_item_type>
{
    RTTI_DECLARE_TYPEINFO(CObjectActionMember<_item_type>, CObjectActionBase<_item_type>);

protected:
    typedef CObjectActionBase<_item_type> inherited;

    using inherited::completed;
    using inherited::set_property;

public:
    using _condition_type = typename inherited::_condition_type;
    using _value_type = typename inherited::_value_type;

protected:
    _condition_type m_condition_id;
    _value_type m_value;

public:
    inline explicit CObjectActionMember(_item_type* item, CAI_Stalker* owner, CPropertyStorage* storage, _condition_type condition_id, _value_type value,
                                        LPCSTR action_name = "");
    ~CObjectActionMember() override = default;

    void execute() override;
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionCommand
//////////////////////////////////////////////////////////////////////////

class CObjectActionCommand : public CObjectActionBase<CInventoryItem>
{
    RTTI_DECLARE_TYPEINFO(CObjectActionCommand, CObjectActionBase<CInventoryItem>);

protected:
    typedef CObjectActionBase<CInventoryItem> inherited;

    EGameActions m_command;

public:
    explicit CObjectActionCommand(CInventoryItem* item, CAI_Stalker* owner, CPropertyStorage* storage, EGameActions command, gsl::czstring action_name = "");
    ~CObjectActionCommand() override = default;

    void initialize() override;
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionShow
//////////////////////////////////////////////////////////////////////////

class CObjectActionShow : public CObjectActionBase<CInventoryItem>
{
    RTTI_DECLARE_TYPEINFO(CObjectActionShow, CObjectActionBase<CInventoryItem>);

protected:
    typedef CObjectActionBase<CInventoryItem> inherited;

private:
    CWeapon* m_weapon;

public:
    explicit CObjectActionShow(CInventoryItem* item, CAI_Stalker* owner, CPropertyStorage* storage, LPCSTR action_name = "");
    ~CObjectActionShow() override = default;

    void initialize() override;
    void execute() override;
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionHide
//////////////////////////////////////////////////////////////////////////

class CObjectActionHide : public CObjectActionBase<CInventoryItem>
{
    RTTI_DECLARE_TYPEINFO(CObjectActionHide, CObjectActionBase<CInventoryItem>);

protected:
    typedef CObjectActionBase<CInventoryItem> inherited;

public:
    explicit CObjectActionHide(CInventoryItem* item, CAI_Stalker* owner, CPropertyStorage* storage, LPCSTR action_name = "");
    ~CObjectActionHide() override = default;

    void execute() override;
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionReload
//////////////////////////////////////////////////////////////////////////

class CObjectActionReload : public CObjectActionBase<CInventoryItem>
{
    RTTI_DECLARE_TYPEINFO(CObjectActionReload, CObjectActionBase<CInventoryItem>);

protected:
    typedef CObjectActionBase<CInventoryItem> inherited;

protected:
    _condition_type m_type;

public:
    explicit CObjectActionReload(CInventoryItem* item, CAI_Stalker* owner, CPropertyStorage* storage, _condition_type type, LPCSTR action_name = "");
    ~CObjectActionReload() override = default;

    void initialize() override;
    void execute() override;
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionFire
//////////////////////////////////////////////////////////////////////////

class CObjectActionFire : public CObjectActionBase<CInventoryItem>
{
    RTTI_DECLARE_TYPEINFO(CObjectActionFire, CObjectActionBase<CInventoryItem>);

protected:
    typedef CObjectActionBase<CInventoryItem> inherited;

protected:
    _condition_type m_type;

public:
    explicit CObjectActionFire(CInventoryItem* item, CAI_Stalker* owner, CPropertyStorage* storage, _condition_type type, LPCSTR action_name = "");
    ~CObjectActionFire() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionStrapping
//////////////////////////////////////////////////////////////////////////

class CObjectActionStrapping : public CObjectActionBase<CInventoryItem>
{
    RTTI_DECLARE_TYPEINFO(CObjectActionStrapping, CObjectActionBase<CInventoryItem>);

protected:
    typedef CObjectActionBase<CInventoryItem> inherited;

private:
    bool m_callback_removed;

    void on_animation_end();

public:
    explicit CObjectActionStrapping(CInventoryItem* item, CAI_Stalker* owner, CPropertyStorage* storage, LPCSTR action_name = "");
    ~CObjectActionStrapping() override;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionStrappingToIdle
//////////////////////////////////////////////////////////////////////////

class CObjectActionStrappingToIdle : public CObjectActionBase<CInventoryItem>
{
    RTTI_DECLARE_TYPEINFO(CObjectActionStrappingToIdle, CObjectActionBase<CInventoryItem>);

protected:
    typedef CObjectActionBase<CInventoryItem> inherited;

private:
    bool m_callback_removed;

private:
    void on_animation_end();

public:
    explicit CObjectActionStrappingToIdle(CInventoryItem* item, CAI_Stalker* owner, CPropertyStorage* storage, LPCSTR action_name = "");
    ~CObjectActionStrappingToIdle() override;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionUnstrapping
//////////////////////////////////////////////////////////////////////////

class CObjectActionUnstrapping : public CObjectActionBase<CInventoryItem>
{
    RTTI_DECLARE_TYPEINFO(CObjectActionUnstrapping, CObjectActionBase<CInventoryItem>);

protected:
    typedef CObjectActionBase<CInventoryItem> inherited;

private:
    bool m_callback_removed;

private:
    void on_animation_end();

public:
    explicit CObjectActionUnstrapping(CInventoryItem* item, CAI_Stalker* owner, CPropertyStorage* storage, LPCSTR action_name = "");
    ~CObjectActionUnstrapping() override;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionUnstrappingToIdle
//////////////////////////////////////////////////////////////////////////

class CObjectActionUnstrappingToIdle : public CObjectActionBase<CInventoryItem>
{
    RTTI_DECLARE_TYPEINFO(CObjectActionUnstrappingToIdle, CObjectActionBase<CInventoryItem>);

protected:
    typedef CObjectActionBase<CInventoryItem> inherited;

private:
    bool m_callback_removed;

    void on_animation_end();

public:
    explicit CObjectActionUnstrappingToIdle(CInventoryItem* item, CAI_Stalker* owner, CPropertyStorage* storage, LPCSTR action_name = "");
    ~CObjectActionUnstrappingToIdle() override;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionQueueWait
//////////////////////////////////////////////////////////////////////////

class CObjectActionQueueWait : public CObjectActionBase<CInventoryItem>
{
    RTTI_DECLARE_TYPEINFO(CObjectActionQueueWait, CObjectActionBase<CInventoryItem>);

protected:
    typedef CObjectActionBase<CInventoryItem> inherited;

    _condition_type m_type;
    CWeaponMagazined* m_magazined;

public:
    explicit CObjectActionQueueWait(CInventoryItem* item, CAI_Stalker* owner, CPropertyStorage* storage, _condition_type type, LPCSTR action_name = "");
    ~CObjectActionQueueWait() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionSwitch
//////////////////////////////////////////////////////////////////////////

class CObjectActionSwitch : public CObjectActionBase<CInventoryItem>
{
    RTTI_DECLARE_TYPEINFO(CObjectActionSwitch, CObjectActionBase<CInventoryItem>);

protected:
    typedef CObjectActionBase<CInventoryItem> inherited;

    _condition_type m_type;

public:
    explicit CObjectActionSwitch(CInventoryItem* item, CAI_Stalker* owner, CPropertyStorage* storage, _condition_type type, LPCSTR action_name = "");
    ~CObjectActionSwitch() override = default;

    void initialize() override;
    void execute() override;
    void finalize() override;
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionDrop
//////////////////////////////////////////////////////////////////////////

class CObjectActionDrop : public CObjectActionBase<CInventoryItem>
{
    RTTI_DECLARE_TYPEINFO(CObjectActionDrop, CObjectActionBase<CInventoryItem>);

protected:
    typedef CObjectActionBase<CInventoryItem> inherited;

public:
    explicit CObjectActionDrop(CInventoryItem* item, CAI_Stalker* owner, CPropertyStorage* storage, LPCSTR action_name = "");
    ~CObjectActionDrop() override = default;

    void initialize() override;
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionThreaten
//////////////////////////////////////////////////////////////////////////

class CObjectActionThreaten : public CObjectActionBase<CAI_Stalker>
{
    RTTI_DECLARE_TYPEINFO(CObjectActionThreaten, CObjectActionBase<CAI_Stalker>);

protected:
    typedef CObjectActionBase<CAI_Stalker> inherited;

public:
    explicit CObjectActionThreaten(CAI_Stalker* item, CAI_Stalker* owner, CPropertyStorage* storage, LPCSTR action_name = "");
    ~CObjectActionThreaten() override = default;

    void execute() override;
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionAim
//////////////////////////////////////////////////////////////////////////

class CObjectActionAim : public CObjectActionMember<CInventoryItem>
{
    RTTI_DECLARE_TYPEINFO(CObjectActionAim, CObjectActionMember<CInventoryItem>);

protected:
    typedef CObjectActionMember<CInventoryItem> inherited;

private:
    CWeaponMagazined* m_weapon;

public:
    explicit CObjectActionAim(CInventoryItem* item, CAI_Stalker* owner, CPropertyStorage* storage, _condition_type condition_id, _value_type value,
                              LPCSTR action_name = "");
    ~CObjectActionAim() override = default;

    void initialize() override;
    void execute() override;
};

//////////////////////////////////////////////////////////////////////////
// CObjectActionIdle
//////////////////////////////////////////////////////////////////////////

class CObjectActionIdle : public CObjectActionBase<CInventoryItem>
{
    RTTI_DECLARE_TYPEINFO(CObjectActionIdle, CObjectActionBase<CInventoryItem>);

protected:
    typedef CObjectActionBase<CInventoryItem> inherited;

public:
    explicit CObjectActionIdle(CInventoryItem* item, CAI_Stalker* owner, CPropertyStorage* storage, LPCSTR action_name = "");
    ~CObjectActionIdle() override = default;

    void initialize() override;
};

#include "object_actions_inline.h"
