//---------------------------------------------------------------------------
#ifndef SkeletonMotionsH
#define SkeletonMotionsH

#include "bone.h"
#include "skeletonmotiondefs.h"

// refs
class IKinematicsAnimated;
class CBlend;
class IKinematics;

// callback
typedef void (*PlayCallback)(CBlend* P);

//*** Key frame definition ************************************************************************
enum
{
    flTKeyPresent = (1 << 0),
    flRKeyAbsent = (1 << 1),
    flTKey16IsBit = (1 << 2),
};
#pragma pack(push, 2)
struct CKey
{
    Fquaternion Q; // rotation
    Fvector T; // translation
};
struct CKeyQR
{
    s16 x, y, z, w; // rotation
};
struct CKeyQT8
{
    s8 x1, y1, z1;
};
struct CKeyQT16
{
    s16 x1, y1, z1;
};
/*
struct  CKeyQT
{
//	s8			x,y,z;
    s16			x1,y1,z1;
};
*/
#pragma pack(pop)

//*** Motion Data *********************************************************************************
class CMotion
{
    struct
    {
        u32 _flags : 8;
        u32 _count : 24;
    };

public:
    ref_smem<CKeyQR> _keysR;
    ref_smem<CKeyQT8> _keysT8;
    ref_smem<CKeyQT16> _keysT16;
    Fvector _initT;
    Fvector _sizeT;

public:
    constexpr void set_flags(u8 val) { _flags = val; }

    constexpr void set_flag(u8 mask, u8 val)
    {
        if (val)
            _flags |= mask;
        else
            _flags &= ~mask;
    }

    [[nodiscard]] constexpr BOOL test_flag(u8 mask) const { return BOOL(_flags & mask); }

    constexpr void set_count(u32 cnt)
    {
        VERIFY(cnt);
        _count = cnt;
    }

    [[nodiscard]] constexpr u32 get_count() const { return (u32(_count) & 0x00FFFFFF); }
    [[nodiscard]] constexpr float GetLength() const { return float(_count) * SAMPLE_SPF; }

    [[nodiscard]] constexpr gsl::index mem_usage() const
    {
        auto sz = gsl::index{sizeof(*this)};

        if (!_keysR.empty())
            sz += _keysR.size() * gsl::index{sizeof(CKeyQR)} / _keysR._get()->dwReference;
        if (!_keysT8.empty())
            sz += _keysT8.size() * gsl::index{sizeof(CKeyQT8)} / _keysT8._get()->dwReference;
        if (!_keysT16.empty())
            sz += _keysT16.size() * gsl::index{sizeof(CKeyQT16)} / _keysT16._get()->dwReference;

        return sz;
    }
};

class motion_marks
{
public:
    typedef std::pair<float, float> interval;

private:
    typedef xr_vector<interval> STORAGE;
    typedef STORAGE::iterator ITERATOR;
    typedef STORAGE::const_iterator C_ITERATOR;

    STORAGE intervals;

public:
    shared_str name;
    void Load(IReader*);

    [[nodiscard]] constexpr bool is_empty() const { return intervals.empty(); }
    [[nodiscard]] const interval* pick_mark(float const& t) const;
    [[nodiscard]] bool is_mark_between(float const& t0, float const& t1) const;
    [[nodiscard]] float time_to_next_mark(float time) const;
};

class CMotionDef
{
private:
    f32 speed{};
    f32 power{};
    f32 accrue{};
    f32 falloff{};

public:
    u16 bone_or_part;
    u16 motion;
    u16 flags;
    xr_vector<motion_marks> marks;

    void Load(IReader* MP, u32 fl, u16 vers);
    [[nodiscard]] constexpr gsl::index mem_usage() const { return gsl::index{sizeof(*this)}; }

    [[nodiscard]] constexpr float Accrue() const { return accrue; }
    [[nodiscard]] constexpr float Falloff() const { return falloff; }
    [[nodiscard]] constexpr float Speed() const { return speed; }
    [[nodiscard]] constexpr float Power() const { return power; }
    [[nodiscard]] bool StopAtEnd() const;
};

using accel_map = string_unordered_map<shared_str, u16>;

DEFINE_VECTOR(CMotionDef, MotionDefVec, MotionDefVecIt);

DEFINE_VECTOR(CMotion, MotionVec, MotionVecIt);
DEFINE_VECTOR(MotionVec*, BoneMotionsVec, BoneMotionsVecIt);

// partition
class CPartDef
{
public:
    shared_str Name;
    xr_vector<u32> bones;

    CPartDef() = default;

    [[nodiscard]] constexpr gsl::index mem_usage() const { return gsl::index{sizeof(*this)} + std::ssize(bones) * gsl::index{sizeof(u32)} + Name.size(); }
};

class CPartition
{
    CPartDef P[MAX_PARTS];

public:
    [[nodiscard]] constexpr CPartDef& operator[](u16 id) { return P[id]; }
    [[nodiscard]] constexpr const CPartDef& part(u16 id) const { return P[id]; }
    [[nodiscard]] u16 part_id(const shared_str& name) const;
    [[nodiscard]] constexpr gsl::index mem_usage() const { return P[0].mem_usage() * MAX_PARTS; }
    void load(IKinematics* V);

    [[nodiscard]] constexpr u8 count() const
    {
        u8 ret = 0;
        for (u8 i = 0; i < MAX_PARTS; ++i)
            if (P[i].Name.size())
                ret++;
        return ret;
    }
};

// shared motions
struct motions_value
{
    accel_map m_motion_map; // motion associations
    accel_map m_cycle; // motion data itself	(shared)
    accel_map m_fx; // motion data itself	(shared)
    CPartition m_partition; // partition
    std::atomic<gsl::index> m_dwReference;
    string_unordered_map<shared_str, MotionVec> m_motions;
    MotionDefVec m_mdefs;

    shared_str m_id;

    [[nodiscard]] BOOL load(LPCSTR N, IReader* data, vecBones* bones);
    [[nodiscard]] MotionVec* bone_motions(const shared_str& bone_name);

    [[nodiscard]] constexpr gsl::index mem_usage() const
    {
        gsl::index sz = gsl::index{sizeof(*this)} + std::ssize(m_motion_map) * 6 + m_partition.mem_usage();

        for (const auto& def : m_mdefs)
            sz += def.mem_usage();

        for (auto [name, vec] : m_motions)
        {
            for (const auto& mot : vec)
                sz += mot.mem_usage();
        }

        return sz;
    }
};

class motions_container
{
    string_unordered_map<shared_str, motions_value*> container;

public:
    motions_container();
    ~motions_container();

    [[nodiscard]] bool has(shared_str key) const;
    [[nodiscard]] motions_value* dock(shared_str key, IReader* data, vecBones* bones);
    void dump() const;
    void clean(bool force_destroy);
};

extern motions_container* g_pMotionsContainer;

class shared_motions
{
private:
    motions_value* p_{};

protected:
    // ref-counting
    constexpr void destroy()
    {
        if (p_ == nullptr)
            return;

        if (--p_->m_dwReference == 0)
            p_ = nullptr;
    }

public:
    [[nodiscard]] bool create(shared_str key, IReader* data, vecBones* bones);
    [[nodiscard]] bool create(const shared_motions& that);

    // construction
    constexpr shared_motions() = default;
    constexpr ~shared_motions() { destroy(); }

    shared_motions(const shared_motions& that) { std::ignore = create(that); }

    constexpr shared_motions(shared_motions&& that)
    {
        destroy();
        p_ = std::move(that.p_);
        that.p_ = nullptr;
    }

    // assignment & accessors
    shared_motions& operator=(const shared_motions& that)
    {
        std::ignore = create(that);
        return *this;
    }

    constexpr shared_motions& operator=(shared_motions&& that)
    {
        destroy();

        p_ = std::move(that.p_);
        that.p_ = nullptr;

        return *this;
    }

    [[nodiscard]] constexpr bool operator==(const shared_motions& rhs) const { return p_ == rhs.p_; }
    [[nodiscard]] constexpr explicit operator bool() const { return p_ != nullptr; }

    // misc func
    [[nodiscard]] MotionVec* bone_motions(const shared_str& bone_name)
    {
        VERIFY(p_);
        return p_->bone_motions(bone_name);
    }

    [[nodiscard]] accel_map* motion_map()
    {
        VERIFY(p_);
        return &p_->m_motion_map;
    }

    [[nodiscard]] accel_map* cycle()
    {
        VERIFY(p_);
        return &p_->m_cycle;
    }

    [[nodiscard]] accel_map* fx()
    {
        VERIFY(p_);
        return &p_->m_fx;
    }

    [[nodiscard]] CPartition* partition()
    {
        VERIFY(p_);
        return &p_->m_partition;
    }

    [[nodiscard]] CMotionDef* motion_def(u16 idx)
    {
        VERIFY(p_);
        return &p_->m_mdefs[idx];
    }

    [[nodiscard]] const shared_str& id() const
    {
        VERIFY(p_);
        return p_->m_id;
    }
};
//---------------------------------------------------------------------------
#endif
