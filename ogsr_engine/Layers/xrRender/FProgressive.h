// FProgressive.h: interface for the FProgressive class.
//
//////////////////////////////////////////////////////////////////////

#ifndef FProgressiveH
#define FProgressiveH

#include "FVisual.h"

struct FSlideWindowItem;

class FProgressive : public Fvisual
{
    RTTI_DECLARE_TYPEINFO(FProgressive, Fvisual);

protected:
    FSlideWindowItem nSWI;
    FSlideWindowItem* xSWI{};
    u32 last_lod{};

public:
    FProgressive();
    ~FProgressive() override;

    virtual void Render(CBackend& cmd_list, float LOD, bool use_fast_geo) override; // LOD - Level Of Detail  [0.0f - min, 1.0f - max], -1 = Ignored
    virtual void Load(const char* N, IReader* data, u32 dwFlags);
    virtual void Copy(dxRender_Visual* pFrom);
    virtual void Release();
};

#endif
