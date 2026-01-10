#include "stdafx.h"

#include "ai/monsters/ai_monster_squad_manager.h"
#include "ai_space.h"
#include "alife_simulator.h"
#include "object_factory.h"
#include "string_table.h"

#include "entity_alive.h"
#include "ui/UIInventoryUtilities.h"
#include "ui/UILine.h"
#include "ui/UIWpnParams.h"
#include "UI/UIXmlInit.h"

#include "InfoPortion.h"
#include "PhraseDialog.h"
#include "GamePersistent.h"
#include "GameTask.h"
#include "encyclopedia_article.h"
#include "map_location.h"

#include "character_info.h"
#include "specific_character.h"
#include "character_community.h"
#include "monster_community.h"
#include "character_rank.h"
#include "character_reputation.h"

#include "sound_collection_storage.h"
#include "relation_registry.h"

tmc::task<void> init_game_globals()
{
    CUIXmlInit::InitColorDefs();

    CInfoPortion::InitInternal();
    CEncyclopediaArticle::InitInternal();
    CPhraseDialog::InitInternal();
    CCharacterInfo::InitInternal();
    CSpecificCharacter::InitInternal();
    CHARACTER_COMMUNITY::InitInternal();
    CHARACTER_RANK::InitInternal();
    CHARACTER_REPUTATION::InitInternal();
    MONSTER_COMMUNITY::InitInternal();

    co_return;
}

void clean_game_globals()
{
    destroy_lua_wpn_params();
    // destroy ai space
    xr_delete(g_ai_space);
    // destroy object factory
    xr_delete(g_object_factory);
    // destroy monster squad global var
    xr_delete(g_monster_squad);

    story_ids.clear();
    spawn_story_ids.clear();

    CInfoPortion::DeleteSharedData();
    CInfoPortion::DeleteIdToIndexData();

    CEncyclopediaArticle::DeleteSharedData();
    CEncyclopediaArticle::DeleteIdToIndexData();

    CPhraseDialog::DeleteSharedData();
    CPhraseDialog::DeleteIdToIndexData();

    CCharacterInfo::DeleteSharedData();
    CCharacterInfo::DeleteIdToIndexData();

    CSpecificCharacter::DeleteSharedData();
    CSpecificCharacter::DeleteIdToIndexData();

    CHARACTER_COMMUNITY::DeleteIdToIndexData();
    CHARACTER_RANK::DeleteIdToIndexData();
    CHARACTER_REPUTATION::DeleteIdToIndexData();
    MONSTER_COMMUNITY::DeleteIdToIndexData();

    // static shader for blood
    CEntityAlive::UnloadBloodyWallmarks();
    CEntityAlive::UnloadFireParticles();
    // очищение памяти таблицы строк
    CStringTable::Destroy();
    // Очищение таблицы цветов
    CUIXmlInit::DeleteColorDefs();
    // Очищение таблицы идентификаторов рангов и отношений сталкеров
    InventoryUtilities::ClearCharacterInfoStrings();

    xr_delete(g_sound_collection_storage);

    RELATION_REGISTRY::clear_relation_registry();

    dump_list_wnd();
    dump_list_lines();
    dump_list_sublines();
    clean_wnd_rects();
    xr_delete(g_gameTaskXml);
    xr_delete(g_uiSpotXml);
    dump_list_xmls();
}
