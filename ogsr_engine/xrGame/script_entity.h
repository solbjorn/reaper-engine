////////////////////////////////////////////////////////////////////////////
//	Module 		: script_entity.h
//	Created 	: 06.10.2003
//  Modified 	: 14.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Script entity class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_entity_space.h"
#include "..\Include/xrRender/KinematicsAnimated.h"

class CSE_Abstract;
class CGameObject;
class CScriptEntityAction;
class CEntity;
class CScriptGameObject;
class CCustomMonster;

class CScriptEntity : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CScriptEntity);

public:
    struct CSavedSound
    {
        CScriptGameObject* m_lua_game_object;
        int m_sound_type;
        Fvector m_position;
        float m_sound_power;
        float m_time_to_stop;

        IC CSavedSound(CScriptGameObject* lua_game_object, int sound_type, const Fvector& position, float sound_power, float time_to_stop)
            : m_lua_game_object(lua_game_object), m_sound_type(sound_type), m_position(position), m_sound_power(sound_power), m_time_to_stop(time_to_stop)
        {}
    };

protected:
    typedef xr_deque<CScriptEntityAction*> ACTIONS;

private:
    CGameObject* m_object{};
    CCustomMonster* m_monster{};
    bool m_initialized{};
    bool m_can_capture{};

protected:
    bool m_bScriptControl{};
    bool m_use_animation_movement_controller{};

    ACTIONS m_tpActionQueue;
    shared_str m_caScriptName;
    MotionID m_tpNextAnimation;
    CScriptEntityAction* m_tpCurrentEntityAction{};

public:
    MotionID m_tpScriptAnimation;

protected:
    ref_sound* m_current_sound{};

    xr_vector<CSavedSound> m_saved_sounds;

public:
    CScriptEntity();
    ~CScriptEntity() override;

    void init();
    virtual void reinit();
    [[nodiscard]] virtual BOOL net_Spawn(CSE_Abstract*);
    virtual void net_Destroy();
    virtual void shedule_Update(u32);
    virtual tmc::task<void> UpdateCL();
    virtual CScriptEntity* cast_script_entity() { return this; }
    virtual DLL_Pure* _construct();

public:
    const Fmatrix GetUpdatedMatrix(shared_str caBoneName, const Fvector& tPositionOffset, const Fvector& tAngleOffset);
    void vfUpdateParticles();
    void vfUpdateSounds();
    virtual void vfFinishAction(CScriptEntityAction* tpEntityAction);
    virtual void SetScriptControl(const bool bScriptControl, shared_str caSciptName);
    virtual bool GetScriptControl() const;
    virtual LPCSTR GetScriptControlName() const;
    virtual bool CheckObjectVisibility(const CGameObject* tpObject);
    virtual bool CheckObjectVisibilityNow(const CGameObject* tpObject);
    virtual bool CheckTypeVisibility(const char* section_name);
    virtual bool CheckIfCompleted() const { return false; }
    virtual CScriptEntityAction* GetCurrentAction();
    virtual void AddAction(const CScriptEntityAction* tpEntityAction, bool bHighPriority = false);
    virtual void ProcessScripts();
    virtual void ResetScriptData(void* = nullptr);
    virtual void ClearActionQueue();
    virtual bool bfAssignMovement(CScriptEntityAction* tpEntityAction);
    virtual bool bfAssignWatch(CScriptEntityAction*);
    virtual bool bfAssignAnimation(CScriptEntityAction*);
    virtual bool bfAssignSound(CScriptEntityAction* tpEntityAction);
    virtual bool bfAssignParticles(CScriptEntityAction* tpEntityAction);
    virtual bool bfAssignObject(CScriptEntityAction*);
    virtual bool bfAssignMonsterAction(CScriptEntityAction*);

    virtual void sound_callback(const CObject* object, int sound_type, const Fvector& position, float sound_power, float time_to_stop);

    virtual LPCSTR GetPatrolPathName();
    bool bfScriptAnimation();
    u32 GetActionCount() const;
    const CScriptEntityAction* GetActionByIndex(u32 action_index) const;

    virtual CEntity* GetCurrentEnemy();
    virtual CEntity* GetCurrentCorpse();
    virtual int get_enemy_strength();

    void process_sound_callbacks();

    void set_script_capture(bool val = true) { m_can_capture = val; }
    bool can_script_capture() const { return m_can_capture; }

public:
    IC CGameObject& object() const;
};

#include "script_entity_inline.h"
