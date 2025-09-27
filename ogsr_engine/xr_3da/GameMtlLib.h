#ifndef GameMtlLibH
#define GameMtlLibH

constexpr u32 GAMEMTLS_CHUNK_VERSION = 0x1000;
constexpr u32 GAMEMTLS_CHUNK_AUTOINC = 0x1001;
constexpr u32 GAMEMTLS_CHUNK_MTLS = 0x1002;
constexpr u32 GAMEMTLS_CHUNK_MTLS_PAIR = 0x1003;

constexpr u32 GAMEMTL_CHUNK_MAIN = 0x1000;
constexpr u32 GAMEMTL_CHUNK_FLAGS = 0x1001;
constexpr u32 GAMEMTL_CHUNK_PHYSICS = 0x1002;
constexpr u32 GAMEMTL_CHUNK_FACTORS = 0x1003;
constexpr u32 GAMEMTL_CHUNK_FLOTATION = 0x1004;
constexpr u32 GAMEMTL_CHUNK_DESC = 0x1005;
constexpr u32 GAMEMTL_CHUNK_INJURIOUS = 0x1006;
constexpr u32 GAMEMTL_CHUNK_DENSITY = 0x1007;
constexpr u32 GAMEMTL_CHUNK_FACTORS_MP = 0x1008;

constexpr u32 GAMEMTLPAIR_CHUNK_PAIR = 0x1000;
constexpr u32 GAMEMTLPAIR_CHUNK_BREAKING = 0x1002;
constexpr u32 GAMEMTLPAIR_CHUNK_STEP = 0x1003;
constexpr u32 GAMEMTLPAIR_CHUNK_COLLIDE = 0x1005;

constexpr int GAMEMTL_SUBITEM_COUNT = 4;

constexpr u32 GAMEMTL_NONE_ID = u32(-1);
constexpr u32 GAMEMTL_NONE_IDX = u16(-1);

constexpr const char* GAMEMTL_FILENAME = "gamemtl.xr";

constexpr u32 GAMEMTL_CURRENT_VERSION = 1;

#include "../Include/xrRender/WallMarkArray.h"
#include "../Include/xrRender/RenderFactory.h"

struct SGameMtl
{
    friend class CGameMtlLibrary;

protected:
    int ID; // auto number
public:
    enum : u32
    {
        flBreakable = (1ul << 0ul),
        flBounceable = (1ul << 2ul),
        flSkidmark = (1ul << 3ul),
        flBloodmark = (1ul << 4ul),
        flClimable = (1ul << 5ul),
        flPassable = (1ul << 7ul),
        flDynamic = (1ul << 8ul),
        flLiquid = (1ul << 9ul),
        flSuppressShadows = (1ul << 10ul),
        flSuppressWallmarks = (1ul << 11ul),
        flActorObstacle = (1ul << 12ul),
        flNoRicoshet = (1ul << 13ul),

        flInjurious = (1ul << 28ul), // flInjurious = fInjuriousSpeed > 0.f
        flShootable = (1ul << 29ul),
        flTransparent = (1ul << 30ul),
        flSlowDown = (1ul << 31ul) // flSlowDown = (fFlotationFactor<1.f)
    };

public:
    shared_str m_Name;
    shared_str m_Desc;

    Flags32 Flags;
    // physics part
    float fPHFriction; // ?
    float fPHDamping; // ?
    float fPHSpring; // ?
    float fPHBounceStartVelocity; // ?
    float fPHBouncing; // ?
    // shoot&bounce&visibility&flotation
    float fFlotationFactor; // 0.f - 1.f   	(1.f-полностью проходимый)
    float fShootFactor; // 0.f - 1.f	(1.f-полностью простреливаемый)
    float fShootFactorMP; // 0.f - 1.f	(1.f-полностью простреливаемый)
    float fBounceDamageFactor; // 0.f - 100.f
    float fInjuriousSpeed; // 0.f - ...	(0.f-не отбирает здоровье (скорость уменьшения здоровья))
    float fVisTransparencyFactor; // 0.f - 1.f	(1.f-полностью прозрачный)
    float fSndOcclusionFactor; // 0.f - 1.f    (1.f-полностью слышен)
    float fDensityFactor;

public:
    SGameMtl()
    {
        ID = -1;
        m_Name = "unknown";
        Flags.zero();
        // factors
        fFlotationFactor = 1.f;
        fShootFactor = 0.f;
        fShootFactorMP = 0.f;
        fBounceDamageFactor = 1.f;
        fInjuriousSpeed = 0.f;
        fVisTransparencyFactor = 0.f;
        fSndOcclusionFactor = 0.f;
        // physics
        fPHFriction = 1.f;
        fPHDamping = 1.f;
        fPHSpring = 1.f;
        fPHBounceStartVelocity = 0.f;
        fPHBouncing = 0.1f;
        fDensityFactor = 0.0f;
    }
    void Load(IReader& fs);
    void Save(IWriter& fs);
    int GetID() const { return ID; }
};

struct SGameMtlPair
{
    friend class CGameMtlLibrary;
    enum
    {
        flBreakingSounds = (1 << 1),
        flStepSounds = (1 << 2),
        flCollideSounds = (1 << 4),
        flCollideParticles = (1 << 5),
        flCollideMarks = (1 << 6)
    };
    CGameMtlLibrary* m_Owner;

private:
    int mtl0;
    int mtl1;

protected:
    int ID; // auto number
    int ID_parent;

public:
    Flags32 OwnProps;

    xr_vector<ref_sound> BreakingSounds;
    xr_vector<ref_sound> StepSounds;
    xr_vector<ref_sound> CollideSounds;
    xr_vector<shared_str> CollideParticles;
    FactoryPtr<IWallMarkArray> CollideMarks;

    SGameMtlPair(CGameMtlLibrary* owner)
    {
        mtl0 = -1;
        mtl1 = -1;
        ID = -1;
        ID_parent = -1;
        m_Owner = owner;
        OwnProps.one();
    }
    ~SGameMtlPair();

    int GetMtl0() const { return mtl0; }
    int GetMtl1() const { return mtl1; }
    int GetID() const { return ID; }
    void SetPair(int m0, int m1)
    {
        mtl0 = m0;
        mtl1 = m1;
    }
    bool IsPair(int m0, int m1) const { return (mtl0 == m0 && mtl1 == m1) || (mtl0 == m1 && mtl1 == m0); }
    int GetParent() const { return ID_parent; }
    void Save(IWriter& fs);
    void Load(IReader& fs);
#ifdef DEBUG
    const char* dbg_Name() const;
#endif
};

using GameMtlPairVec = xr_vector<SGameMtlPair*>;
using GameMtlPairIt = GameMtlPairVec::iterator;

class CGameMtlLibrary
{
private:
    int material_index;
    int material_pair_index;

    xr_vector<SGameMtl*> materials;
    GameMtlPairVec material_pairs;
    GameMtlPairVec material_pairs_rt;

public:
    CGameMtlLibrary();
    ~CGameMtlLibrary() {}
    void Unload()
    {
        material_pairs_rt.clear();
        for (auto& mtl : materials)
            xr_delete(mtl);
        materials.clear();
        for (auto& mtlPair : material_pairs)
            xr_delete(mtlPair);
        material_pairs.clear();
    }

    auto GetMaterialIt(const char* name)
    {
        auto pred = [&](const SGameMtl* mtl) { return !_stricmp(mtl->m_Name.c_str(), name); };
        return std::find_if(materials.begin(), materials.end(), pred);
    }
    auto GetMaterialIt(const shared_str& name)
    {
        auto pred = [&](const SGameMtl* mtl) { return mtl->m_Name.equal(name); };
        return std::find_if(materials.begin(), materials.end(), pred);
    }
    auto GetMaterialItByID(int id)
    {
        auto pred = [&](const SGameMtl* mtl) { return mtl->ID == id; };
        return std::find_if(materials.begin(), materials.end(), pred);
    }
    u32 GetMaterialID(const char* name)
    {
        const auto it = GetMaterialIt(name);
        return it == materials.end() ? GAMEMTL_NONE_ID : (*it)->ID;
    }
    SGameMtl* GetMaterialByID(s32 id) { return GetMaterialByIdx(GetMaterialIdx(id)); }
    u16 GetMaterialIdx(int ID)
    {
        const auto it = GetMaterialItByID(ID);
        if (materials.end() != it)
        {
            return (u16)std::distance(materials.begin(), it);
        }
        else
        {
            Msg("!![%s] material id [%d] not found!", __FUNCTION__, ID);
            return 0;
        }
    }
    u16 GetMaterialIdx(const char* name)
    {
        const auto it = GetMaterialIt(name);
        if (materials.end() != it)
        {
            return (u16)std::distance(materials.begin(), it);
        }
        else
        {
            Msg("!![%s] material name [%s] not found!", __FUNCTION__, name);
            return 0;
        }
    }
    SGameMtl* GetMaterialByIdx(u16 idx) const
    {
        if (idx < (u16)materials.size())
        {
            return materials[idx];
        }
        else
        {
            Msg("!![%s] material id [%u] not found!", __FUNCTION__, idx);
            return materials.front();
        }
    }

    auto FirstMaterial() { return materials.begin(); }
    auto LastMaterial() { return materials.end(); }
    u32 CountMaterial() const { return materials.size(); }

    // game
    SGameMtlPair* GetMaterialPair(u16 i0, u16 i1) const
    {
        const u32 mtlCount = materials.size();
        R_ASSERT(i0 < mtlCount && i1 < mtlCount);
        return material_pairs_rt[i1 * mtlCount + i0];
    }

    GameMtlPairIt FirstMaterialPair() { return material_pairs.begin(); }
    GameMtlPairIt LastMaterialPair() { return material_pairs.end(); }

    // IO routines
    void Load();
    bool Save();
    u64 get_hash();
};

#define GET_RANDOM(a_vector) (a_vector[Random.randI(a_vector.size())])

#define CLONE_MTL_SOUND(_res_, _mtl_pair_, _a_vector_) \
    { \
        VERIFY2(!(_mtl_pair_)->_a_vector_.empty(), (_mtl_pair_)->dbg_Name()); \
        _res_.clone(GET_RANDOM((_mtl_pair_)->_a_vector_), st_Effect, sg_SourceType); \
    } \
    XR_MACRO_END()

extern CGameMtlLibrary GMLib;

#endif
