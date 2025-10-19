////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_simulator_script.cpp
//	Created 	: 25.12.2002
//  Modified 	: 13.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "alife_simulator.h"
#include "ai_space.h"
#include "alife_object_registry.h"
#include "alife_story_registry.h"
#include "script_engine.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "restriction_space.h"
#include "alife_graph_registry.h"
#include "alife_spawn_registry.h"
#include "alife_registry_container.h"
#include "xrServer.h"
#include "level.h"

namespace detail
{
bool object_exists_in_alife_registry(u32 id)
{
    if (ai().get_alife())
    {
        return ai().alife().objects().object((ALife::_OBJECT_ID)id, true) != nullptr;
    }
    return false;
}
} // namespace detail

STORY_PAIRS story_ids;
SPAWN_STORY_PAIRS spawn_story_ids;

static CALifeSimulator* alife() { return (const_cast<CALifeSimulator*>(ai().get_alife())); }

static CSE_ALifeDynamicObject* alife_object(const CALifeSimulator* self, ALife::_OBJECT_ID object_id)
{
    VERIFY(self);
    return (self->objects().object(object_id, true));
}

static bool valid_object_id(const CALifeSimulator* self, ALife::_OBJECT_ID object_id)
{
    VERIFY(self);
    return (object_id != 0xffff);
}

static CSE_ALifeDynamicObject* alife_object(const CALifeSimulator* self, LPCSTR name)
{
    VERIFY(self);
    return self->objects().object_by_name(name);
}

static CSE_ALifeDynamicObject* alife_object(const CALifeSimulator* self, ALife::_OBJECT_ID id, bool no_assert)
{
    VERIFY(self);
    return (self->objects().object(id, no_assert));
}

static const CALifeObjectRegistry::OBJECT_REGISTRY& alife_objects(const CALifeSimulator* self)
{
    VERIFY(self);
    return self->objects().objects();
}

static CSE_ALifeDynamicObject* alife_story_object(const CALifeSimulator* self, ALife::_STORY_ID id) { return (self->story_objects().object(id, true)); }

template <typename _id_type>
static void generate_story_ids(STORY_PAIRS& result, _id_type INVALID_ID, LPCSTR section_name, LPCSTR INVALID_ID_STRING, LPCSTR invalid_id_description,
                               LPCSTR invalid_id_redefinition, LPCSTR duplicated_id_description)
{
    result.clear();

    CInifile* Ini = pGameIni;

    LPCSTR N, V;
    u32 k;
    shared_str temp;
    LPCSTR section = section_name;
    R_ASSERT(Ini->section_exist(section));

    for (k = 0; Ini->r_line(section, k, &N, &V); ++k)
    {
        temp = Ini->r_string_wb(section, N);

        R_ASSERT3(!strchr(*temp, ' '), invalid_id_description, *temp);
        R_ASSERT2(xr_strcmp(*temp, INVALID_ID_STRING), invalid_id_redefinition);

        auto ret = result.try_emplace(*temp, atoi(N));
        ASSERT_FMT(ret.second == true, "%s %s!", duplicated_id_description, *temp);
    }

    result.try_emplace(INVALID_ID_STRING, INVALID_ID);
}

static void kill_entity0(CALifeSimulator* alife, CSE_ALifeMonsterAbstract* monster, const GameGraph::_GRAPH_ID& game_vertex_id) { alife->kill_entity(monster, game_vertex_id); }

static void kill_entity1(CALifeSimulator* alife, CSE_ALifeMonsterAbstract* monster) { alife->kill_entity(monster, monster->m_tGraphID); }

static void add_in_restriction(CALifeSimulator* alife, CSE_ALifeMonsterAbstract* monster, ALife::_OBJECT_ID id)
{
    alife->add_restriction(monster->ID, id, RestrictionSpace::eRestrictorTypeIn);
}

static void add_out_restriction(CALifeSimulator* alife, CSE_ALifeMonsterAbstract* monster, ALife::_OBJECT_ID id)
{
    alife->add_restriction(monster->ID, id, RestrictionSpace::eRestrictorTypeOut);
}

static void remove_in_restriction(CALifeSimulator* alife, CSE_ALifeMonsterAbstract* monster, ALife::_OBJECT_ID id)
{
    alife->remove_restriction(monster->ID, id, RestrictionSpace::eRestrictorTypeIn);
}

static void remove_in_restrictions(CALifeSimulator* alife, CSE_ALifeMonsterAbstract* monster) { alife->remove_all_restrictions(monster->ID, RestrictionSpace::eRestrictorTypeIn); }

static void remove_out_restriction(CALifeSimulator* alife, CSE_ALifeMonsterAbstract* monster, ALife::_OBJECT_ID id)
{
    alife->remove_restriction(monster->ID, id, RestrictionSpace::eRestrictorTypeOut);
}

static void remove_out_restrictions(CALifeSimulator* alife, CSE_ALifeMonsterAbstract* monster)
{
    alife->remove_all_restrictions(monster->ID, RestrictionSpace::eRestrictorTypeOut);
}

static u32 get_level_id(CALifeSimulator* self) { return (self->graph().level().level_id()); }

static CSE_ALifeDynamicObject* CALifeSimulator__create(CALifeSimulator* self, ALife::_SPAWN_ID spawn_id)
{
    const CALifeSpawnRegistry::SPAWN_GRAPH::CVertex* vertex = ai().alife().spawns().spawns().vertex(spawn_id);
    THROW2(vertex, "Invalid spawn id!");

    CSE_ALifeDynamicObject* spawn = smart_cast<CSE_ALifeDynamicObject*>(&vertex->data()->object());
    THROW(spawn);

    CSE_ALifeDynamicObject* object;
    self->create(object, spawn, spawn_id);

    return (object);
}

static CSE_Abstract* CALifeSimulator__spawn_item(CALifeSimulator* self, LPCSTR section, const Fvector& position, u32 level_vertex_id, GameGraph::_GRAPH_ID game_vertex_id)
{
    THROW(self);
    return (self->spawn_item(section, position, level_vertex_id, game_vertex_id, ALife::_OBJECT_ID(-1)));
}

static CSE_Abstract* CALifeSimulator__spawn_item2(CALifeSimulator* self, LPCSTR section, const Fvector& position, u32 level_vertex_id, GameGraph::_GRAPH_ID game_vertex_id,
                                                  ALife::_OBJECT_ID id_parent)
{
    if (id_parent == ALife::_OBJECT_ID(-1))
        return (self->spawn_item(section, position, level_vertex_id, game_vertex_id, id_parent));

    CSE_ALifeDynamicObject* object = ai().alife().objects().object(id_parent, true);
    if (!object)
    {
        Msg("! invalid parent id [%d] specified", id_parent);
        return nullptr;
    }

    if (!object->m_bOnline)
        return (self->spawn_item(section, position, level_vertex_id, game_vertex_id, id_parent));

    NET_Packet packet;
    packet.w_begin(M_SPAWN);
    packet.w_stringZ(section);

    CSE_Abstract* item = self->spawn_item(section, position, level_vertex_id, game_vertex_id, id_parent, false);
    item->Spawn_Write(packet, FALSE);
    self->server().FreeID(item->ID, 0);
    F_entity_Destroy(item);

    ClientID clientID;
    clientID.set(0xffff);

    u16 dummy;
    packet.r_begin(dummy);
    VERIFY(dummy == M_SPAWN);
    return (self->server().Process_spawn(packet, clientID));
}

static CSE_Abstract* CALifeSimulator__spawn_ammo(CALifeSimulator* self, LPCSTR section, const Fvector& position, u32 level_vertex_id, GameGraph::_GRAPH_ID game_vertex_id,
                                                 ALife::_OBJECT_ID id_parent, int ammo_to_spawn)
{
    CSE_ALifeDynamicObject* object{};

    if (id_parent != ALife::_OBJECT_ID(-1))
    {
        object = ai().alife().objects().object(id_parent, true);
        if (!object)
        {
            Msg("! invalid parent id [%d] specified", id_parent);
            return nullptr;
        }
    }

    if (!object || !object->m_bOnline)
    {
        CSE_Abstract* item = self->spawn_item(section, position, level_vertex_id, game_vertex_id, id_parent);

        CSE_ALifeItemAmmo* ammo = smart_cast<CSE_ALifeItemAmmo*>(item);
        THROW(ammo);
        THROW(ammo->m_boxSize >= ammo_to_spawn);
        ammo->a_elapsed = (u16)ammo_to_spawn;

        return (item);
    }

    NET_Packet packet;
    packet.w_begin(M_SPAWN);
    packet.w_stringZ(section);

    CSE_Abstract* item = self->spawn_item(section, position, level_vertex_id, game_vertex_id, id_parent, false);

    CSE_ALifeItemAmmo* ammo = smart_cast<CSE_ALifeItemAmmo*>(item);
    THROW(ammo);
    THROW(ammo->m_boxSize >= ammo_to_spawn);
    ammo->a_elapsed = (u16)ammo_to_spawn;

    item->Spawn_Write(packet, FALSE);
    self->server().FreeID(item->ID, 0);
    F_entity_Destroy(item);

    ClientID clientID;
    clientID.set(0xffff);

    u16 dummy;
    packet.r_begin(dummy);
    VERIFY(dummy == M_SPAWN);
    return (self->server().Process_spawn(packet, clientID));
}

static void CALifeSimulator__release(CALifeSimulator* self, CSE_Abstract* object, bool)
{
    VERIFY(self);
    //	self->release						(object,true);

    R_ASSERT(object);
    CSE_ALifeObject* alife_object = smart_cast<CSE_ALifeObject*>(object);
    THROW(alife_object);
    if (!alife_object->m_bOnline)
    {
        self->release(object, true);
        return;
    }

    // awful hack, for stohe only
    NET_Packet packet;
    packet.w_begin(M_EVENT);
    packet.w_u32(Level().timeServer());
    packet.w_u16(GE_DESTROY);
    packet.w_u16(object->ID);
    Level().Send(packet, net_flags(TRUE, TRUE));
}

static void CALifeSimulator__assign_story_id(CALifeSimulator* self, ALife::_OBJECT_ID _id, ALife::_STORY_ID _story_id)
{
    CSE_ALifeDynamicObject* obj = ai().alife().objects().object(_id);
    R_ASSERT(obj);

#ifdef DEBUG
    if (psAI_Flags.test(aiALife))
    {
        Msg("[LSS] Assigning story_id for object [%s][%s][%d][%x]", obj->name_replace(), obj->s_name.c_str(), obj->ID, obj);
    }
#endif

    if (!(ai().alife().story_objects().object(_story_id, true)))
    {
        obj->m_story_id = _story_id;
        self->add_story_object(obj, _story_id);
    }
    else
        Msg("assign_story_id: specified id is already using");
}

static void CALifeSimulator__use_ai_locations(CALifeSimulator*, CSE_Abstract* object, bool _use)
{
#ifdef DEBUG
    if (psAI_Flags.test(aiALife))
    {
        Msg("[LSS] Assigning use of ai locations for object [%s][%s][%d][%x]", object->name_replace(), *object->s_name, object->ID, smart_cast<void*>(object));
    }
#endif
    CSE_ALifeDynamicObject* obj = ai().alife().objects().object(object->ID);
    R_ASSERT(obj);

    obj->used_ai_locations(_use);
}

static void CALifeSimulator__teleport_object(CALifeSimulator* self, u16 ID, Fvector _pos, u32 _lvid, GameGraph::_GRAPH_ID _gvid) { self->teleport_object(ID, _gvid, _lvid, _pos); }

static void FAKE_CALifeSimulator__teleport_object(CALifeSimulator*, const char*, Fvector, u32, GameGraph::_GRAPH_ID, u16)
{
    FATAL("INCORRECT ARGUMENTS! Must be: alife():teleport_object(id, position, lvid, gvid)");
}

static LPCSTR get_level_name(const CALifeSimulator*, int level_id)
{
    LPCSTR result = *ai().game_graph().header().level((GameGraph::_LEVEL_ID)level_id).name();
    return (result);
}

static CSE_ALifeCreatureActor* get_actor(const CALifeSimulator* self)
{
    THROW(self);
    return (self->graph().actor());
}

static KNOWN_INFO_VECTOR* registry(const CALifeSimulator* self, const ALife::_OBJECT_ID& id)
{
    THROW(self);
    return (self->registry(info_portions).object(id, true));
}

class CFindByIDPred
{
public:
    CFindByIDPred(shared_str element_to_find) { element = element_to_find; }
    bool operator()(const INFO_DATA& data) const { return data.info_id == element; }

private:
    shared_str element;
};

static bool has_info(const CALifeSimulator* self, const ALife::_OBJECT_ID& id, LPCSTR info_id)
{
    const KNOWN_INFO_VECTOR* known_info = registry(self, id);
    if (!known_info)
        return (false);

    if (std::find_if(known_info->begin(), known_info->end(), CFindByIDPred(info_id)) == known_info->end())
        return (false);

    return (true);
}

static bool dont_has_info(const CALifeSimulator* self, const ALife::_OBJECT_ID& id, LPCSTR info_id)
{
    THROW(self);
    // absurdly, but only because of scriptwriters needs
    return (!has_info(self, id, info_id));
}

// void disable_info_portion						(const CALifeSimulator *self, const ALife::_OBJECT_ID &id)
//{
//	THROW								(self);
// }

// void give_info_portion							(const CALifeSimulator *self, const ALife::_OBJECT_ID &id)
//{
//	THROW								(self);
// }

static LPCSTR get_save_name(CALifeSimulator* sim)
{
    // alpet: обертка предотвращает вылет, при обращении к свойству на ранней стадии  инициализации
    LPCSTR result{};
    if (sim)
        result = sim->save_name(FALSE);

    return result ? result : "NULL";
}

static LPCSTR get_loaded_save(CALifeSimulator* sim)
{
    LPCSTR result{};
    if (sim)
        result = sim->save_name(TRUE);

    return result ? result : "NULL";
}

static bool sim_is_unloading(CALifeSimulator* sim) { return sim->is_unloading(); }

void CALifeSimulator::script_register(sol::state_view& lua)
{
    lua.new_usertype<CALifeSimulator>(
        "alife_simulator", sol::no_constructor, "valid_object_id", &valid_object_id, "level_id", sol::resolve<u32(CALifeSimulator*)>(&get_level_id), "level_name", &get_level_name,
        "objects", &alife_objects, "object",
        sol::overload(sol::resolve<CSE_ALifeDynamicObject*(const CALifeSimulator*, ALife::_OBJECT_ID)>(&alife_object),
                      sol::resolve<CSE_ALifeDynamicObject*(const CALifeSimulator*, LPCSTR)>(&alife_object),
                      sol::resolve<CSE_ALifeDynamicObject*(const CALifeSimulator*, ALife::_OBJECT_ID, bool)>(&alife_object)),
        "is_unloading", &sim_is_unloading, "story_object", sol::resolve<CSE_ALifeDynamicObject*(const CALifeSimulator*, ALife::_STORY_ID)>(alife_story_object), "set_switch_online",
        sol::resolve<void(ALife::_OBJECT_ID, bool)>(&CALifeSimulator::set_switch_online), "set_switch_offline",
        sol::resolve<void(ALife::_OBJECT_ID, bool)>(&CALifeSimulator::set_switch_offline), "set_interactive",
        sol::resolve<void(ALife::_OBJECT_ID, bool)>(&CALifeSimulator::set_interactive), "kill_entity", sol::overload(&CALifeSimulator::kill_entity, &kill_entity0, &kill_entity1),
        "add_in_restriction", &add_in_restriction, "add_out_restriction", &add_out_restriction, "remove_in_restriction", &remove_in_restriction, "remove_in_restrictions",
        &remove_in_restrictions, "remove_out_restriction", &remove_out_restriction, "remove_out_restrictions", &remove_out_restrictions, "remove_all_restrictions",
        &CALifeSimulator::remove_all_restrictions, "create", sol::overload(&CALifeSimulator__create, &CALifeSimulator__spawn_item2, &CALifeSimulator__spawn_item), "create_ammo",
        &CALifeSimulator__spawn_ammo, "release", &CALifeSimulator__release, "spawn_id",
        sol::overload([](CALifeSimulator* self, ALife::_SPAWN_STORY_ID spawn_story_id) { return self->spawns().spawn_id(spawn_story_id); },
                      [](CALifeSimulator* self, const char* obj_name) { return self->spawns().spawn_id(obj_name); }),
        "actor", &get_actor, "has_info", &has_info, "dont_has_info", &dont_has_info, "switch_distance", &CALifeSimulator::switch_distance, "set_switch_distance",
        &CALifeSimulator::set_switch_distance, "teleport_object", sol::overload(&FAKE_CALifeSimulator__teleport_object, &CALifeSimulator__teleport_object), "assign_story_id",
        &CALifeSimulator__assign_story_id, "use_ai_locations", &CALifeSimulator__use_ai_locations, "save_name", sol::property(&get_save_name), "loaded_save_name",
        sol::property(&get_loaded_save));

    lua.set_function("alife", &alife);

    sol::table target;

    if (story_ids.empty())
        generate_story_ids(story_ids, INVALID_STORY_ID, "story_ids", "INVALID_STORY_ID", "Invalid story id description (contains spaces)!", "INVALID_STORY_ID redifinition!",
                           "Duplicated story id description");

    target = lua.create_table(story_ids.size(), 0);

    for (const auto& pair : story_ids)
        target.set(*pair.first, pair.second);

    xr::sol_new_enum(lua, "story_ids", target);

    if (spawn_story_ids.empty())
        generate_story_ids(spawn_story_ids, INVALID_SPAWN_STORY_ID, "spawn_story_ids", "INVALID_SPAWN_STORY_ID", "Invalid spawn story id description (contains spaces)!",
                           "INVALID_SPAWN_STORY_ID redifinition!", "Duplicated spawn story id description");

    target = lua.create_table(spawn_story_ids.size(), 0);

    for (const auto& pair : spawn_story_ids)
        target.set(*pair.first, pair.second);

    xr::sol_new_enum(lua, "spawn_story_ids", target);
}

#if 0 // def DEBUG
struct dummy {
    int count;
    lua_State* state;
    int ref;
};

void CALifeSimulator::validate			()
{
	typedef CALifeSpawnRegistry::SPAWN_GRAPH::const_vertex_iterator	const_vertex_iterator;
	const_vertex_iterator		I = spawns().spawns().vertices().begin();
	const_vertex_iterator		E = spawns().spawns().vertices().end();
	for ( ; I != E; ++I) {
		luabind::wrap_base		*base = smart_cast<luabind::wrap_base*>(&(*I).second->data()->object());
		if (!base)
			continue;

		if (!base->m_self.m_impl)
			continue;

		dummy					*_dummy = (dummy*)((void*)base->m_self.m_impl);
		lua_State				**_state = &_dummy->state;
		VERIFY2					(
			base->m_self.state(),
			make_string(
				"0x%08x name[%s] name_replace[%s]",
				*(int*)&_state,
				(*I).second->data()->object().name(),
				(*I).second->data()->object().name_replace()
			)
		);
	}
}
#endif // DEBUG
