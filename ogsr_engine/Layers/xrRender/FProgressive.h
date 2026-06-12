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

    void Render(CBackend& cmd_list, f32 LOD, bool use_fast_geo) override; // LOD - Level Of Detail  [0.0f - min, 1.0f - max], -1 = Ignored
    void Load(gsl::czstring N, IReader* data, u32 dwFlags) override;
    void Copy(dxRender_Visual* pFrom) override;
    void Release() override;
};

#endif
