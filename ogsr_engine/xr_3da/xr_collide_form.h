#ifndef __XR_COLLIDE_FORM_H__
#define __XR_COLLIDE_FORM_H__

#include "../xrcdb/xr_collide_defs.h"
#include "vismask.h"

// refs
class CObject;
class CInifile;

// t-defs
constexpr inline u32 clGET_TRIS{1 << 0};
constexpr inline u32 clGET_BOXES{1 << 1};
constexpr inline u32 clGET_SPHERES{1 << 2};
constexpr inline u32 clQUERY_ONLYFIRST{1 << 3}; // stop if was any collision
constexpr inline u32 clQUERY_TOPLEVEL{1 << 4}; // get only top level of model box/sphere
constexpr inline u32 clQUERY_STATIC{1 << 5}; // static
constexpr inline u32 clQUERY_DYNAMIC{1 << 6}; // dynamic
constexpr inline u32 clCOARSE{1 << 7}; // coarse test (triangles vs obb)

struct XR_TRIVIAL alignas(16) clQueryTri
{
    Fvector p[3];
    const CDB::TRI* T;

    constexpr clQueryTri() = default;

    constexpr clQueryTri(const clQueryTri& that) { xr_memcpy128(this, &that, sizeof(that)); }

#ifdef XR_TRIVIAL_BROKEN
    constexpr clQueryTri(clQueryTri&&) = default;
#else
    constexpr clQueryTri(clQueryTri&& that) { xr_memcpy128(this, &that, sizeof(that)); }
#endif

    constexpr clQueryTri& operator=(const clQueryTri& that)
    {
        xr_memcpy128(this, &that, sizeof(that));
        return *this;
    }

#ifdef XR_TRIVIAL_BROKEN
    constexpr clQueryTri& operator=(clQueryTri&&) = default;
#else
    constexpr clQueryTri& operator=(clQueryTri&& that)
    {
        xr_memcpy128(this, &that, sizeof(that));
        return *this;
    }
#endif
};
XR_TRIVIAL_ASSERT(clQueryTri);

struct clQueryCollision
{
    xr_vector<CObject*> objects; // affected objects
    xr_vector<clQueryTri> tris; // triangles		(if queried)
    xr_vector<Fobb> boxes; // boxes/ellipsoids	(if queried)
    xr_vector<Fvector4> spheres; // spheres			(if queried)

    IC void Clear()
    {
        objects.clear();
        tris.clear();
        boxes.clear();
        spheres.clear();
    }
    IC void AddTri(const Fmatrix& m, const CDB::TRI* one, const Fvector* verts)
    {
        clQueryTri T;
        m.transform_tiny(T.p[0], verts[one->verts[0]]);
        m.transform_tiny(T.p[1], verts[one->verts[1]]);
        m.transform_tiny(T.p[2], verts[one->verts[2]]);
        T.T = one;
        tris.push_back(T);
    }
    IC void AddTri(const CDB::TRI* one, const Fvector* verts)
    {
        clQueryTri T;
        T.p[0] = verts[one->verts[0]];
        T.p[1] = verts[one->verts[1]];
        T.p[2] = verts[one->verts[2]];
        T.T = one;
        tris.push_back(T);
    }
    IC void AddBox(const Fmatrix& M, const Fbox& B)
    {
        Fobb box;
        Fvector c;
        B.getcenter(c);
        B.getradius(box.m_halfsize);

        Fmatrix T, R;
        T.translate(c);
        R.mul_43(M, T);

        box.xform_set(R);
        boxes.push_back(box);
    }
    IC void AddBox(const Fobb& B) { boxes.push_back(B); }
};

enum ECollisionFormType
{
    cftObject,
    cftShape
};

class XR_NOVTABLE ICollisionForm : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(ICollisionForm);

private:
    friend class CObjectSpace;

protected:
    CObject* owner; // владелец
    u32 dwQueryID{};

    Fbox bv_box; // (Local) BBox объекта
    Fsphere bv_sphere; // (Local) Sphere

private:
    ECollisionFormType m_type;

    [[nodiscard]] virtual BOOL _RayQuery(const collide::ray_defs& Q, collide::rq_results& R) = 0;

public:
    explicit ICollisionForm(CObject* _owner, ECollisionFormType tp);
    ~ICollisionForm() override = 0;

    [[nodiscard]] BOOL RayQuery(collide::rq_results& dest, const collide::ray_defs& rq)
    {
        dest.r_clear();
        return _RayQuery(rq, dest);
    }

    IC CObject* Owner() const { return owner; }
    const Fbox& getBBox() const { return bv_box; }
    float getRadius() const { return bv_sphere.R; }
    const Fsphere& getSphere() const { return bv_sphere; }
    ECollisionFormType Type() const { return m_type; }
};

inline ICollisionForm::~ICollisionForm() = default;

class CCF_Skeleton : public ICollisionForm
{
    RTTI_DECLARE_TYPEINFO(CCF_Skeleton, ICollisionForm);

public:
#ifdef XR_TRIVIAL_BROKEN
    XR_DIAG_PUSH();
    XR_DIAG_IGNORE("-Wignored-attributes");
#endif

    struct XR_TRIVIAL SElement
    {
        union
        {
            struct
            {
                Fmatrix b_IM; // world 2 bone xform
                Fvector b_hsize;
            };
            struct
            {
                Fsphere s_sphere;
            };
            struct
            {
                Fcylinder c_cylinder;
            };
        };

        u16 type;
        u16 elem_id;

        constexpr SElement() : type{0}, elem_id{std::numeric_limits<u16>::max()} {}
        constexpr explicit SElement(u16 id, u16 t) : type{t}, elem_id{id} {}

        constexpr SElement(const SElement& that) { xr_memcpy128(this, &that, sizeof(that)); }

#ifdef XR_TRIVIAL_BROKEN
        constexpr SElement(SElement&&) = default;
#else
        constexpr SElement(SElement&& that) { xr_memcpy128(this, &that, sizeof(that)); }
#endif

        constexpr SElement& operator=(const SElement& that)
        {
            xr_memcpy128(this, &that, sizeof(that));
            return *this;
        }

#ifdef XR_TRIVIAL_BROKEN
        constexpr SElement& operator=(SElement&&) = default;
#else
        constexpr SElement& operator=(SElement&& that)
        {
            xr_memcpy128(this, &that, sizeof(that));
            return *this;
        }
#endif

        [[nodiscard]] BOOL valid() const { return (elem_id != (u16(-1))) && (type != 0); }
        void center(Fvector& center) const;
    };
    XR_TRIVIAL_ASSERT(SElement);

#ifdef XR_TRIVIAL_BROKEN
    XR_DIAG_POP();
#endif

    DEFINE_VECTOR(SElement, ElementVec, ElementVecIt);

private:
    VisMask vis_mask;
    ElementVec elements;

    u32 dwFrame{}; // The model itself
    u32 dwFrameTL{}; // Top level

    void BuildState();
    void BuildTopLevel();

public:
    explicit CCF_Skeleton(CObject* _owner);
    ~CCF_Skeleton() override = default;

    [[nodiscard]] BOOL _RayQuery(const collide::ray_defs& Q, collide::rq_results& R) override;

    bool _ElementCenter(u16 elem_id, Fvector& e_center);
    const ElementVec& _GetElements() { return elements; }

    void Calculate();

    void _dbg_refresh()
    {
        BuildTopLevel();
        BuildState();
    }
};

class CCF_Shape : public ICollisionForm
{
    RTTI_DECLARE_TYPEINFO(CCF_Shape, ICollisionForm);

public:
    union XR_TRIVIAL shape_data
    {
        Fsphere sphere;
        struct
        {
            Fmatrix box;
            Fmatrix ibox;
        };

        constexpr shape_data() = default;

        constexpr shape_data(const shape_data& that) { xr_memcpy128(this, &that, sizeof(that)); }

#ifdef XR_TRIVIAL_BROKEN
        constexpr shape_data(shape_data&&) = default;
#else
        constexpr shape_data(shape_data&& that) { xr_memcpy128(this, &that, sizeof(that)); }
#endif

        constexpr shape_data& operator=(const shape_data& that)
        {
            xr_memcpy128(this, &that, sizeof(that));
            return *this;
        }

#ifdef XR_TRIVIAL_BROKEN
        constexpr shape_data& operator=(shape_data&&) = default;
#else
        constexpr shape_data& operator=(shape_data&& that)
        {
            xr_memcpy128(this, &that, sizeof(that));
            return *this;
        }
#endif
    };
    XR_TRIVIAL_ASSERT(shape_data);

    struct XR_TRIVIAL shape_def
    {
        int type;
        shape_data data;

        constexpr shape_def() = default;

        constexpr shape_def(const shape_def& that) { xr_memcpy128(this, &that, sizeof(that)); }

#ifdef XR_TRIVIAL_BROKEN
        constexpr shape_def(shape_def&&) = default;
#else
        constexpr shape_def(shape_def&& that) { xr_memcpy128(this, &that, sizeof(that)); }
#endif

        constexpr shape_def& operator=(const shape_def& that)
        {
            xr_memcpy128(this, &that, sizeof(that));
            return *this;
        }

#ifdef XR_TRIVIAL_BROKEN
        constexpr shape_def& operator=(shape_def&&) = default;
#else
        constexpr shape_def& operator=(shape_def&& that)
        {
            xr_memcpy128(this, &that, sizeof(that));
            return *this;
        }
#endif
    };
    XR_TRIVIAL_ASSERT(shape_def);

    xr_vector<shape_def> shapes;

    explicit CCF_Shape(CObject* _owner);
    ~CCF_Shape() override = default;

    [[nodiscard]] BOOL _RayQuery(const collide::ray_defs& Q, collide::rq_results& R) override;

    void add_sphere(Fsphere& S);
    void add_box(Fmatrix& B);

    void ComputeBounds();

    BOOL Contact(CObject* O);
    xr_vector<shape_def>& Shapes() { return shapes; }
};

#endif //__XR_COLLIDE_FORM_H__
