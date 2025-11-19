#ifndef ShapeDataH
#define ShapeDataH

struct CShapeData : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CShapeData);

public:
    enum
    {
        cfSphere = 0,
        cfBox
    };

    union XR_TRIVIAL shape_data
    {
        Fsphere sphere;
        Fmatrix box;

        constexpr shape_data() = default;

        constexpr shape_data(const shape_data& that) { box = that.box; }

#ifdef XR_TRIVIAL_BROKEN
        constexpr shape_data(shape_data&&) = default;
#else
        constexpr shape_data(shape_data&& that) { box = that.box; }
#endif

        constexpr shape_data& operator=(const shape_data& that)
        {
            box = that.box;
            return *this;
        }

#ifdef XR_TRIVIAL_BROKEN
        constexpr shape_data& operator=(shape_data&&) = default;
#else
        constexpr shape_data& operator=(shape_data&& that)
        {
            box = that.box;
            return *this;
        }
#endif
    };
    XR_TRIVIAL_ASSERT(shape_data);

    struct XR_TRIVIAL shape_def
    {
        u8 type;
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

    DEFINE_VECTOR(shape_def, ShapeVec, ShapeIt);
    ShapeVec shapes;

    ~CShapeData() override = default;
};

#endif
