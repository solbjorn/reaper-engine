#pragma once

class CPhysicsShellHolder;
struct dContact;
struct SGameMtl;

class XR_NOVTABLE CPHCollisionDamageReceiver : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CPHCollisionDamageReceiver);

public:
    typedef std::pair<u16, float> SControledBone;
    DEFINE_VECTOR(SControledBone, DAMAGE_CONTROLED_BONES_V, DAMAGE_BONES_I);

    struct SFind
    {
        u16 id;

        SFind(u16 _id) : id{_id} {}

        bool operator()(const SControledBone& cb) { return cb.first == id; }
    };

    DAMAGE_CONTROLED_BONES_V m_controled_bones;

    ~CPHCollisionDamageReceiver() override = 0;

protected:
    [[nodiscard]] virtual CPhysicsShellHolder* PPhysicsShellHolder() = 0;

    void Init();
    void Hit(u16 source_id, u16 bone_id, float power, const Fvector& dir, Fvector& pos);
    void Clear();

private:
    void BoneInsert(u16 id, float k);

    IC DAMAGE_BONES_I FindBone(u16 id) { return std::find_if(m_controled_bones.begin(), m_controled_bones.end(), SFind(id)); }
    static void CollisionCallback(bool&, bool bo1, dContact& c, SGameMtl* material_1, SGameMtl* material_2);
};

inline CPHCollisionDamageReceiver::~CPHCollisionDamageReceiver() = default;
