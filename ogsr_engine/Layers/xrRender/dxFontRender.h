#ifndef dxFontRender_included
#define dxFontRender_included

#include "..\..\Include\xrRender\FontRender.h"

class dxFontRender : public IFontRender
{
    RTTI_DECLARE_TYPEINFO(dxFontRender, IFontRender);

public:
    dxFontRender();
    ~dxFontRender() override;

    [[nodiscard]] gsl::index Initialize(gsl::czstring shader, gsl::czstring font) override;
    void OnRender(CGameFont& owner) override;

private:
    ref_shader pShader;
    ref_geom pGeom;
};

#endif //	FontRender_included
