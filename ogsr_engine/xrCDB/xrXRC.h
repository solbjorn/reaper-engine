// xrXRC.h: interface for the xrXRC class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "xrCDB.h"

#ifdef DEBUG
extern CStatTimer* cdb_clRAY; // total: ray-testing
extern CStatTimer* cdb_clBOX; // total: box query
extern CStatTimer* cdb_clFRUSTUM; // total: frustum query
#endif

class xrXRC
{
    CDB::COLLIDER CL;

public:
    IC void ray_query(u32 options, const CDB::MODEL* m_def, const Fvector& r_start, const Fvector& r_dir, float r_range = 10000.f)
    {
#ifdef DEBUG
        cdb_clRAY->Begin();
#endif

        CL.ray_query(options, m_def, r_start, r_dir, r_range);

#ifdef DEBUG
        cdb_clRAY->End();
#endif
    }

    IC void box_query(u32 options, const CDB::MODEL* m_def, const Fvector& b_center, const Fvector& b_dim)
    {
#ifdef DEBUG
        cdb_clBOX->Begin();
#endif

        CL.box_query(options, m_def, b_center, b_dim);

#ifdef DEBUG
        cdb_clBOX->End();
#endif
    }

    IC void frustum_query(u32 options, const CDB::MODEL* m_def, const CFrustum& F)
    {
#ifdef DEBUG
        cdb_clFRUSTUM->Begin();
#endif

        CL.frustum_query(options, m_def, F);

#ifdef DEBUG
        cdb_clFRUSTUM->End();
#endif
    }

    [[nodiscard]] constexpr CDB::RESULT* r_begin() { return CL.r_begin(); }
    [[nodiscard]] constexpr const CDB::RESULT* r_begin() const { return CL.r_begin(); }

    [[nodiscard]] constexpr xr_vector<CDB::RESULT>* r_get() { return CL.r_get(); }
    [[nodiscard]] constexpr const xr_vector<CDB::RESULT>* r_get() const { return CL.r_get(); }

    [[nodiscard]] constexpr auto r_count() const { return CL.r_count(); }
    [[nodiscard]] constexpr bool r_empty() const { return CL.r_empty(); }

    constexpr void r_free() { CL.r_free(); }
    constexpr void r_clear() { CL.r_clear(); }
    constexpr void r_clear_compact() { CL.r_clear_compact(); }

    xrXRC() = default;
    xrXRC(const xrXRC&) = default;
    xrXRC(xrXRC&&) = default;
    ~xrXRC() = default;

    xrXRC& operator=(const xrXRC&) = default;
    xrXRC& operator=(xrXRC&&) = default;
};

extern xrXRC XRC;
