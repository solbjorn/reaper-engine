#include "stdafx.h"
#include "alife_space.h"
#include "Car.h"
#include "CarWeapon.h"
#include "script_game_object.h"

static u8 CCar__IsLightsOn(CCar* self) { return self->IsLightsOn() ? 1 : 0; }
static u8 CCar__IsEngineOn(CCar* self) { return self->IsEngineOn() ? 1 : 0; }

void CCar::script_register(sol::state_view& lua)
{
    lua.new_usertype<CCar>(
        "CCar", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CCar>), "factory", &client_factory<CCar>,

        // wpn_action
        "eWpnDesiredDir", sol::var(CCarWeapon::eWpnDesiredDir), "eWpnDesiredPos", sol::var(CCarWeapon::eWpnDesiredPos), "eWpnActivate", sol::var(CCarWeapon::eWpnActivate),
        "eWpnFire", sol::var(CCarWeapon::eWpnFire), "eWpnAutoFire", sol::var(CCarWeapon::eWpnAutoFire), "eWpnToDefaultDir", sol::var(CCarWeapon::eWpnToDefaultDir),

        "Action", &CCar::Action, "SetParam", sol::resolve<void(int, Fvector)>(&CCar::SetParam), "CanHit", &CCar::WpnCanHit, "FireDirDiff", &CCar::FireDirDiff, "IsObjectVisible",
        &CCar::isObjectVisible, "HasWeapon", &CCar::HasWeapon, "CurrentVel", &CCar::CurrentVel, "GetfHealth", &CCar::GetfHealth, "SetfHealth", &CCar::SetfHealth, "SetExplodeTime",
        &CCar::SetExplodeTime, "ExplodeTime", &CCar::ExplodeTime, "CarExplode", &CCar::CarExplode,

        "GetFuelTank", &CCar::GetFuelTank, "GetFuel", &CCar::GetFuel, "SetFuel", &CCar::SetFuel, "IsLightsOn", &CCar__IsLightsOn, "IsEngineOn", &CCar__IsEngineOn, "SwitchEngine",
        &CCar::SwitchEngine, "SwitchLights", &CCar::SwitchLights, "transmission", sol::readonly(&CCar::m_current_transmission_num), sol::base_classes, xr_sol_bases<CCar>());
}
