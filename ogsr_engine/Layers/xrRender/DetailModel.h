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

    virtual void transfer(Fmatrix& mXform, fvfVertexOut* vDest, u32 C, u16* iDest, u32 iOffset);
    virtual void transfer(Fmatrix& mXform, fvfVertexOut* vDest, u32 C, u16* iDest, u32 iOffset, float du, float dv);
};

#endif
