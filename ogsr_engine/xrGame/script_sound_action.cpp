////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound_action.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script sound action class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "script_sound_action.h"
#include "ai_space.h"
#include "script_engine.h"

CScriptSoundAction::~CScriptSoundAction() {}

void CScriptSoundAction::SetSound(LPCSTR caSoundToPlay)
{
    m_caSoundToPlay._set(caSoundToPlay);
    m_tGoalType = eGoalTypeSoundAttached;
    m_bStartedToPlay = false;
    string_path l_caFileName;

    if (FS.exist(l_caFileName, "$game_sounds$", *m_caSoundToPlay, ".ogg"))
    {
        m_bStartedToPlay = false;
        m_bCompleted = false;
    }
    else
    {
        ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "File not found \"%s\"!", l_caFileName);
        m_bStartedToPlay = true;
        m_bCompleted = true;
    }
}

void CScriptSoundAction::clone(const CScriptSoundAction& from)
{
    CScriptAbstractAction::clone(from);

    m_caSoundToPlay = from.m_caSoundToPlay;
    m_caBoneName = from.m_caBoneName;
    m_tGoalType = from.m_tGoalType;
    m_bLooped = from.m_bLooped;
    m_bStartedToPlay = from.m_bStartedToPlay;
    m_tSoundPosition = from.m_tSoundPosition;
    m_tSoundAngles = from.m_tSoundAngles;
    m_sound_type = from.m_sound_type;
    m_monster_sound = from.m_monster_sound;
    m_monster_sound_delay = from.m_monster_sound_delay;
    m_tHeadAnimType = from.m_tHeadAnimType;
}
