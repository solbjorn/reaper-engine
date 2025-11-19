////////////////////////////////////////////////////////////////////////////
//	Module 		: object_manager.h
//	Created 	: 30.12.2003
//  Modified 	: 30.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Object manager
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename T>
class CObjectManager : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CObjectManager<T>);

public:
    typedef xr_vector<T*> OBJECTS;

protected:
    OBJECTS m_objects;
    T* m_selected;

public:
    CObjectManager() = default;
    ~CObjectManager() override = default;

    inline virtual void Load(LPCSTR);
    inline virtual void reinit();
    inline virtual void reload(LPCSTR);
    inline virtual void update();
    inline bool add(T* object);
    inline virtual bool is_useful(T* object) const;
    inline virtual float do_evaluate(T*) const;
    inline virtual void reset();

    inline T* selected() const;
    inline const OBJECTS& objects() const;
};

#include "object_manager_inline.h"
