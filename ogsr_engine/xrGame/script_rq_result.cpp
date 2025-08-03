#include "stdafx.h"
#include "script_game_object.h"
#include "GameObject.h"
#include "..\xrcdb\xr_collide_defs.h"
#include "..\xr_3da\GameMtlLib.h"
#include "..\Include/xrRender/Kinematics.h"
#include "Level.h"
#include "script_rq_result.h"

void script_rq_result::set_result(collide::rq_result _res)
{
    range = _res.range;
    element = _res.element;
    result = true;
    if (_res.O)
    {
        CGameObject* obj = smart_cast<CGameObject*>(_res.O);
        if (obj)
            object = obj->lua_game_object();

        const auto pK = smart_cast<IKinematics*>(_res.O->Visual());
        if (pK)
        {
            const auto& bone_data = pK->LL_GetData((u16)_res.element);
            if (bone_data.game_mtl_idx < GMLib.CountMaterial())
            {
                mtl = GMLib.GetMaterialByIdx(bone_data.game_mtl_idx);
            }
        }
    }
    else
    {
        CDB::TRI* T = Level().ObjectSpace.GetStaticTris() + element;
        if (T->material < GMLib.CountMaterial())
        {
            mtl = GMLib.GetMaterialByIdx(T->material);
        }
    }
}

static LPCSTR get_name(const SGameMtl* self) { return (*self->m_Name); }
static LPCSTR get_desc(const SGameMtl* self) { return (*self->m_Desc); }

void script_rq_result::script_register(sol::state_view& lua)
{
    lua.new_usertype<script_rq_result>("rq_result", sol::no_constructor, "range", sol::readonly(&script_rq_result::range), "object", sol::readonly(&script_rq_result::object),
                                       "element", sol::readonly(&script_rq_result::element), "result", sol::readonly(&script_rq_result::result), "mtl",
                                       sol::readonly(&script_rq_result::mtl));

    lua.new_enum("SGameMtlFlags", "flActorObstacle", SGameMtl::flActorObstacle, "flBloodmark", SGameMtl::flBloodmark, "flBounceable", SGameMtl::flBounceable, "flClimable",
                 SGameMtl::flClimable, "flDynamic", SGameMtl::flDynamic, "flInjurious", SGameMtl::flInjurious, "flLiquid", SGameMtl::flLiquid, "flPassable", SGameMtl::flPassable,
                 "flSlowDown", SGameMtl::flSlowDown, "flSuppressShadows", SGameMtl::flSuppressShadows, "flSuppressWallmarks", SGameMtl::flSuppressWallmarks);

    lua.new_usertype<SGameMtl>("SGameMtl", sol::no_constructor, "m_Name", sol::property(&get_name), "m_Desc", sol::property(&get_desc), "Flags", sol::readonly(&SGameMtl::Flags),
                               "fPHFriction", sol::readonly(&SGameMtl::fPHFriction), "fPHDamping", sol::readonly(&SGameMtl::fPHDamping), "fPHSpring",
                               sol::readonly(&SGameMtl::fPHSpring), "fPHBounceStartVelocity", sol::readonly(&SGameMtl::fPHBounceStartVelocity), "fPHBouncing",
                               sol::readonly(&SGameMtl::fPHBouncing), "fFlotationFactor", sol::readonly(&SGameMtl::fFlotationFactor), "fShootFactor",
                               sol::readonly(&SGameMtl::fShootFactor), "fBounceDamageFactor", sol::readonly(&SGameMtl::fBounceDamageFactor), "fInjuriousSpeed",
                               sol::readonly(&SGameMtl::fInjuriousSpeed), "fVisTransparencyFactor", sol::readonly(&SGameMtl::fVisTransparencyFactor), "fSndOcclusionFactor",
                               sol::readonly(&SGameMtl::fSndOcclusionFactor));
}
