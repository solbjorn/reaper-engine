#include "stdafx.h"

#include "PhraseScript.h"

#include "script_engine.h"
#include "ai_space.h"
#include "gameobject.h"
#include "script_game_object.h"
#include "infoportion.h"
#include "inventoryowner.h"
#include "ai_debug.h"
#include "ui/xrUIXmlParser.h"
#include "actor.h"

CPhraseScript::CPhraseScript() = default;
CPhraseScript::~CPhraseScript() = default;

// загрузка из XML файла
void CPhraseScript::Load(CUIXml* uiXml, XML_NODE* phrase_node)
{
    m_sScriptTextFunc._set(uiXml->Read(phrase_node, "script_text", 0, ""));

    LoadSequence(uiXml, phrase_node, "precondition", m_Preconditions);
    LoadSequence(uiXml, phrase_node, "action", m_ScriptActions);

    LoadSequence(uiXml, phrase_node, "has_info", m_HasInfo);
    LoadSequence(uiXml, phrase_node, "dont_has_info", m_DontHasInfo);

    LoadSequence(uiXml, phrase_node, "give_info", m_GiveInfo);
    LoadSequence(uiXml, phrase_node, "disable_info", m_DisableInfo);
}

template <typename T>
void CPhraseScript::LoadSequence(CUIXml* uiXml, XML_NODE* phrase_node, LPCSTR tag, T& str_vector)
{
    const gsl::index tag_num = uiXml->GetNodesNum(phrase_node, tag);

    str_vector.clear();
    str_vector.reserve(tag_num);

    for (gsl::index i{}; i < tag_num; ++i)
        str_vector.emplace_back(uiXml->Read(phrase_node, tag, i, nullptr));
}

bool CPhraseScript::CheckInfo(const CInventoryOwner* pOwner) const
{
    THROW(pOwner);

    for (auto& info : m_HasInfo)
    {
        if (!Actor()->HasInfo(info))
        {
#ifdef DEBUG
            if (psAI_Flags.test(aiDialogs))
                Msg("----rejected: [%s] has info %s", pOwner->Name(), info.c_str());
#endif

            return false;
        }
    }

    for (auto& info : m_DontHasInfo)
    {
        if (Actor()->HasInfo(info))
        {
#ifdef DEBUG
            if (psAI_Flags.test(aiDialogs))
                Msg("----rejected: [%s] dont has info %s", pOwner->Name(), info.c_str());
#endif

            return false;
        }
    }

    return true;
}

void CPhraseScript::TransferInfo(const CInventoryOwner* pOwner) const
{
    THROW(pOwner);

    for (auto& info : m_GiveInfo)
        Actor()->TransferInfo(info, true);

    for (auto& info : m_DisableInfo)
        Actor()->TransferInfo(info, false);
}

bool CPhraseScript::Precondition(const CGameObject* pSpeakerGO, [[maybe_unused]] LPCSTR dialog_id, [[maybe_unused]] LPCSTR phrase_id) const
{
    bool predicate_result = true;

    if (!CheckInfo(smart_cast<const CInventoryOwner*>(pSpeakerGO)))
    {
#ifdef DEBUG
        if (psAI_Flags.test(aiDialogs))
            Msg("dialog [%s] phrase[%s] rejected by CheckInfo", dialog_id, phrase_id);
#endif

        return false;
    }

    for (const auto& Cond : Preconditions())
    {
        if (sol::function lua_function; ai().script_engine().function(Cond.c_str(), lua_function))
        {
            // Обычный функтор
            predicate_result = lua_function(pSpeakerGO->lua_game_object());
        }
        else
        {
            // Функтор с аргументами
            sol::function loadstring_function;
            std::ignore = ai().script_engine().function("loadstring", loadstring_function);

            xr_string ConditionString{Cond.c_str()}; //-V808
            ConditionString = "return " + ConditionString;

            sol::function ret_func = loadstring_function(ConditionString.c_str()); // Создаём функцию из строки через loadstring
            ASSERT_FMT_DBG(ret_func, "Loadstring returns nil for code: %s", ConditionString.c_str()); // Если это не функция, значит loadstring вернул nil и что-то пошло не так

            // Вызываем созданную функцию и передаём ей дефолтные аргументы. Они прилетят после аргументов, прописанных явно, если например сделать так:
            // <precondition>my_script.test_func(123, true, nil, ...)</precondition>
            // А если не указать '...' - дефолтные аргументы не будут переданы в функцию.
            if (ret_func)
                predicate_result = ret_func(pSpeakerGO->lua_game_object());
        }

        if (!predicate_result)
        {
#ifdef DEBUG
            if (psAI_Flags.test(aiDialogs))
                Msg("dialog [%s] phrase[%s] rejected by script predicate", dialog_id, phrase_id);
#endif

            break;
        }
    }

    return predicate_result;
}

void CPhraseScript::Action(const CGameObject* pSpeakerGO, LPCSTR dialog_id, LPCSTR) const
{
    for (const auto& Act : Actions())
    {
        if (sol::function lua_function; ai().script_engine().function(Act.c_str(), lua_function))
        {
            // Обычный функтор
            lua_function(pSpeakerGO->lua_game_object(), dialog_id);
        }
        else
        {
            // Функтор с аргументами
            sol::function loadstring_function;
            std::ignore = ai().script_engine().function("loadstring", loadstring_function);

            xr_string ActionString{Act.c_str()}; //-V808
            ActionString = "return " + ActionString;

            sol::function ret_func = loadstring_function(ActionString.c_str()); // Создаём функцию из строки через loadstring
            ASSERT_FMT_DBG(ret_func, "Loadstring returns nil for code: %s", ActionString.c_str()); // Если это не функция, значит loadstring вернул nil и что-то пошло не так

            // Вызываем созданную функцию и передаём ей дефолтные аргументы. Они прилетят после аргументов, прописанных явно, если например сделать так:
            // <action>my_script.test_func(123, true, nil, ...)</action>
            // А если не указать '...' - дефолтные аргументы не будут переданы в функцию.
            if (ret_func)
                ret_func(pSpeakerGO->lua_game_object(), dialog_id);
        }
    }

    TransferInfo(smart_cast<const CInventoryOwner*>(pSpeakerGO));
}

bool CPhraseScript::Precondition(const CGameObject* pSpeakerGO1, const CGameObject* pSpeakerGO2, LPCSTR dialog_id, LPCSTR phrase_id, LPCSTR next_phrase_id) const
{
    bool predicate_result = true;

    if (!CheckInfo(smart_cast<const CInventoryOwner*>(pSpeakerGO1)))
    {
#ifdef DEBUG
        if (psAI_Flags.test(aiDialogs))
            Msg("dialog [%s] phrase[%s] rejected by CheckInfo", dialog_id, phrase_id);
#endif

        return false;
    }

    for (const auto& Cond : Preconditions())
    {
        if (sol::function lua_function; ai().script_engine().function(Cond.c_str(), lua_function))
        {
            // Обычный функтор
            predicate_result = lua_function(pSpeakerGO1->lua_game_object(), pSpeakerGO2->lua_game_object(), dialog_id, phrase_id, next_phrase_id);
        }
        else
        {
            // Функтор с аргументами
            sol::function loadstring_function;
            std::ignore = ai().script_engine().function("loadstring", loadstring_function);

            xr_string ConditionString{Cond.c_str()}; //-V808
            ConditionString = "return " + ConditionString;

            sol::function ret_func = loadstring_function(ConditionString.c_str()); // Создаём функцию из строки через loadstring
            ASSERT_FMT_DBG(ret_func, "Loadstring returns nil for code: %s", ConditionString.c_str()); // Если это не функция, значит loadstring вернул nil и что-то пошло не так

            // Вызываем созданную функцию и передаём ей дефолтные аргументы. Они прилетят после аргументов, прописанных явно, если например сделать так:
            // <precondition>my_script.test_func(123, true, nil, ...)</precondition>
            // А если не указать '...' - дефолтные аргументы не будут переданы в функцию.
            if (ret_func)
                predicate_result = ret_func(pSpeakerGO1->lua_game_object(), pSpeakerGO2->lua_game_object(), dialog_id, phrase_id, next_phrase_id);
        }

        if (!predicate_result)
        {
#ifdef DEBUG
            if (psAI_Flags.test(aiDialogs))
                Msg("dialog [%s] phrase[%s] rejected by script predicate", dialog_id, phrase_id);
#endif

            break;
        }
    }

    return predicate_result;
}

void CPhraseScript::Action(const CGameObject* pSpeakerGO1, const CGameObject* pSpeakerGO2, LPCSTR dialog_id, LPCSTR phrase_id) const
{
    TransferInfo(smart_cast<const CInventoryOwner*>(pSpeakerGO1));

    for (const auto& Act : Actions())
    {
        if (sol::function lua_function; ai().script_engine().function(Act.c_str(), lua_function))
        {
            // Обычный функтор
            lua_function(pSpeakerGO1->lua_game_object(), pSpeakerGO2->lua_game_object(), dialog_id, phrase_id);
        }
        else
        {
            // Функтор с аргументами
            sol::function loadstring_function;
            std::ignore = ai().script_engine().function("loadstring", loadstring_function);

            xr_string ActionString{Act.c_str()}; //-V808
            ActionString = "return " + ActionString;

            sol::function ret_func = loadstring_function(ActionString.c_str()); // Создаём функцию из строки через loadstring
            ASSERT_FMT_DBG(ret_func, "Loadstring returns nil for code: %s", ActionString.c_str()); // Если это не функция, значит loadstring вернул nil и что-то пошло не так

            // Вызываем созданную функцию и передаём ей дефолтные аргументы. Они прилетят после аргументов, прописанных явно, если например сделать так:
            // <action>my_script.test_func(123, true, nil, ...)</action>
            // А если не указать '...' - дефолтные аргументы не будут переданы в функцию.
            if (ret_func)
                ret_func(pSpeakerGO1->lua_game_object(), pSpeakerGO2->lua_game_object(), dialog_id, phrase_id);
        }
    }
}

LPCSTR CPhraseScript::GetScriptText(LPCSTR str_to_translate, const CGameObject* pSpeakerGO1, const CGameObject* pSpeakerGO2, LPCSTR dialog_id, LPCSTR phrase_id)
{
    if (m_sScriptTextFunc.empty())
        return str_to_translate;

    if (sol::function lua_function; ai().script_engine().function(m_sScriptTextFunc.c_str(), lua_function))
    {
        // Обычный функтор
        xr_string ret = lua_function(pSpeakerGO1->lua_game_object(), pSpeakerGO2->lua_game_object(), dialog_id, phrase_id);
        return shared_str{ret.c_str()}.c_str();
    }
    else
    {
        // Функтор с аргументами
        sol::function loadstring_function;
        std::ignore = ai().script_engine().function("loadstring", loadstring_function);

        xr_string ScriptTextString{m_sScriptTextFunc.c_str()}; //-V808
        ScriptTextString = "return " + ScriptTextString;

        sol::function ret_func = loadstring_function(ScriptTextString.c_str()); // Создаём функцию из строки через loadstring
        ASSERT_FMT_DBG(ret_func, "Loadstring returns nil for code: %s", ScriptTextString.c_str()); // Если это не функция, значит loadstring вернул nil и что-то пошло не так

        // Вызываем созданную функцию и передаём ей дефолтные аргументы. Они прилетят после аргументов, прописанных явно, если например сделать так:
        // <script_text>my_script.test_func(123, true, nil, ...)</script_text>
        // А если не указать '...' - дефолтные аргументы не будут переданы в функцию.
        if (ret_func)
        {
            xr_string ret = ret_func(pSpeakerGO1->lua_game_object(), pSpeakerGO2->lua_game_object(), dialog_id, phrase_id);
            return shared_str{ret.c_str()}.c_str();
        }
    }

    return "";
}
