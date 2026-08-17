#ifndef DetailModelH
#define DetailModelH

#include "IRenderDetailModel.h"

class CDetail final : public IRender_DetailModel
{
    RTTI_DECLARE_TYPEINFO(CDetail, IRender_DetailModel);

public:
    ~CDetail() override;

    void Load(IReader* S);
    void Optimize();
    void Unload();

    void transfer(Fmatrix& mXform, std::span<fvfVertexOut> vDest, u32 C, std::span<u16> iDest, u32 iOffset, f32 du = 0.0f, f32 dv = 0.0f) const override;
};

#endif
