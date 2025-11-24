#include "stdafx.h"

#include "GameTask.h"

#include "ui/xrUIXmlParser.h"
#include "encyclopedia_article.h"
#include "map_location.h"
#include "map_manager.h"

#include "level.h"
#include "actor.h"
#include "script_engine.h"
#include "script_game_object.h"
#include "ai_space.h"
#include "alife_object_registry.h"
#include "alife_simulator.h"
#include "alife_story_registry.h"
#include "game_object_space.h"
#include "object_broker.h"
#include "ui/uitexturemaster.h"
#include "alife_registry_wrappers.h"
#include "alife_simulator_header.h"

ALife::_STORY_ID story_id(LPCSTR story_id);
u16 storyId2GameId(ALife::_STORY_ID);

ALife::_STORY_ID story_id(LPCSTR story_id)
{
    auto I = story_ids.find(shared_str{story_id});
    ASSERT_FMT_DBG(I != story_ids.end(), "story_id not found: %s", story_id);
    return I != story_ids.end() ? ALife::_STORY_ID((*I).second) : INVALID_STORY_ID;
}

u16 storyId2GameId(ALife::_STORY_ID id)
{
    if (ai().get_alife())
    {
        CSE_ALifeDynamicObject* so = ai().alife().story_objects().object(id, true);
        return (so) ? so->ID : u16(-1);
    }
    else
    {
        u32 cnt = Level().Objects.o_count();
        for (u32 it = 0; it < cnt; ++it)
        {
            CObject* O = Level().Objects.o_get_by_iterator(it);
            CGameObject* GO = smart_cast<CGameObject*>(O);
            if (GO->story_id() == id)
                return GO->ID();
        }
        return u16(-1);
    }
}

CUIXml* g_gameTaskXml{};

CGameTask::CGameTask() : m_ID{nullptr}, m_version{0}, m_objectives_version{0}, m_show_all_objectives{false} {}
CGameTask::CGameTask(const TASK_ID& id) : m_priority{std::numeric_limits<u32>::max()} { Load(id); }

void CGameTask::Load(const TASK_ID& id)
{
    m_ID = id;

    if (!g_gameTaskXml)
    {
        g_gameTaskXml = xr_new<CUIXml>();
        g_gameTaskXml->Init(CONFIG_PATH, "gameplay", "game_tasks.xml");
    }
    XML_NODE* task_node = g_gameTaskXml->NavigateToNodeWithAttribute("game_task", "id", *id);

    THROW3(task_node, "game task id=", *id);
    g_gameTaskXml->SetLocalRoot(task_node);
    m_Title._set(g_gameTaskXml->Read(g_gameTaskXml->GetLocalRoot(), "title", 0, nullptr));
    m_priority = g_gameTaskXml->ReadAttribInt(g_gameTaskXml->GetLocalRoot(), "prio", -1);
    m_version = g_gameTaskXml->ReadAttribInt(g_gameTaskXml->GetLocalRoot(), "version", 0);
    m_objectives_version = g_gameTaskXml->ReadAttribInt(g_gameTaskXml->GetLocalRoot(), "objectives_version", 0);
    m_show_all_objectives =
        !!g_gameTaskXml->ReadAttribInt(g_gameTaskXml->GetLocalRoot(), "show_all_objectives", (Core.Features.test(xrCore::Feature::show_objectives_ondemand) ? 0 : 1));
#ifdef DEBUG
    if (m_priority == u32(-1))
    {
        Msg("Game Task [%s] has no priority", *id);
    }
#endif // DEBUG
    int tag_num = g_gameTaskXml->GetNodesNum(g_gameTaskXml->GetLocalRoot(), "objective");
    m_Objectives.clear();
    for (int i = 0; i < tag_num; i++)
    {
        XML_NODE* l_root{};
        l_root = g_gameTaskXml->NavigateToNode("objective", i);
        g_gameTaskXml->SetLocalRoot(l_root);

        m_Objectives.emplace_back(this, i);
        SGameTaskObjective& objective = m_Objectives.back();

        //.
        LPCSTR tag_text = g_gameTaskXml->Read(l_root, "text", 0, nullptr);
        objective.description._set(tag_text);
        //.
        tag_text = g_gameTaskXml->Read(l_root, "article", 0, nullptr);
        if (tag_text != nullptr)
            objective.article_id._set(tag_text);

        //.
        if (i == 0)
        {
            objective.icon_texture_name._set(g_gameTaskXml->Read(g_gameTaskXml->GetLocalRoot(), "icon", 0, nullptr));
            if (!objective.icon_texture_name.empty() && std::is_neq(xr::strcasecmp(objective.icon_texture_name, "ui\\ui_icons_task")))
            {
                objective.icon_rect = CUITextureMaster::GetTextureRect(*objective.icon_texture_name);
                objective.icon_rect.rb.sub(objective.icon_rect.rb, objective.icon_rect.lt);
                objective.icon_texture_name._set(CUITextureMaster::GetTextureFileName(*objective.icon_texture_name));
            }
            else if (!objective.icon_texture_name.empty())
            {
                objective.icon_rect.x1 = g_gameTaskXml->ReadAttribFlt(l_root, "icon", 0, "x");
                objective.icon_rect.y1 = g_gameTaskXml->ReadAttribFlt(l_root, "icon", 0, "y");
                objective.icon_rect.x2 = g_gameTaskXml->ReadAttribFlt(l_root, "icon", 0, "width");
                objective.icon_rect.y2 = g_gameTaskXml->ReadAttribFlt(l_root, "icon", 0, "height");
            }
        }
        //.
        objective.map_location._set(g_gameTaskXml->Read(l_root, "map_location_type", 0, nullptr));

        LPCSTR object_story_id = g_gameTaskXml->Read(l_root, "object_story_id", 0, nullptr);

        //*
        LPCSTR ddd;
        ddd = g_gameTaskXml->Read(l_root, "map_location_hidden", 0, nullptr);
        if ((ddd || !m_show_all_objectives) && i > 1) // первая точка в квесте не может быть скрытая
            objective.def_location_enabled = false;

        bool b1, b2;
        b1 = objective.map_location.empty();
        b2 = !object_story_id;
        VERIFY3(b1 == b2, "check [map_location_type] and [object_story_id] fields in objective definition for: ", *objective.description);

        //.
        objective.object_id = u16(-1);

        //.
        objective.map_hint._set(g_gameTaskXml->ReadAttrib(l_root, "map_location_type", 0, "hint", nullptr));

        if (object_story_id)
        {
            ALife::_STORY_ID _sid = story_id(object_story_id);
            objective.object_id = storyId2GameId(_sid);
        }

        //------infoportion_complete
        gsl::index info_num = g_gameTaskXml->GetNodesNum(l_root, "infoportion_complete");
        objective.m_completeInfos.reserve(info_num);

        for (gsl::index j{}; j < info_num; ++j)
            objective.m_completeInfos.emplace_back(g_gameTaskXml->Read(l_root, "infoportion_complete", j, nullptr));

        //------infoportion_fail
        info_num = g_gameTaskXml->GetNodesNum(l_root, "infoportion_fail");
        objective.m_failInfos.reserve(info_num);

        for (gsl::index j{}; j < info_num; ++j)
            objective.m_failInfos.emplace_back(g_gameTaskXml->Read(l_root, "infoportion_fail", j, nullptr));

        //------infoportion_set_complete
        info_num = g_gameTaskXml->GetNodesNum(l_root, "infoportion_set_complete");
        objective.m_infos_on_complete.reserve(info_num);

        for (gsl::index j{}; j < info_num; ++j)
            objective.m_infos_on_complete.emplace_back(g_gameTaskXml->Read(l_root, "infoportion_set_complete", j, nullptr));

        //------infoportion_set_fail
        info_num = g_gameTaskXml->GetNodesNum(l_root, "infoportion_set_fail");
        objective.m_infos_on_fail.reserve(info_num);

        for (gsl::index j{}; j < info_num; ++j)
            objective.m_infos_on_fail.emplace_back(g_gameTaskXml->Read(l_root, "infoportion_set_fail", j, nullptr));

        //------function_complete
        objective.m_complete_lua_functions.resize(g_gameTaskXml->GetNodesNum(l_root, "function_complete"));

        for (auto [j, fn] : xr::views_enumerate(objective.m_complete_lua_functions))
        {
            gsl::czstring str = g_gameTaskXml->Read(l_root, "function_complete", j, nullptr);
            const bool function_exists = ai().script_engine().function(str, fn);

            ASSERT_FMT_DBG(function_exists, "[%s]: Cannot find script function described in task objective: %s", __FUNCTION__, str);
        }

        //------function_fail
        objective.m_fail_lua_functions.resize(g_gameTaskXml->GetNodesNum(l_root, "function_fail"));

        for (auto [j, fn] : xr::views_enumerate(objective.m_fail_lua_functions))
        {
            gsl::czstring str = g_gameTaskXml->Read(l_root, "function_fail", j, nullptr);
            const bool function_exists = ai().script_engine().function(str, fn);

            ASSERT_FMT_DBG(function_exists, "[%s]: Cannot find script function described in task objective: %s", __FUNCTION__, str);
        }

        //------function_on_complete
        objective.m_lua_functions_on_complete.resize(g_gameTaskXml->GetNodesNum(l_root, "function_call_complete"));

        for (auto [j, fn] : xr::views_enumerate(objective.m_lua_functions_on_complete))
        {
            gsl::czstring str = g_gameTaskXml->Read(l_root, "function_call_complete", j, nullptr);
            const bool function_exists = ai().script_engine().function(str, fn);

            ASSERT_FMT_DBG(function_exists, "[%s]: Cannot find script function described in task objective: %s", __FUNCTION__, str);
        }

        //------function_on_fail
        objective.m_lua_functions_on_fail.resize(g_gameTaskXml->GetNodesNum(l_root, "function_call_fail"));

        for (auto [j, fn] : xr::views_enumerate(objective.m_lua_functions_on_fail))
        {
            gsl::czstring str = g_gameTaskXml->Read(l_root, "function_call_fail", j, nullptr);
            const bool function_exists = ai().script_engine().function(str, fn);

            ASSERT_FMT_DBG(function_exists, "[%s]: Cannot find script function described in task objective: %s", __FUNCTION__, str);
        }

        g_gameTaskXml->SetLocalRoot(task_node);
    }

    g_gameTaskXml->SetLocalRoot(g_gameTaskXml->GetRoot());
}

bool CGameTask::HasLinkedMapLocations() const
{
    for (auto& obj : m_Objectives)
    {
        if (obj.LinkedMapLocation() != nullptr)
            return true;
    }

    return false;
}

bool CGameTask::HasInProgressObjective() const
{
    for (auto& obj : m_Objectives | std::views::drop(1))
    {
        if (obj.TaskState() == eTaskStateInProgress)
            return true;
    }

    return false;
}

SGameTaskObjective::SGameTaskObjective() : parent{nullptr}, idx{0} {}
SGameTaskObjective::SGameTaskObjective(CGameTask* _parent, int _idx) : parent{_parent}, idx{_idx} {}

CMapLocation* SGameTaskObjective::LinkedMapLocation() const
{
    if (map_location.empty())
        return nullptr;

    return Level().MapManager().GetMapLocation(map_location, object_id);
}

void SGameTaskObjective::SetTaskState(ETaskState new_state)
{
    task_state = new_state;
    if ((new_state == eTaskStateFail) || (new_state == eTaskStateCompleted))
    {
        if (task_state == eTaskStateFail)
        {
            SendInfo(m_infos_on_fail);
            CallAllFuncs(m_lua_functions_on_fail);
        }
        else if (task_state == eTaskStateCompleted)
        {
            SendInfo(m_infos_on_complete);
            CallAllFuncs(m_lua_functions_on_complete);
        }
        // callback for scripters
        ChangeStateCallback();
    }
}

ETaskState SGameTaskObjective::UpdateState()
{
    if ((idx == 0) && (parent->m_ReceiveTime != parent->m_TimeToComplete))
    {
        if (Level().GetGameTime() > parent->m_TimeToComplete)
        {
            return eTaskStateFail;
        }
    }
    // check fail infos
    if (CheckInfo(m_failInfos))
        return eTaskStateFail;

    // check fail functor
    if (CheckFunctions(m_fail_lua_functions))
        return eTaskStateFail;

    // check complete infos
    if (CheckInfo(m_completeInfos))
        return eTaskStateCompleted;

    // check complete functor
    if (CheckFunctions(m_complete_lua_functions))
        return eTaskStateCompleted;

    return TaskState();
}

void SGameTaskObjective::SendInfo(xr_vector<shared_str>& v)
{
    for (const auto& info : v)
        Actor()->TransferInfo(info, true);
}

bool SGameTaskObjective::CheckInfo(xr_vector<shared_str>& v)
{
    for (const auto& info : v)
    {
        if (!Actor()->HasInfo(info))
            return false;
    }

    return true;
}

bool SGameTaskObjective::CheckFunctions(xr_vector<sol::function>& v)
{
    for (auto& fn : v)
    {
        if (!fn || !fn(parent->m_ID.c_str(), idx))
            return false;
    }

    return true;
}

void SGameTaskObjective::CallAllFuncs(xr_vector<sol::function>& v)
{
    for (auto& fn : v)
    {
        if (fn)
            fn(parent->m_ID.c_str(), idx);
    }
}

void SGameTaskObjective::SetDescription_script(LPCSTR _descr) { description._set(_descr); }
void SGameTaskObjective::SetArticleID_script(LPCSTR _id) { article_id._set(_id); }

void SGameTaskObjective::SetMapHint_script(LPCSTR _str) { map_hint._set(_str); }
void SGameTaskObjective::SetMapLocation_script(LPCSTR _str) { map_location._set(_str); }

void SGameTaskObjective::SetObjectID_script(u16 id) { object_id = id; }

void SGameTaskObjective::SetIconName_script(LPCSTR _str)
{
    icon_texture_name._set(_str);
    icon_rect = CUITextureMaster::GetTextureRect(icon_texture_name.c_str());
    icon_rect.rb.sub(icon_rect.rb, icon_rect.lt);
    icon_texture_name._set(CUITextureMaster::GetTextureFileName(icon_texture_name.c_str()));
}

void SGameTaskObjective::AddCompleteInfo_script(LPCSTR _str) { m_completeInfos.emplace_back(_str); }
void SGameTaskObjective::AddFailInfo_script(LPCSTR _str) { m_failInfos.emplace_back(_str); }
void SGameTaskObjective::AddOnCompleteInfo_script(LPCSTR _str) { m_infos_on_complete.emplace_back(_str); }
void SGameTaskObjective::AddOnFailInfo_script(LPCSTR _str) { m_infos_on_fail.emplace_back(_str); }

void SGameTaskObjective::AddCompleteFunc_script(LPCSTR _str) { m_pScriptHelper.m_s_complete_lua_functions.emplace_back(_str); }
void SGameTaskObjective::AddFailFunc_script(LPCSTR _str) { m_pScriptHelper.m_s_fail_lua_functions.emplace_back(_str); }
void SGameTaskObjective::AddOnCompleteFunc_script(LPCSTR _str) { m_pScriptHelper.m_s_lua_functions_on_complete.emplace_back(_str); }
void SGameTaskObjective::AddOnFailFunc_script(LPCSTR _str) { m_pScriptHelper.m_s_lua_functions_on_fail.emplace_back(_str); }

void CGameTask::Load_script(LPCSTR _id) { Load(shared_str{_id}); }
void CGameTask::SetTitle_script(LPCSTR _title) { m_Title._set(_title); }
void CGameTask::SetPriority_script(int _prio) { m_priority = _prio; }

void CGameTask::AddObjective_script(SGameTaskObjective* O)
{
    O->m_pScriptHelper.init_functions(O->m_pScriptHelper.m_s_complete_lua_functions, O->m_complete_lua_functions);
    O->m_pScriptHelper.init_functions(O->m_pScriptHelper.m_s_fail_lua_functions, O->m_fail_lua_functions);
    O->m_pScriptHelper.init_functions(O->m_pScriptHelper.m_s_lua_functions_on_complete, O->m_lua_functions_on_complete);
    O->m_pScriptHelper.init_functions(O->m_pScriptHelper.m_s_lua_functions_on_fail, O->m_lua_functions_on_fail);

    m_Objectives.push_back(*O);
}

void SGameTaskObjective::ChangeStateCallback() { Actor()->callback(GameObject::eTaskStateChange)(parent, this, task_state); }

void SGameTaskObjective::save(IWriter& stream) const
{
    save_data(idx, stream);
    save_data(task_state, stream);

    save_data(description, stream);
    save_data(map_location, stream);
    save_data(object_id, stream);
    save_data(task_state, stream);
    save_data(def_location_enabled, stream);
    save_data(map_hint, stream);
    save_data(icon_texture_name, stream);
    save_data(icon_rect, stream);
    save_data(article_id, stream);

    save_data(m_completeInfos, stream);
    save_data(m_failInfos, stream);
    save_data(m_infos_on_complete, stream);
    save_data(m_infos_on_fail, stream);

    bool b_script = m_pScriptHelper.not_empty();
    save_data(b_script, stream);
    if (b_script)
        save_data(m_pScriptHelper, stream);
}

void SGameTaskObjective::load(IReader& stream)
{
    load_data(idx, stream);
    load_data(task_state, stream);

    load_data(description, stream);
    load_data(map_location, stream);
    load_data(object_id, stream);
    load_data(task_state, stream);
    load_data(def_location_enabled, stream);
    load_data(map_hint, stream);
    load_data(icon_texture_name, stream);
    load_data(icon_rect, stream);
    load_data(article_id, stream);

    load_data(m_completeInfos, stream);
    load_data(m_failInfos, stream);
    load_data(m_infos_on_complete, stream);
    load_data(m_infos_on_fail, stream);

    bool b_script;
    load_data(b_script, stream); //-V614

    if (b_script)
    {
        load_data(m_pScriptHelper, stream);

        m_pScriptHelper.init_functions(m_pScriptHelper.m_s_complete_lua_functions, m_complete_lua_functions);
        m_pScriptHelper.init_functions(m_pScriptHelper.m_s_fail_lua_functions, m_fail_lua_functions);
        m_pScriptHelper.init_functions(m_pScriptHelper.m_s_lua_functions_on_complete, m_lua_functions_on_complete);
        m_pScriptHelper.init_functions(m_pScriptHelper.m_s_lua_functions_on_fail, m_lua_functions_on_fail);
    }
}

void SScriptObjectiveHelper::init_functions(xr_vector<shared_str>& v_src, xr_vector<sol::function>& v_dest)
{
    v_dest.resize(v_src.size());

    for (auto [str, fn] : std::views::zip(v_src, v_dest))
    {
        const bool function_exists = ai().script_engine().function(str.c_str(), fn);
        ASSERT_FMT_DBG(function_exists, "[%s]: Cannot find script function described in task objective: %s", __FUNCTION__, str.c_str());
    }
}

void SScriptObjectiveHelper::load(IReader& stream)
{
    load_data(m_s_complete_lua_functions, stream);
    load_data(m_s_fail_lua_functions, stream);
    load_data(m_s_lua_functions_on_complete, stream);
    load_data(m_s_lua_functions_on_fail, stream);
}

void SScriptObjectiveHelper::save(IWriter& stream) const
{
    save_data(m_s_complete_lua_functions, stream);
    save_data(m_s_fail_lua_functions, stream);
    save_data(m_s_lua_functions_on_complete, stream);
    save_data(m_s_lua_functions_on_fail, stream);
}

void SGameTaskKey::save(IWriter& stream) const
{
    save_data(task_id, stream);
    save_data(game_task->m_version, stream);
    save_data(game_task->m_objectives_version, stream);
    save_data(game_task->m_ReceiveTime, stream);
    save_data(game_task->m_FinishTime, stream);
    save_data(game_task->m_TimeToComplete, stream);

    save_data(game_task->m_Title, stream);

    u32 cnt = game_task->m_Objectives.size();
    save_data(cnt, stream);

    OBJECTIVE_VECTOR_IT it = game_task->m_Objectives.begin();
    OBJECTIVE_VECTOR_IT it_e = game_task->m_Objectives.end();
    for (; it != it_e; ++it)
        save_data(*it, stream);
}

void SGameTaskKey::load(IReader& stream)
{
    load_data(task_id, stream);
    game_task = xr_new<CGameTask>(task_id);
    u32 load_version = 0;
    if (ai().get_alife()->header().version() > 6)
    {
        load_data(load_version, stream);
        load_data(game_task->m_objectives_version, stream);
    }
    else
        game_task->m_objectives_version = 0;
    load_data(game_task->m_ReceiveTime, stream);
    load_data(game_task->m_FinishTime, stream);
    load_data(game_task->m_TimeToComplete, stream);

    load_data(game_task->m_Title, stream);

    u32 cnt;
    load_data(cnt, stream); //-V614

    if (cnt > game_task->m_Objectives.size())
        game_task->m_Objectives.resize(cnt);

    for (u32 i = 0; i < cnt; ++i)
    {
        load_data(game_task->m_Objectives[i], stream);
        game_task->m_Objectives[i].parent = game_task;
    }

    if (game_task->m_version > load_version)
        sync_task_version();
}

void SGameTaskKey::destroy() { delete_data(game_task); }

void SGameTaskKey::sync_task_version()
{
    CGameTask* new_game_task = xr_new<CGameTask>(game_task->m_ID);
    new_game_task->m_ReceiveTime = game_task->m_ReceiveTime;
    new_game_task->m_FinishTime = game_task->m_FinishTime;
    new_game_task->m_TimeToComplete = game_task->m_TimeToComplete;
    if (new_game_task->m_objectives_version == game_task->m_objectives_version)
    {
        for (auto& it : game_task->m_Objectives)
        {
            if (u32(it.idx) == new_game_task->m_Objectives.size())
                break;
            new_game_task->m_Objectives[it.idx].task_state = it.TaskState();
        }
    }
    delete_data(game_task);
    game_task = new_game_task;
}
