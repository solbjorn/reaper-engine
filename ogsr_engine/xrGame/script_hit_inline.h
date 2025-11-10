////////////////////////////////////////////////////////////////////////////
//	Module 		: script_hit_inline.h
//	Created 	: 06.02.2004
//  Modified 	: 24.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script hit class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC CScriptHit::CScriptHit() : m_caBoneName{""} {}

IC void CScriptHit::set_bone_name(LPCSTR bone_name) { m_caBoneName._set(bone_name); }
