////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects space
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_export_space.h"

struct SRotation
{
    f32 yaw{}, pitch{}, roll{};

    constexpr SRotation() = default;
    constexpr explicit SRotation(f32 y, f32 p, f32 r) : yaw{y}, pitch{p}, roll{r} {}
};

enum EPOType
{
    epotBox,
    epotFixedChain,
    epotFreeChain,
    epotSkeleton
};

DEFINE_VECTOR(u32, DWORD_VECTOR, DWORD_IT);
DEFINE_VECTOR(bool, BOOL_VECTOR, BOOL_IT);
DEFINE_VECTOR(float, FLOAT_VECTOR, FLOAT_IT);
DEFINE_VECTOR(LPSTR, LPSTR_VECTOR, LPSTR_IT);
DEFINE_VECTOR(Fvector, FVECTOR_VECTOR, FVECTOR_IT);

#ifdef XRGAME_EXPORTS
#define DECLARE_ENTITY_DESTROY
#endif

#ifdef DECLARE_ENTITY_DESTROY
template <class T>
void F_entity_Destroy(T*& P)
{
    xr_delete(P);
}
#endif
