////////////////////////////////////////////////////////////////////////////
//	Module 		: particle_params.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Particle parameters class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "particle_params.h"

void CParticleParams::script_register(sol::state_view& lua)
{
    lua.new_usertype<CParticleParams>("particle_params", sol::no_constructor, sol::call_constructor,
                                      sol::constructors<CParticleParams(), CParticleParams(const Fvector&), CParticleParams(const Fvector&, const Fvector&),
                                                        CParticleParams(const Fvector&, const Fvector&, const Fvector&)>());
}
