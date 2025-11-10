#pragma once

// forward declarations
class CFrustum;

namespace Opcode
{
class Model;
class AABBNoLeafNode;
} // namespace Opcode

namespace CDB
{
// Triangle
class XR_TRIVIAL alignas(16) TRI //*** 16 bytes total (was 32 :)
{
public:
    u32 verts[3]; // 3*4 = 12b
    union
    {
        u32 dummy; // 4b
        struct
        {
            u32 material : 14; //
            u32 suppress_shadows : 1; //
            u32 suppress_wm : 1; //
            u32 sector : 16; //
        };
    };

    constexpr TRI() = default;

    constexpr TRI(const TRI& that) { xr_memcpy16(this, &that); }

#ifdef XR_TRIVIAL_BROKEN
    constexpr TRI(TRI&&) = default;
#else
    constexpr TRI(TRI&& that) { xr_memcpy16(this, &that); }
#endif

    constexpr TRI& operator=(const TRI& that)
    {
        xr_memcpy16(this, &that);
        return *this;
    }

#ifdef XR_TRIVIAL_BROKEN
    constexpr TRI& operator=(TRI&&) = default;
#else
    constexpr TRI& operator=(TRI&& that)
    {
        xr_memcpy16(this, &that);
        return *this;
    }
#endif

    [[nodiscard]] auto IDvert(size_t ID) const { return verts[ID]; }
};
static_assert(sizeof(TRI) == 16);
XR_TRIVIAL_ASSERT(TRI);

// Build callback
using build_callback = void(Fvector* V, size_t Vcnt, TRI* T, size_t Tcnt, void* params);
using serialize_callback = void(IWriter& writer);
using deserialize_callback = bool(IReader& reader);

class Noncopyable
{
public:
    Noncopyable() = default;
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;
};

// Model definition
class MODEL : Noncopyable
{
    friend class COLLIDER;

    enum : u32
    {
        S_READY = 0,
        S_INIT = 1,
        S_BUILD = 2,
    };

private:
    mutable xrCriticalSection cs;
    Opcode::Model* tree{};
    u32 status{S_INIT}; // 0=ready, 1=init, 2=building

    // tris
    TRI* tris{};
    size_t tris_count{};
    Fvector* verts{};
    size_t verts_count{};

public:
    MODEL();
    ~MODEL();

    IC Fvector* get_verts() { return verts; }
    IC const Fvector* get_verts() const { return verts; }
    IC size_t get_verts_count() const { return verts_count; }
    IC const TRI* get_tris() const { return tris; }
    IC TRI* get_tris() { return tris; }
    IC size_t get_tris_count() const { return tris_count; }
    IC void syncronize() const
    {
        if (S_READY != status)
        {
            Log("! WARNING: syncronized CDB::query");

            cs.Enter();
            cs.Leave();
        }
    }

    void build_internal(const Fvector* V, size_t Vcnt, const TRI* T, size_t Tcnt, build_callback* bc = nullptr, void* bcp = nullptr);
    void build(const Fvector* V, size_t Vcnt, const TRI* T, size_t Tcnt, build_callback* bc = nullptr, void* bcp = nullptr);
    size_t memory();

    bool serialize(const char* file, u64 xxh, serialize_callback callback = nullptr) const;
    bool deserialize(const char* file, u64 xxh, deserialize_callback callback = nullptr);

private:
    void serialize_tree(IWriter* stream) const;
    bool deserialize_tree(IReader* stream);
};

// Collider result
struct XR_TRIVIAL alignas(16) RESULT
{
    Fvector verts[3];
    union
    {
        u32 dummy; // 4b
        struct
        {
            u32 material : 14; //
            u32 suppress_shadows : 1; //
            u32 suppress_wm : 1; //
            u32 sector : 16; //
        };
    };
    int id;
    float range;
    float u, v;
    u64 pad;

    constexpr RESULT() = default;

    constexpr RESULT(const RESULT& that) { xr_memcpy128(this, &that, sizeof(that)); }

#ifdef XR_TRIVIAL_BROKEN
    constexpr RESULT(RESULT&&) = default;
#else
    constexpr RESULT(RESULT&& that) { xr_memcpy128(this, &that, sizeof(that)); }
#endif

    constexpr RESULT& operator=(const RESULT& that)
    {
        xr_memcpy128(this, &that, sizeof(that));
        return *this;
    }

#ifdef XR_TRIVIAL_BROKEN
    constexpr RESULT& operator=(RESULT&&) = default;
#else
    constexpr RESULT& operator=(RESULT&& that)
    {
        xr_memcpy128(this, &that, sizeof(that));
        return *this;
    }
#endif
};
static_assert(sizeof(RESULT) == 64);
XR_TRIVIAL_ASSERT(RESULT);

// Collider Options
enum
{
    OPT_CULL = (1 << 0),
    OPT_ONLYFIRST = (1 << 1),
    OPT_ONLYNEAREST = (1 << 2),
    OPT_FULL_TEST = (1 << 3) // for box & frustum queries - enable class III test(s)
};

// Collider itself
class COLLIDER
{
    // Result management
    xr_vector<RESULT> rd;

public:
    COLLIDER() = default;
    ~COLLIDER();

    COLLIDER(const COLLIDER&) = default;
    COLLIDER(COLLIDER&&) = default;

    COLLIDER& operator=(const COLLIDER&) = default;
    COLLIDER& operator=(COLLIDER&&) = default;

    void ray_query(u32 ray_mode, const MODEL* m_def, const Fvector& r_start, const Fvector& r_dir, float r_range = 10000.f);
    void box_query(u32 box_mode, const MODEL* m_def, const Fvector& b_center, const Fvector& b_dim);
    void frustum_query(u32 frustum_mode, const MODEL* m_def, const CFrustum& F);

    ICF RESULT* r_begin() { return &*rd.begin(); }
    ICF xr_vector<RESULT>* r_get() { return &rd; }
    RESULT& r_add();
    void r_free();
    ICF size_t r_count() { return rd.size(); }
    ICF void r_clear() { rd.clear(); }
    ICF void r_clear_compact() { rd.clear(); }
};

//
class Collector
{
    xr_vector<Fvector> verts;
    xr_vector<TRI> faces;

    u32 VPack(const Fvector& V, float eps);

public:
    void add_face(const Fvector& v0, const Fvector& v1, const Fvector& v2, u16 material, u16 sector);

    void add_face_D(const Fvector& v0, const Fvector& v1, const Fvector& v2, u32 dummy);
    void add_face_packed(const Fvector& v0, const Fvector& v1, const Fvector& v2, u16 material, u16 sector, float eps = EPS);
    void add_face_packed_D(const Fvector& v0, const Fvector& v1, const Fvector& v2, u32 dummy, float eps = EPS);
    void remove_duplicate_T();
    void calc_adjacency(xr_vector<u32>& dest) const;

    Fvector* getV() { return &*verts.begin(); }
    size_t getVS() { return verts.size(); }
    TRI* getT() { return &*faces.begin(); }
    size_t getTS() { return faces.size(); }
    void clear()
    {
        verts.clear();
        faces.clear();
    }
};

constexpr inline u32 clpMX{24}, clpMY{16}, clpMZ{24};

class CollectorPacked : public Noncopyable
{
    using DWORDList = xr_vector<u32>;
    using DWORDIt = DWORDList::iterator;

private:
    xr_vector<Fvector> verts;
    xr_vector<TRI> faces;

    Fvector VMmin, VMscale;
    DWORDList VM[clpMX + 1][clpMY + 1][clpMZ + 1];
    Fvector VMeps;

    u32 VPack(const Fvector& V);

public:
    explicit CollectorPacked(const Fbox& bb, int apx_vertices = 5000, int apx_faces = 5000);

    //		__declspec(noinline) CollectorPacked &operator=	(const CollectorPacked &object)
    //		{
    //			verts
    //		}

    void add_face(const Fvector& v0, const Fvector& v1, const Fvector& v2, u16 material, u16 sector);
    void add_face_D(const Fvector& v0, const Fvector& v1, const Fvector& v2, u32 dummy);

    xr_vector<Fvector>& getV_Vec() { return verts; }
    Fvector* getV() { return &*verts.begin(); }
    size_t getVS() { return verts.size(); }
    TRI* getT() { return &*faces.begin(); }
    size_t getTS() { return faces.size(); }
    void clear();
};
} // namespace CDB
