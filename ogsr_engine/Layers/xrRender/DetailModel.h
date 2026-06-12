#ifndef DetailModelH
#define DetailModelH

#include "IRenderDetailModel.h"

class CDetail : public IRender_DetailModel
{
    RTTI_DECLARE_TYPEINFO(CDetail, IRender_DetailModel);

public:
    ~CDetail() override;

    void Load(IReader* S);
    void Optimize();
    virtual void Unload();

    void transfer(Fmatrix& mXform, fvfVertexOut* vDest, u32 C, u16* iDest, u32 iOffset) override;
    void transfer(Fmatrix& mXform, fvfVertexOut* vDest, u32 C, u16* iDest, u32 iOffset, f32 du, f32 dv) override;
};

#endif
