#pragma once

#include "../xrCDB/xr_collide_defs.h"
#include "Render.h"
#include "pure_relcase.h"

class CObject;
class ISpatial;

namespace Feel
{
constexpr inline f32 fuzzy_update_vis{1000.0f}; // speed of fuzzy-logic desisions
constexpr inline f32 fuzzy_update_novis{1000.0f}; // speed of fuzzy-logic desisions
constexpr inline f32 fuzzy_guaranteed{0.001f}; // distance which is supposed 100% visible
constexpr inline f32 lr_granularity{0.1f}; // assume similar positions

class XR_NOVTABLE Vision : public virtual RTTI::Enable, private pure_relcase
{
    RTTI_DECLARE_TYPEINFO(Vision);

public:
    friend class pure_relcase;

private:
    xr_vector<CObject*> seen;
    xr_vector<CObject*> query;
    xr_vector<CObject*> diff;

    xr_vector<ISpatial*> r_spatial;
    collide::rq_results RQR;

    void o_new(CObject* E);
    void o_delete(CObject* E);
    void o_trace(Fvector& P, float dt, float vis_threshold);

public:
    Vision();
    ~Vision() override;

    struct feel_visible_Item
    {
        CObject* O{};
        f32 fuzzy{}; // note range: (-1[no]..1[yes])
        f32 Cache_vis{};
        collide::ray_cache Cache;

        Fvector cp_LP{};
        Fvector cp_LR_src{};
        Fvector cp_LR_dst{};
        Fvector cp_LAST{}; // last point found to be visible

        f32 trans{};

        constexpr feel_visible_Item() = default;
    };
    xr_vector<feel_visible_Item> feel_visible;

public:
    void feel_vision_clear();
    void feel_vision_query(Fmatrix& mFull);
    void feel_vision_update(CObject* parent, Fvector& P, float dt, float vis_threshold);
    void feel_vision_relcase(CObject* object);

    void feel_vision_get(xr_vector<CObject*>& R) const
    {
        R.clear();

        for (auto& item : feel_visible)
        {
            if (positive(item.fuzzy))
                R.emplace_back(item.O);
        }
    }

    [[nodiscard]] Fvector feel_vision_get_vispoint(const CObject* _O) const
    {
        const auto it = std::ranges::find(feel_visible, _O, &feel_visible_Item::O);
        XR_ASSERT(it != feel_visible.end() && positive(it->fuzzy));
        return it->cp_LAST;
    }

    [[nodiscard]] virtual BOOL feel_vision_isRelevant(CObject* O) = 0;
    [[nodiscard]] virtual f32 feel_vision_mtl_transp(CObject* O, u32 element) = 0;

    float feel_vision_get_transparency(const CObject* _O) const;
};
} // namespace Feel
