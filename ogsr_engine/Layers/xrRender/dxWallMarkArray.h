#ifndef dxWallMarkArray_included
#define dxWallMarkArray_included

#include "..\..\Include\xrRender\WallMarkArray.h"

class dxWallMarkArray : public IWallMarkArray
{
    RTTI_DECLARE_TYPEINFO(dxWallMarkArray, IWallMarkArray);

public:
    ~dxWallMarkArray() override;

    void Copy(IWallMarkArray& _in) override;

    void AppendMark(gsl::czstring s_textures) override;
    void clear() override;
    [[nodiscard]] bool empty() override;
    [[nodiscard]] wm_shader GenerateWallmark() override;

    ref_shader* dxGenerateWallmark();

private:
    DEFINE_VECTOR(ref_shader, ShaderVec, ShaderIt);

    ShaderVec m_CollideMarks;
};

#endif //	WallMarkArray_included
