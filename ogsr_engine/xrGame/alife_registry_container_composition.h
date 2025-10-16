////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_registry_container_composition.h
//	Created 	: 01.07.2004
//  Modified 	: 01.07.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife registry container class, composition of the registries
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "InfoPortionDefs.h"
#include "PdaMsg.h"
#include "encyclopedia_article_defs.h"

#include "character_info_defs.h"
#include "relation_registry_defs.h"

#include "gametaskdefs.h"
#include "game_news.h"
#include "map_location_defs.h"

#include "actor_statistic_defs.h"

template <typename _index_type, typename _data_type>
class CALifeAbstractRegistry;

// для всех персонажей, те порции информации, которые они помнят
using CInfoPortionRegistry = CALifeAbstractRegistry<u16, KNOWN_INFO_VECTOR>;
#define info_portions define_constant(CInfoPortionRegistry)
add_to_registry_type_list(CInfoPortionRegistry);
#undef registry_type_list
#define registry_type_list save_registry_type_list(CInfoPortionRegistry)

// для всех персонажей, отношения с другими персонажами
using CRelationRegistry = CALifeAbstractRegistry<u16, RELATION_DATA>;
add_to_registry_type_list(CRelationRegistry);
#undef registry_type_list
#define registry_type_list save_registry_type_list(CRelationRegistry)

// для актеров, список персонажей с которыми были разговоры
using CKnownContactsRegistry = CALifeAbstractRegistry<u16, TALK_CONTACT_VECTOR>;
add_to_registry_type_list(CKnownContactsRegistry);
#undef registry_type_list
#define registry_type_list save_registry_type_list(CKnownContactsRegistry)

// список статей энциклопедии, которые знает актер
using CEncyclopediaRegistry = CALifeAbstractRegistry<u16, ARTICLE_VECTOR>;
add_to_registry_type_list(CEncyclopediaRegistry);
#undef registry_type_list
#define registry_type_list save_registry_type_list(CEncyclopediaRegistry)

// список новостей полученных актером, состоит из новостей симуляции и сюжетных (скриптованых) новостей
using CGameNewsRegistry = CALifeAbstractRegistry<u16, GAME_NEWS_VECTOR>;
add_to_registry_type_list(CGameNewsRegistry);
#undef registry_type_list
#define registry_type_list save_registry_type_list(CGameNewsRegistry)

// список описаний персонажей, которые уже задействованы в игре
using CSpecificCharacterRegistry = CALifeAbstractRegistry<shared_str, int>;
#define specific_characters define_constant(CSpecificCharacterRegistry)
add_to_registry_type_list(CSpecificCharacterRegistry);
#undef registry_type_list
#define registry_type_list save_registry_type_list(CSpecificCharacterRegistry)

// map locations for actor
add_to_registry_type_list(CMapLocationRegistry);
#undef registry_type_list
#define registry_type_list save_registry_type_list(CMapLocationRegistry)

// game tasks for actor
add_to_registry_type_list(CGameTaskRegistry);
#undef registry_type_list
#define registry_type_list save_registry_type_list(CGameTaskRegistry)

// ActorStatistics
add_to_registry_type_list(CActorStatisticRegistry);
#undef registry_type_list
#define registry_type_list save_registry_type_list(CActorStatisticRegistry)
