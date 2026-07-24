#include "stdafx.h"

#include "GameMtlLib.h"

namespace
{
void DestroySounds(xr_vector<ref_sound>& lst)
{
    for (auto& it : lst)
        it.queue_destroy();
}

void CreateSounds(xr_vector<ref_sound>& lst, const char* buf)
{
    string128 tmp;
    const auto cnt = XR_ASSERT_VAL(_GetItemCount(buf) <= GAMEMTL_SUBITEM_COUNT + 2);
    lst.resize(cnt);

    for (int k = 0; k < cnt; ++k)
        lst[k].create(_GetItem(buf, k, tmp), st_Effect, sg_SourceType);
}

void CreateMarks(IWallMarkArray* pMarks, const char* buf)
{
    string256 tmp;
    const auto cnt = XR_ASSERT_VAL(_GetItemCount(buf) <= GAMEMTL_SUBITEM_COUNT);

    for (int k = 0; k < cnt; ++k)
        pMarks->AppendMark(_GetItem(buf, k, tmp));
}

void CreatePSs(xr_vector<shared_str>& lst, const char* buf)
{
    string256 tmp;
    const auto cnt = XR_ASSERT_VAL(_GetItemCount(buf) <= GAMEMTL_SUBITEM_COUNT);
    lst.reserve(cnt);

    for (int k = 0; k < cnt; ++k)
        lst.emplace_back(_GetItem(buf, k, tmp));
}
} // namespace

SGameMtlPair::~SGameMtlPair()
{
    // destroy all media
    DestroySounds(BreakingSounds);
    DestroySounds(StepSounds);
    DestroySounds(CollideSounds);
}

void SGameMtlPair::Load(IReader& fs)
{
    shared_str buf;

    XR_ASSERT(fs.find_chunk(GAMEMTLPAIR_CHUNK_PAIR) > 0);
    mtl0 = fs.r_u32();
    mtl1 = fs.r_u32();
    ID = fs.r_u32();
    ID_parent = fs.r_u32();
    OwnProps.assign(fs.r_u32());

    XR_ASSERT(fs.find_chunk(GAMEMTLPAIR_CHUNK_BREAKING) > 0);
    fs.r_stringZ(buf);
    CreateSounds(BreakingSounds, buf.c_str());

    XR_ASSERT(fs.find_chunk(GAMEMTLPAIR_CHUNK_STEP) > 0);
    fs.r_stringZ(buf);
    CreateSounds(StepSounds, buf.c_str());

    XR_ASSERT(fs.find_chunk(GAMEMTLPAIR_CHUNK_COLLIDE) > 0);
    fs.r_stringZ(buf);
    CreateSounds(CollideSounds, buf.c_str());

    fs.r_stringZ(buf);
    CreatePSs(CollideParticles, buf.c_str());

    fs.r_stringZ(buf);
    CreateMarks(&*CollideMarks, buf.c_str());
}
