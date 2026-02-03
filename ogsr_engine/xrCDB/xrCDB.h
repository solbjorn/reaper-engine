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
    constexpr explicit TRI(u32 v1, u32 v2, u32 v3, u32 d) : verts{v1, v2, v3}, dummy{d} {}

    constexpr explicit TRI(u32 v1, u32 v2, u32 v3, u16 mat, u16 sec) : verts{v1, v2, v3}
    {
        material = mat;
        suppress_shadows = 0;
        suppress_wm = 0;
        sector = sec;
    }

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

    [[nodiscard]] constexpr auto IDvert(gsl::index ID) const { return verts[ID]; }
};
static_assert(sizeof(TRI) == 16);
XR_TRIVIAL_ASSERT(TRI);

// Build callback
using build_callback = void(std::span<Fvector> V, std::span<TRI> T, void* params);
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
    Opcode::Model* tree{};
    u32 status{S_INIT}; // 0=ready, 1=init, 2=building

    // tris
    TRI* tris{};
    gsl::index tris_count{};
    Fvector* verts{};
    gsl::index verts_count{};

public:
    MODEL();
    ~MODEL();

    [[nodiscard]] constexpr Fvector* get_verts() { return verts; }
    [[nodiscard]] constexpr const Fvector* get_verts() const { return verts; }
    [[nodiscard]] constexpr auto get_verts_count() const { return verts_count; }

    [[nodiscard]] constexpr TRI* get_tris() { return tris; }
    [[nodiscard]] constexpr const TRI* get_tris() const { return tris; }
    [[nodiscard]] constexpr auto get_tris_count() const { return tris_count; }

    void syncronize() const
    {
        if (S_READY != status)
            Log("! WARNING: syncronized CDB::query");
    }

    void build_internal(std::span<const Fvector> V, std::span<const TRI> T, build_callback* bc = nullptr, void* bcp = nullptr);
    void build(std::span<const Fvector> V, std::span<const TRI> T, build_callback* bc = nullptr, void* bcp = nullptr);
    [[nodiscard]] gsl::index memory() const;

    [[nodiscard]] bool serialize(const char* file, u64 xxh, serialize_callback callback = nullptr) const;
    [[nodiscard]] bool deserialize(const char* file, u64 xxh, deserialize_callback callback = nullptr);

private:
    void serialize_tree(IWriter* stream) const;
    [[nodiscard]] bool deserialize_tree(IReader* stream);
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
    s32 id;
    f32 range;
    f32 u, v;
    u64 pad;

    constexpr RESULT() = default;
    constexpr explicit RESULT(s32 i, const Fvector& v0, const Fvector& v1, const Fvector& v2, u32 d) : verts{v0, v1, v2}, dummy{d}, id{i} {}
    constexpr explicit RESULT(s32 i, f32 r, f32 uu, f32 vv, const Fvector& v0, const Fvector& v1, const Fvector& v2, u32 d)
        : verts{v0, v1, v2}, dummy{d}, id{i}, range{r}, u{uu}, v{vv}
    {}

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
    constexpr COLLIDER() = default;
    constexpr ~COLLIDER() { r_free(); }

    constexpr COLLIDER(const COLLIDER&) = default;
    constexpr COLLIDER(COLLIDER&&) = default;

    constexpr COLLIDER& operator=(const COLLIDER&) = default;
    constexpr COLLIDER& operator=(COLLIDER&&) = default;

    void ray_query(u32 ray_mode, const MODEL* m_def, const Fvector& r_start, const Fvector& r_dir, float r_range = 10000.f);
    void box_query(u32 box_mode, const MODEL* m_def, const Fvector& b_center, const Fvector& b_dim);
    void frustum_query(u32 frustum_mode, const MODEL* m_def, const CFrustum& F);

    [[nodiscard]] constexpr RESULT* r_begin() { return rd.data(); }
    [[nodiscard]] constexpr const RESULT* r_begin() const { return rd.data(); }
    [[nodiscard]] constexpr xr_vector<RESULT>* r_get() { return &rd; }
    [[nodiscard]] constexpr const xr_vector<RESULT>* r_get() const { return &rd; }

    template <typename... Args>
    constexpr RESULT& r_add(Args&&... args)
    {
        return rd.emplace_back(std::forward<Args>(args)...);
    }

    constexpr void r_free() { rd.clear(); }

    [[nodiscard]] constexpr gsl::index r_count() const { return std::ssize(rd); }
    [[nodiscard]] constexpr bool r_empty() const { return rd.empty(); }

    constexpr void r_clear() { rd.clear(); }
    constexpr void r_clear_compact() { rd.clear(); }
};

//
class Collector
{
    xr_vector<Fvector> verts;
    xr_vector<TRI> faces;

    [[nodiscard]] gsl::index VPack(const Fvector& V, float eps);

public:
    void add_face(const Fvector& v0, const Fvector& v1, const Fvector& v2, u16 material, u16 sector);

    void add_face_D(const Fvector& v0, const Fvector& v1, const Fvector& v2, u32 dummy);
    void add_face_packed(const Fvector& v0, const Fvector& v1, const Fvector& v2, u16 material, u16 sector, float eps = EPS);
    void add_face_packed_D(const Fvector& v0, const Fvector& v1, const Fvector& v2, u32 dummy, float eps = EPS);
    void calc_adjacency(xr_vector<u32>& dest) const;

    [[nodiscard]] constexpr Fvector* getV() { return verts.data(); }
    [[nodiscard]] constexpr const Fvector* getV() const { return verts.data(); }
    [[nodiscard]] constexpr gsl::index getVS() const { return std::ssize(verts); }
    [[nodiscard]] constexpr auto get_verts() { return std::span{verts}; }
    [[nodiscard]] constexpr auto get_verts() const { return std::span{verts}; }

    [[nodiscard]] constexpr TRI* getT() { return faces.data(); }
    [[nodiscard]] constexpr const TRI* getT() const { return faces.data(); }
    [[nodiscard]] constexpr gsl::index getTS() const { return std::ssize(faces); }
    [[nodiscard]] constexpr auto get_faces() { return std::span{faces}; }
    [[nodiscard]] constexpr auto get_faces() const { return std::span{faces}; }

    constexpr void clear()
    {
        verts.clear();
        faces.clear();
    }
};

class CollectorPacked : public Noncopyable
{
private:
    static constexpr gsl::index clpMX{24}, clpMY{16}, clpMZ{24};

    xr_vector<Fvector> verts;
    xr_vector<TRI> faces;

    Fvector VMmin, VMscale;
    xr_vector<u32> VM[clpMX + 1][clpMY + 1][clpMZ + 1];
    Fvector VMeps;

    [[nodiscard]] u32 VPack(const Fvector& V);

public:
    explicit CollectorPacked(const Fbox& bb, gsl::index apx_vertices = 5000, gsl::index apx_faces = 5000);

    void add_face(const Fvector& v0, const Fvector& v1, const Fvector& v2, u16 material, u16 sector);
    void add_face_D(const Fvector& v0, const Fvector& v1, const Fvector& v2, u32 dummy);

    [[nodiscard]] constexpr xr_vector<Fvector>& getV_Vec() { return verts; }
    [[nodiscard]] constexpr Fvector* getV() { return verts.data(); }
    [[nodiscard]] constexpr const Fvector* getV() const { return verts.data(); }
    [[nodiscard]] constexpr gsl::index getVS() const { return std::ssize(verts); }

    [[nodiscard]] constexpr TRI* getT() { return faces.data(); }
    [[nodiscard]] constexpr const TRI* getT() const { return faces.data(); }
    [[nodiscard]] constexpr gsl::index getTS() const { return std::ssize(faces); }

    void clear();
};
} // namespace CDB
