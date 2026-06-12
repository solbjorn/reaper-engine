#pragma once

#include "entity.h"

DEFINE_VECTOR(shared_str, STR_VECTOR, STR_VECTOR_IT);

class MONSTER_COMMUNITY;
class CEntityCondition;
class CWound;
class CCharacterPhysicsSupport;
class CMaterialManager;
class CVisualMemoryManager;
class CBlend;

class XR_NOVTABLE CEntityAlive : public CEntity
{
    RTTI_DECLARE_TYPEINFO(CEntityAlive, CEntity);

private:
    typedef CEntity inherited;
    u32 m_used_time;

public:
    [[nodiscard]] CEntityAlive* cast_entity_alive() override { return this; }

    bool m_bMobility;
    float m_fAccuracy;
    float m_fIntelligence;
    u32 m_use_timeout{5000};
    u8 m_squad_index{std::numeric_limits<u8>::max()};

private:
    bool m_is_agresive;
    bool m_is_start_attack;

public:
    // General
    CEntityAlive();
    ~CEntityAlive() override;

    // Core events
    [[nodiscard]] DLL_Pure* _construct() override;
    void Load(gsl::czstring section) override;
    void reinit() override;
    void reload(gsl::czstring section) override;

    // object serialization
    void save(NET_Packet& output_packet) override;
    void load(IReader& input_packet) override;

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    [[nodiscard]] BOOL net_SaveRelevant() override;

    tmc::task<void> shedule_Update(u32 dt) override;
    void create_anim_mov_ctrl(CBlend* b) override;
    void destroy_anim_mov_ctrl() override;

    void HitImpulse(float, Fvector&, Fvector&) override;
    void Hit(SHit* pHDS) override;
    tmc::task<void> Die(CObject* who) override;
    virtual void g_WeaponBones(s32& L, s32& R1, s32& R2) = 0;
    void set_lock_corpse(bool b_l_corpse);
    bool is_locked_corpse();

    virtual float g_Radiation() const;
    virtual float SetfRadiation(float value);

    float CalcCondition(float) override;

    // Visibility related
    [[nodiscard]] virtual f32 ffGetFov() const = 0;
    [[nodiscard]] virtual f32 ffGetRange() const = 0;

    [[nodiscard]] virtual bool human_being() const { return false; }

    [[nodiscard]] u16 PHGetSyncItemsNumber() override;
    [[nodiscard]] CPHSynchronize* PHGetSyncItem(u16 item) override;
    void PHUnFreeze() override;
    void PHFreeze() override;

    void PHGetLinearVell(Fvector& velocity) override;
    [[nodiscard]] CPHSoundPlayer* ph_sound_player() override;
    [[nodiscard]] CIKLimbsController* character_ik_controller() override;
    [[nodiscard]] ICollisionHitCallback* get_collision_hit_callback() override;
    void set_collision_hit_callback(ICollisionHitCallback* cc) override;

protected:
    DEFINE_VECTOR(CWound*, WOUND_VECTOR, WOUND_VECTOR_IT);
    WOUND_VECTOR m_ParticleWounds;

    virtual void StartFireParticles(CWound* pWound);
    virtual void UpdateFireParticles();
    virtual void LoadFireParticles(LPCSTR section);

public:
    static void UnloadFireParticles();

protected:
    static STR_VECTOR* m_pFireParticlesVector;
    static u32 m_dwMinBurnTime;
    static float m_fStartBurnWoundSize;
    static float m_fStopBurnWoundSize;

    virtual void BloodyWallmarks(float P, const Fvector& dir, s16 element, const Fvector& position_in_object_space);
    static void LoadBloodyWallmarks(LPCSTR section);

public:
    static void UnloadBloodyWallmarks();

    void ClearBloodWounds() { m_BloodWounds.clear(); }

protected:
    virtual void PlaceBloodWallmark(const Fvector& dir, const Fvector& start_pos, float trace_dist, float wallmark_size, IWallMarkArray* wallmarks_vector);

    // информация о кровавых отметках на стенах, общая для всех CEntityAlive
    static FactoryPtr<IWallMarkArray>* m_pBloodMarksVector;
    static float m_fBloodMarkSizeMax;
    static float m_fBloodMarkSizeMin;
    static float m_fBloodMarkDistance;
    static float m_fNominalHit;

    // текстурки капель крови
    static FactoryPtr<IWallMarkArray>* m_pBloodDropsVector;
    // список ран с которых капает кровь

    DEFINE_VECTOR(CWound*, WOUND_VECTOR, WOUND_VECTOR_IT);
    WOUND_VECTOR m_BloodWounds;
    // размер раны, чтоб начала капать кровь
    static float m_fStartBloodWoundSize;
    // размер раны, чтоб остановить кровь
    static float m_fStopBloodWoundSize;
    static float m_fBloodDropSize;

    // обновление ран, и рисование отметок от капающей крови
    virtual void StartBloodDrops(CWound* pWound);
    virtual void UpdateBloodDrops();

    // отношения между существами и персонажами в зоне
public:
    [[nodiscard]] virtual ALife::ERelationType tfGetRelationType(const CEntityAlive* tpEntityAlive) const;
    [[nodiscard]] virtual bool is_relation_enemy(const CEntityAlive* tpEntityAlive) const;

public:
    MONSTER_COMMUNITY* monster_community;

private:
    CEntityCondition* m_entity_condition;
    CMaterialManager* m_material_manager{};
    bool b_eating{};

protected:
    [[nodiscard]] CEntityConditionSimple* create_entity_condition(CEntityConditionSimple* ec) override;

public:
    IC CEntityCondition& conditions() const;

    IC CMaterialManager& material() const
    {
        VERIFY(m_material_manager);
        return (*m_material_manager);
    }

protected:
    u32 m_ef_creature_type;
    u32 m_ef_weapon_type{std::numeric_limits<u32>::max()};
    u32 m_ef_detector_type{std::numeric_limits<u32>::max()};

public:
    [[nodiscard]] u32 ef_creature_type() const override;
    [[nodiscard]] u32 ef_weapon_type() const override;
    [[nodiscard]] u32 ef_detector_type() const override;

public:
    [[nodiscard]] virtual CVisualMemoryManager* visual_memory() const { return nullptr; }
    void net_Relcase(CObject* O) override;

public:
    IC bool const& is_agresive() const;
    IC void is_agresive(bool const& value);
    IC bool const& is_start_attack() const;
    IC void is_start_attack(bool const& value);
};
XR_SOL_BASE_CLASSES(CEntityAlive);

#include "entity_alive_inline.h"
