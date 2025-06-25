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
    union shape_data
    {
        Fsphere sphere;
        Fmatrix box;

        constexpr inline shape_data() = default;
        constexpr inline shape_data(const shape_data& d) { box = d.box; }

        constexpr inline shape_data& operator=(const shape_data& d)
        {
            box = d.box;
            return *this;
        }
    };
    struct shape_def
    {
        u8 type;
        shape_data data;

        constexpr inline shape_def() = default;
        constexpr inline shape_def(const shape_def& d) { xr_memcpy128(this, &d, sizeof(d)); }

        constexpr inline shape_def& operator=(const shape_def& d)
        {
            xr_memcpy128(this, &d, sizeof(d));
            return *this;
        }
    };
    DEFINE_VECTOR(shape_def, ShapeVec, ShapeIt);
    ShapeVec shapes;
};

#endif
