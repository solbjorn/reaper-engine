// Portal.h: interface for the CPortal class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _PORTAL_H_
#define _PORTAL_H_

class CPortal;
class CSector;

struct XR_TRIVIAL _scissor : public Fbox2
{
    f32 depth;

    constexpr _scissor() = default;

    constexpr _scissor(const _scissor& that) { xr::memcpy64(this, &that, sizeof(that)); }

#ifdef XR_TRIVIAL_BROKEN
    constexpr _scissor(_scissor&&) = default;
#else
    constexpr _scissor(_scissor&& that) { xr::memcpy64(this, &that, sizeof(that)); }
#endif

    constexpr _scissor& operator=(const _scissor& that)
    {
        xr::memcpy64(this, &that, sizeof(that));
        return *this;
    }

#ifdef XR_TRIVIAL_BROKEN
    constexpr _scissor& operator=(_scissor&&) = default;
#else
    constexpr _scissor& operator=(_scissor&& that)
    {
        xr::memcpy64(this, &that, sizeof(that));
        return *this;
    }
#endif
};
static_assert(sizeof(_scissor) == 24);
XR_TRIVIAL_ASSERT(_scissor);

// Connector
class CPortal : public virtual RTTI::Enable
#ifdef DEBUG
    ,
                public pureRender
#endif
{
    RTTI_DECLARE_TYPEINFO(CPortal
#ifdef DEBUG
                          ,
                          pureRender
#endif
    );

public:
    using Poly = svector<Fvector, 6>;
    struct level_portal_data_t
    {
        u16 sector_front;
        u16 sector_back;
        Poly vertices;
    };
    static_assert(sizeof(level_portal_data_t) == 80);

private:
    Poly poly;
    CSector *pFace, *pBack;

public:
    Fplane P;
    Fsphere S;
    u32 marker;
    BOOL bDualRender;

    void setup(const level_portal_data_t& data, const xr_vector<CSector*>& portals);

    [[nodiscard]] Poly& getPoly() { return poly; }
    [[nodiscard]] const Poly& getPoly() const { return poly; }

    [[nodiscard]] CSector* Back() const { return pBack; }
    [[nodiscard]] CSector* Front() const { return pFace; }
    [[nodiscard]] CSector* getSector(const CSector* pFrom) const { return pFrom == pFace ? pBack : pFace; }

    [[nodiscard]] CSector* getSectorFacing(const Fvector& V) const
    {
        if (P.classify(V) > 0)
            return pFace;
        else
            return pBack;
    }

    [[nodiscard]] CSector* getSectorBack(const Fvector& V) const
    {
        if (P.classify(V) > 0)
            return pBack;
        else
            return pFace;
    }

    [[nodiscard]] f32 distance(const Fvector& V) const { return _abs(P.classify(V)); }

    CPortal();
    ~CPortal() override;

#ifdef DEBUG
    [[nodiscard]] tmc::task<void> OnRender() override;
#endif
};

class dxRender_Visual;

// Main 'Sector' class
class CSector : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CSector);

public:
    struct level_sector_data_t
    {
        xr_vector<u32> portals_id;
        u32 root_id;
    };

    sector_id_t unique_id{INVALID_SECTOR_ID};

protected:
    dxRender_Visual* m_root{}; // whole geometry of that sector

public:
    xr_vector<CPortal*> m_portals;
    xr_vector<CFrustum> r_frustums;
    xr_vector<_scissor> r_scissors;
    _scissor r_scissor_merged;
    u32 r_marker;

public:
    // Main interface
    [[nodiscard]] dxRender_Visual* root() const { return m_root; }
    void setup(const level_sector_data_t& data, const xr_vector<CPortal*>& portals);

    CSector() = default;
    ~CSector() override = default;
};

class CPortalTraverser
{
public:
    enum
    {
        VQ_HOM = (1 << 0),
        VQ_SSA = (1 << 1),
        VQ_SCISSOR = (1 << 2),
        VQ_FADE = (1 << 3), // requires SSA to work
    };

public:
    u32 i_marker{std::numeric_limits<u32>::max()}; // input
    u32 i_options; // input:	culling options
    Fvector i_vBase; // input:	"view" point
    Fmatrix i_mXFORM; // input:	4x4 xform
    Fmatrix i_mXFORM_01; //
    CSector* i_start; // input:	starting point
    xr_vector<CSector*> r_sectors; // result
    xr_vector<std::pair<CPortal*, float>> f_portals; //

public:
    CPortalTraverser();

    void traverse(CSector* sector, CFrustum& F, Fvector& vBase, Fmatrix& mXFORM, u32 options);
    void traverse_sector(CSector* sector, CFrustum& F, _scissor& R);
    void fade_portal(CPortal* _p, float ssa);
    void fade_render();

#ifdef DEBUG
    void dbg_draw();
#endif
};

#endif // !defined(AFX_PORTAL_H__1FC2D371_4A19_49EA_BD1E_2D0F8DEBBF15__INCLUDED_)
