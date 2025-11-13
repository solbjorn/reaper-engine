#pragma once

#include "xrcdb.h"

class CObject;

namespace collide
{
struct tri
{
    Fvector e10;
    float e10s;
    Fvector e21;
    float e21s;
    Fvector e02;
    float e02s;
    Fvector p[3];
    Fvector N;
    float d;
};

struct elipsoid
{
    Fmatrix mL2W; // convertion from sphere(000,1) to real space
    Fmatrix mW2L; // convertion from real space to sphere(000,1)
};

struct ray_cache
{
    // previous state
    Fvector start{};
    Fvector dir{};
    f32 range{};
    bool result{};

    // cached vertices
    Fvector verts[3]{};

    constexpr ray_cache() = default;

    constexpr void set(const Fvector& _start, const Fvector& _dir, f32 _range, bool _result)
    {
        start = _start;
        dir = _dir;
        range = _range;
        result = _result;
    }

    [[nodiscard]] constexpr bool similar(const Fvector& _start, const Fvector& _dir, f32 _range)
    {
        if (!_start.similar(start))
            return false;

        if (!fsimilar(1.0f, dir.dotproduct(_dir)))
            return false;

        if (!fsimilar(_range, range))
            return false;

        return true;
    }
};

enum rq_target
{
    rqtNone = (0),
    rqtObject = (1 << 0),
    rqtStatic = (1 << 1),
    rqtShape = (1 << 2),
    rqtObstacle = (1 << 3),
    rqtBoth = (rqtObject | rqtStatic),
    rqtDyn = (rqtObject | rqtShape | rqtObstacle)
};

struct ray_defs
{
    Fvector start;
    Fvector dir;
    f32 range;
    u32 flags;
    rq_target tgt;

    constexpr explicit ray_defs(const Fvector& _start, const Fvector& _dir, f32 _range, u32 _flags, rq_target _tgt)
    {
        start = _start;
        dir = _dir;
        range = _range;
        flags = _flags;
        tgt = _tgt;
    }
};

struct XR_TRIVIAL alignas(16) rq_result
{
    CObject* O; // if NULL - static
    f32 range; // range to intersection
    s32 element; // номер кости/номер треугольника

    constexpr rq_result() = default;
    constexpr explicit rq_result(CObject* obj, f32 r, s32 elem) : O{obj}, range{r}, element{elem} {}

    constexpr rq_result(const rq_result& that) { xr_memcpy16(this, &that); }

#ifdef XR_TRIVIAL_BROKEN
    constexpr rq_result(rq_result&&) = default;
#else
    constexpr rq_result(rq_result&& that) { xr_memcpy16(this, &that); }
#endif

    constexpr rq_result& operator=(const rq_result& that)
    {
        xr_memcpy16(this, &that);
        return *this;
    }

#ifdef XR_TRIVIAL_BROKEN
    constexpr rq_result& operator=(rq_result&&) = default;
#else
    constexpr rq_result& operator=(rq_result&& that)
    {
        xr_memcpy16(this, &that);
        return *this;
    }
#endif

    constexpr rq_result& set(CObject* _O, f32 _range, s32 _element)
    {
        O = _O;
        range = _range;
        element = _element;

        return *this;
    }

    [[nodiscard]] constexpr bool set_if_less(const CDB::RESULT& I)
    {
        if (I.range < range)
        {
            set(nullptr, I.range, I.id);
            return true;
        }

        return false;
    }

    [[nodiscard]] constexpr bool set_if_less(const rq_result& R)
    {
        if (R.range < range)
        {
            set(R.O, R.range, R.element);
            return true;
        }

        return false;
    }

    [[nodiscard]] constexpr bool set_if_less(CObject* _who, f32 _range, s32 _element)
    {
        if (_range < range)
        {
            set(_who, _range, _element);
            return true;
        }

        return false;
    }

    [[nodiscard]] constexpr bool valid() const { return element >= 0; }
};
static_assert(sizeof(rq_result) == 16);
XR_TRIVIAL_ASSERT(rq_result);

using rqVec = xr_vector<rq_result>;

class rq_results
{
protected:
    rqVec results;

public:
    [[nodiscard]] constexpr bool append_result(CObject* _who, f32 _range, s32 _element, bool bNearest)
    {
        if (bNearest && !results.empty())
        {
            rq_result& R = results.back();
            if (_range < R.range)
            {
                R.O = _who;
                R.range = _range;
                R.element = _element;

                return true;
            }

            return false;
        }

        results.emplace_back(_who, _range, _element);

        return true;
    }

    constexpr void append_result(const CDB::RESULT& res)
    {
        if (results.capacity() == 0)
            results.reserve(8);

        results.emplace_back(nullptr, res.range, res.id);
    }

    constexpr void append_result(const rq_result& res)
    {
        if (results.capacity() == 0)
            results.reserve(8);

        results.emplace_back(res);
    }

    constexpr void append_result(rq_result&& res)
    {
        if (results.capacity() == 0)
            results.reserve(8);

        results.emplace_back(std::move(res));
    }

    [[nodiscard]] constexpr auto r_count() const { return std::ssize(results); }
    [[nodiscard]] constexpr bool r_empty() const { return results.empty(); }

    [[nodiscard]] constexpr rq_result* r_begin() { return results.data(); }
    [[nodiscard]] constexpr const rq_result* r_begin() const { return results.data(); }

    [[nodiscard]] constexpr rqVec* r_get() { return &results; }
    [[nodiscard]] constexpr const rqVec* r_get() const { return &results; }

    constexpr void r_clear() { results.clear(); }

    constexpr void r_sort()
    {
        std::ranges::sort(results, [](const rq_result& a, const rq_result& b) { return a.range < b.range; });
    }

    [[nodiscard]] constexpr rqVec& r_results() { return results; }
    [[nodiscard]] constexpr const rqVec& r_results() const { return results; }
};

typedef BOOL rq_callback(rq_result& result, LPVOID user_data);
typedef BOOL test_callback(const ray_defs& rd, CObject* object, LPVOID user_data);
} // namespace collide
