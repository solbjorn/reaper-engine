#ifndef WallMarkArray_included
#define WallMarkArray_included

#include "FactoryPtr.h"
#include "UIShader.h"

typedef FactoryPtr<IUIShader> wm_shader;

class IWallMarkArray : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IWallMarkArray);

public:
    virtual ~IWallMarkArray() { ; }
    virtual void Copy(IWallMarkArray& _in) = 0;

    virtual void AppendMark(LPCSTR s_textures) = 0;
    virtual void clear() = 0;
    virtual bool empty() = 0;
    //	Igor: this function performs unobviouse small memory allocations/
    //	deallocations while generation a return value, so prefere
    //	passing IWallMarkArray directly to renderer.
    virtual wm_shader GenerateWallmark() = 0;
};

#endif //	WallMarkArray_included
