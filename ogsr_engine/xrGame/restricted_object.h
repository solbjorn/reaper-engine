////////////////////////////////////////////////////////////////////////////
//	Module 		: restricted_object.h
//	Created 	: 18.08.2004
//  Modified 	: 23.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Restricted object
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"

class CSE_Abstract;
class CCustomMonster;
class CGameObject;

namespace RestrictionSpace
{
enum ERestrictorTypes;
};

template <bool add>
struct CRestrictionPredicate;

class CRestrictedObject : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CRestrictedObject);

public:
    friend struct CRestrictionPredicate<true>;
    friend struct CRestrictionPredicate<false>;

private:
    typedef CGameObject inherited;

private:
    CCustomMonster* m_object;
    mutable bool m_applied;
    mutable bool m_removed;
    bool m_actual;

protected:
    IC void add_object_restriction(ALife::_OBJECT_ID id, const RestrictionSpace::ERestrictorTypes& restrictor_type);
    IC void remove_object_restriction(ALife::_OBJECT_ID id, const RestrictionSpace::ERestrictorTypes& restrictor_type);

public:
    IC CRestrictedObject(CCustomMonster* object);
    virtual ~CRestrictedObject();
    virtual BOOL net_Spawn(CSE_Abstract* data);
    virtual void net_Destroy();
    void add_border(u32 start_vertex_id, float radius) const;
    void add_border(const Fvector& start_position, const Fvector& dest_position) const;
    void add_border(u32 start_vertex_id, u32 dest_vertex_id) const;
    void remove_border() const;
    u32 accessible_nearest(const Fvector& position, Fvector& result) const;
    bool accessible(const Fvector& position) const;
    bool accessible(const Fvector& position, float radius) const;
    bool accessible(u32 level_vertex_id) const;
    bool accessible(u32 level_vertex_id, float radius) const;
    void add_restrictions(const xr_vector<ALife::_OBJECT_ID>& out_restrictions, const xr_vector<ALife::_OBJECT_ID>& in_restrictions);
    void remove_restrictions(const xr_vector<ALife::_OBJECT_ID>& out_restrictions, const xr_vector<ALife::_OBJECT_ID>& in_restrictions);
    void add_restrictions(const shared_str& out_restrictions, const shared_str& in_restrictions);
    void remove_restrictions(const shared_str& out_restrictions, const shared_str& in_restrictions);
    void remove_all_restrictions(const RestrictionSpace::ERestrictorTypes& restrictor_type);
    void remove_all_restrictions();
    shared_str in_restrictions() const;
    shared_str out_restrictions() const;
    shared_str base_in_restrictions() const;
    shared_str base_out_restrictions() const;
    IC bool applied() const;
    IC CCustomMonster& object() const;
    IC bool actual() const;
    void actual(bool value);

public:
#ifdef DEBUG
    IC void initialize();
#endif
};

#include "restricted_object_inline.h"
