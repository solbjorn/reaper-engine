////////////////////////////////////////////////////////////////////////////
//	Module 		: script_watch_action.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script watch action class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_abstract_action.h"
#include "script_export_space.h"

namespace SightManager
{
enum ESightType : u32;
}

class CScriptGameObject;

class CScriptWatchAction : public CScriptAbstractAction
{
    RTTI_DECLARE_TYPEINFO(CScriptWatchAction, CScriptAbstractAction);

public:
    enum EGoalType : u32
    {
        eGoalTypeObject = u32(0),
        eGoalTypeWatchType,
        eGoalTypeDirection,
        eGoalTypeCurrent,
        eGoalTypeDummy = u32(-1),
    };

    CObject* m_tpObjectToWatch{};
    SightManager::ESightType m_tWatchType;
    EGoalType m_tGoalType{eGoalTypeCurrent};
    Fvector m_tWatchVector{};
    shared_str m_bone_to_watch;

    // Searchlight
    Fvector m_tTargetPoint;
    float vel_bone_x{};
    float vel_bone_y{};

    CScriptWatchAction();
    inline explicit CScriptWatchAction(SightManager::ESightType tWatchType);
    inline explicit CScriptWatchAction(SightManager::ESightType tWatchType, const Fvector& tDirection);
    inline explicit CScriptWatchAction(SightManager::ESightType tWatchType, CScriptGameObject* tpObjectToWatch, LPCSTR bone_to_watch = "");
    // Searchlight look ///////////////////////////////////////////////
    explicit CScriptWatchAction(const Fvector& tTarget, float vel1, float vel2);
    inline explicit CScriptWatchAction(CScriptGameObject* tpObjectToWatch, float vel1, float vel2);
    ///////////////////////////////////////////////////////////////////
    ~CScriptWatchAction() override;

    void SetWatchObject(CScriptGameObject* tpObjectToWatch);
    IC void SetWatchType(SightManager::ESightType tWatchType);
    IC void SetWatchDirection(const Fvector& tDirection);
    IC void SetWatchBone(LPCSTR bone_to_watch);
    IC void initialize();

    void clone(const CScriptWatchAction& from);

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};

add_to_type_list(CScriptWatchAction);
#undef script_type_list
#define script_type_list save_type_list(CScriptWatchAction)

#include "script_watch_action_inline.h"
