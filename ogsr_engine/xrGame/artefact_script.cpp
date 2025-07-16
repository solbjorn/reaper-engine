#include "stdafx.h"
#include "artifact.h"
#include "MercuryBall.h"
#include "GraviArtifact.h"
#include "BlackDrops.h"
#include "Needles.h"
#include "BastArtifact.h"
#include "BlackGraviArtifact.h"
#include "DummyArtifact.h"
#include "ZudaArtifact.h"
#include "ThornArtifact.h"
#include "FadedBall.h"
#include "ElectricBall.h"
#include "RustyHairArtifact.h"
#include "GalantineArtifact.h"

void CArtefact::script_register(lua_State* L)
{
    auto lua = sol::state_view(L);

    lua.new_usertype<CArtefact>("CArtefact", sol::no_constructor, "can_spawn_zone", &CArtefact::m_bCanSpawnZone, "health_restore_speed", &CArtefact::m_fHealthRestoreSpeed,
                                "psy_health_restore_speed", &CArtefact::m_fPsyHealthRestoreSpeed, "radiation_restore_speed", &CArtefact::m_fRadiationRestoreSpeed,
                                "satiety_restore_speed", &CArtefact::m_fSatietyRestoreSpeed, "power_restore_speed", &CArtefact::m_fPowerRestoreSpeed, "bleeding_restore_speed",
                                &CArtefact::m_fBleedingRestoreSpeed, "thirst_restore_speed", &CArtefact::m_fThirstRestoreSpeed,

                                "additional_inventory_weight", &CArtefact::m_additional_weight, "additional_inventory_weight2", &CArtefact::m_additional_weight2, sol::base_classes,
                                xr_sol_bases<CArtefact>());

    lua.new_usertype<CMercuryBall>("CMercuryBall", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CMercuryBall>), sol::base_classes,
                                   xr_sol_bases<CMercuryBall>());
    lua.new_usertype<CBlackDrops>("CBlackDrops", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CBlackDrops>), sol::base_classes,
                                  xr_sol_bases<CBlackDrops>());
    lua.new_usertype<CBlackGraviArtefact>("CBlackGraviArtefact", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CBlackGraviArtefact>),
                                          sol::base_classes, xr_sol_bases<CBlackGraviArtefact>());
    lua.new_usertype<CBastArtefact>("CBastArtefact", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CBastArtefact>), sol::base_classes,
                                    xr_sol_bases<CBastArtefact>());
    lua.new_usertype<CDummyArtefact>("CDummyArtefact", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CDummyArtefact>), sol::base_classes,
                                     xr_sol_bases<CDummyArtefact>());
    lua.new_usertype<CZudaArtefact>("CZudaArtefact", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CZudaArtefact>), sol::base_classes,
                                    xr_sol_bases<CZudaArtefact>());
    lua.new_usertype<CThornArtefact>("CThornArtefact", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CThornArtefact>), sol::base_classes,
                                     xr_sol_bases<CThornArtefact>());
    lua.new_usertype<CFadedBall>("CFadedBall", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CFadedBall>), sol::base_classes,
                                 xr_sol_bases<CFadedBall>());
    lua.new_usertype<CElectricBall>("CElectricBall", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CElectricBall>), sol::base_classes,
                                    xr_sol_bases<CElectricBall>());
    lua.new_usertype<CRustyHairArtefact>("CRustyHairArtefact", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CRustyHairArtefact>), sol::base_classes,
                                         xr_sol_bases<CRustyHairArtefact>());
    lua.new_usertype<CGalantineArtefact>("CGalantineArtefact", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CGalantineArtefact>), sol::base_classes,
                                         xr_sol_bases<CGalantineArtefact>());
    lua.new_usertype<CGraviArtefact>("CGraviArtefact", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CGraviArtefact>), sol::base_classes,
                                     xr_sol_bases<CGraviArtefact>());
}
